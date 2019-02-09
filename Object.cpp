#include "stdafx.h"
#include "Object.h"

D3D12_SHADER_RESOURCE_VIEW_DESC Material::GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY:
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = 0;
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Object::Object() :  m_nMeshIndex(0)
{
	XMStoreFloat4x4(&m_ObjectInfo.xmf4x4World, XMMatrixIdentity());
	m_ObjectInfo.xmf4x4World._11 = m_ObjectInfo.xmf4x4World._22 = m_ObjectInfo.xmf4x4World._33 = 3.0f;
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 30.0f);
}

Object::~Object()
{
	for (int i = 0; i < m_vpRenderer.size(); i++)
		delete m_vpRenderer[i];

	for (int i = 0; i < m_vpMeshes.size(); i++)
		delete m_vpMeshes[i];
}
void Object::RelaseUploadBuffer()
{
	for (int i = 0; i < m_vpMeshes.size(); i++)
	{
		m_vpMeshes[i]->RelaseUploadBuffer();
	}
}

void Object::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(OBJECT_INFO) + 255) & ~255); //256ÀÇ ¹è¼ö
	m_d3dcbObjects = Mesh::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_d3dcbObjects->Map(0, NULL, (void **)&m_pMappedObjectInfo);
}
void Object::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, UINT RootParameterIndex)
{
	UINT ncbElementBytes = ((sizeof(OBJECT_INFO) + 255) & ~255);

	m_ObjectInfo.xmf4x4World._41 = m_xmf3Position.x;
	m_ObjectInfo.xmf4x4World._42 = m_xmf3Position.y;
	m_ObjectInfo.xmf4x4World._43 = m_xmf3Position.z;

	XMStoreFloat4x4(&m_pMappedObjectInfo->xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ObjectInfo.xmf4x4World)));

	pd3dCommandList->SetGraphicsRootConstantBufferView(RootParameterIndex, m_d3dcbObjects->GetGPUVirtualAddress());
}
void Object::CreateRenderer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_vpRenderer[0]->CreateRenderer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
}
void Object::SetName(const string& strName)
{
	m_strName = strName;
}
void Object::SetMesh(Mesh* pMesh)
{
	m_vpMeshes.push_back(pMesh);
}
void Object::SetRenderer(Renderer* pRenderer)
{
	m_vpRenderer.push_back(pRenderer);
}

void Object::SetPosition(const XMFLOAT3& xmf3Position)
{
	m_xmf3Position = xmf3Position;
}

void Object::SetPosition(float fX, float fY, float fZ)
{
	m_xmf3Position.x = fX;
	m_xmf3Position.y = fY;
	m_xmf3Position.z = fZ;
}

XMFLOAT3 Object::GetPosition()
{
	return m_xmf3Position;
}

Mesh* Object::GetMesh(int nIndex)
{
	return m_vpMeshes[nIndex];
}
void Object::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UpdateShaderVariables(pd3dCommandList, 2);
	m_vpRenderer[0]->OnPrepareForRender(pd3dCommandList);
	for (int i = 0; i < m_vpMeshes.size(); i++)
		m_vpMeshes[i]->Render(pd3dCommandList);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StaticObject::StaticObject(int nMeshes) : Object()
{
}
StaticObject::~StaticObject()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AnimationObject::AnimationObject() : Object(), m_bRoot(false), m_pParents(nullptr)
{
	XMStoreFloat4x4(&m_xmf4x4ToRootTransform, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmf4x4ToParentTransform, XMMatrixIdentity());
}

AnimationObject::~AnimationObject() 
{
}

void AnimationObject::RelaseUploadBuffer()
{
	for (int i = 0; i < m_vpMeshes.size(); i++)
	{
		m_vpMeshes[i]->RelaseUploadBuffer();
	}

	for (int i = 0; i < m_vpChild.size(); i++)
	{
		m_vpChild[i]->RelaseUploadBuffer();
	}
}

void AnimationObject::SetRoot(bool bIsRoot)
{
	m_bRoot = bIsRoot;
}
void AnimationObject::AddChild(AnimationObject* pChild)
{
	if(pChild != nullptr)
		m_vpChild.push_back(pChild);
}
void AnimationObject::SetParents(AnimationObject* pParents)
{
	if (pParents != nullptr)
		m_pParents = pParents;
}
AnimationObject* AnimationObject::GetChild(int nIndex)
{
	assert(nIndex < m_vpChild.size());
	return m_vpChild[nIndex];
}
AnimationObject* AnimationObject::GetParentsAndNull()
{
	return m_pParents;
}
void AnimationObject::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_bRoot)
	{
		UpdateShaderVariables(pd3dCommandList, 2);
		m_vpRenderer[0]->OnPrepareForRender(pd3dCommandList);
	}

	for (int i = 0; i < m_vpMeshes.size(); i++)
		m_vpMeshes[i]->Render(pd3dCommandList);

	for (int i = 0; i < m_vpChild.size(); i++)
	{
		m_vpChild[i]->Render(pd3dCommandList);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
