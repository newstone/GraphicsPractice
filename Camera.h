#pragma once

struct CameraInfo
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT3						    m_xmf3CameraPosition;
};

class Camera
{
	CameraInfo m_CameraInfo;
	CameraInfo* m_pcbMappedCamera;

	D3D12_VIEWPORT					m_d3dViewport;
	D3D12_RECT						m_d3dScissorRect;

	ID3D12Resource					*m_pd3dcbCamera;

	XMFLOAT3 m_xmf3Look;
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;

	XMFLOAT3	m_xmf3LookAtWorld;
public:
	Camera();
	~Camera();

	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ);

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void GenerateViewMatrix();
	void RegenerateViewMatrix();
	void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList);

	const D3D12_VIEWPORT* GetViewport() { return(&m_d3dViewport); }
	const D3D12_RECT* GetScissorRect() { return(&m_d3dScissorRect); }
};

