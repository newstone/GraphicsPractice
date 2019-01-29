#include "stdafx.h"
#include "Player.h"
#include "Camera.h"

inline XMFLOAT4X4 Camera::PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
	return(xmmtx4x4Result);
}
/////////////////////////////////////////////////////////////////////////////////////////

Camera::Camera() : m_pd3dcbCamera(nullptr)
{
	m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
}


Camera::~Camera()
{
}

void Camera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void Camera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}
void Camera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}
void Camera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	m_CameraInfo.m_xmf4x4Projection = PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void Camera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_CameraInfo.m_xmf3CameraPosition = xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;

	GenerateViewMatrix();
}

void Camera::GenerateViewMatrix()
{
	m_CameraInfo.m_xmf4x4View = Matrix4x4::LookAtLH(m_CameraInfo.m_xmf3CameraPosition, m_xmf3LookAtWorld, m_xmf3Up);
}

void Camera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_CameraInfo.m_xmf4x4View._11 = m_xmf3Right.x; m_CameraInfo.m_xmf4x4View._12 = m_xmf3Up.x; m_CameraInfo.m_xmf4x4View._13 = m_xmf3Look.x;
	m_CameraInfo.m_xmf4x4View._21 = m_xmf3Right.y; m_CameraInfo.m_xmf4x4View._22 = m_xmf3Up.y; m_CameraInfo.m_xmf4x4View._23 = m_xmf3Look.y;
	m_CameraInfo.m_xmf4x4View._31 = m_xmf3Right.z; m_CameraInfo.m_xmf4x4View._32 = m_xmf3Up.z; m_CameraInfo.m_xmf4x4View._33 = m_xmf3Look.z;
	m_CameraInfo.m_xmf4x4View._41 = -Vector3::DotProduct(m_CameraInfo.m_xmf3CameraPosition, m_xmf3Right);
	m_CameraInfo.m_xmf4x4View._42 = -Vector3::DotProduct(m_CameraInfo.m_xmf3CameraPosition, m_xmf3Up);
	m_CameraInfo.m_xmf4x4View._43 = -Vector3::DotProduct(m_CameraInfo.m_xmf3CameraPosition, m_xmf3Look);
}

void Camera::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CameraInfo) + 255) & ~255); //256�� ���
	m_pd3dcbCamera = Mesh::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbCamera->Map(0, NULL, (void **)&m_pcbMappedCamera);
}

void Camera::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_CameraInfo.m_xmf4x4View)));
	XMStoreFloat4x4(&m_pcbMappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_CameraInfo.m_xmf4x4Projection)));
	::memcpy(&m_pcbMappedCamera->m_xmf3CameraPosition, &m_CameraInfo.m_xmf3CameraPosition, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbCamera->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, d3dGpuVirtualAddress);
}
void Camera::Rotate(float x, float y, float z)
{
	if (m_pPlayer && (x != 0.0f))
	{
		XMFLOAT3 xmf3Right = m_pPlayer->GetRightVector();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right), XMConvertToRadians(x));
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_CameraInfo.m_xmf3CameraPosition = Vector3::Subtract(m_CameraInfo.m_xmf3CameraPosition, m_pPlayer->GetPosition());
		m_CameraInfo.m_xmf3CameraPosition = Vector3::TransformCoord(m_CameraInfo.m_xmf3CameraPosition, xmmtxRotate);
		m_CameraInfo.m_xmf3CameraPosition = Vector3::Add(m_CameraInfo.m_xmf3CameraPosition, m_pPlayer->GetPosition());
	}
	if (m_pPlayer && (y != 0.0f))
	{
		XMFLOAT3 xmf3Up = m_pPlayer->GetUpVector();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(y));
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_CameraInfo.m_xmf3CameraPosition = Vector3::Subtract(m_CameraInfo.m_xmf3CameraPosition, m_pPlayer->GetPosition());
		m_CameraInfo.m_xmf3CameraPosition = Vector3::TransformCoord(m_CameraInfo.m_xmf3CameraPosition, xmmtxRotate);
		m_CameraInfo.m_xmf3CameraPosition = Vector3::Add(m_CameraInfo.m_xmf3CameraPosition, m_pPlayer->GetPosition());
	}
	if (m_pPlayer && (z != 0.0f))
	{
		XMFLOAT3 xmf3Look = m_pPlayer->GetLookVector();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look), XMConvertToRadians(z));
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_CameraInfo.m_xmf3CameraPosition = Vector3::Subtract(m_CameraInfo.m_xmf3CameraPosition, m_pPlayer->GetPosition());
		m_CameraInfo.m_xmf3CameraPosition = Vector3::TransformCoord(m_CameraInfo.m_xmf3CameraPosition, xmmtxRotate);
		m_CameraInfo.m_xmf3CameraPosition = Vector3::Add(m_CameraInfo.m_xmf3CameraPosition, m_pPlayer->GetPosition());
	}
}