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
	m_ObjectInfo.xmf4x4World._11 = m_ObjectInfo.xmf4x4World._22 = m_ObjectInfo.xmf4x4World._33 = 30.0f;
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 30.0f);
}

Object::~Object()
{
	if(m_pRenderer != nullptr)
		delete m_pRenderer;

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

	m_d3dcbObjects = Mesh::CreateBufferResource(pd3dDevice, pd3dCommandList, nullptr, ncbElementBytes
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);

	m_d3dcbObjects->Map(0, nullptr, (void **)&m_pMappedObjectInfo);
}
void Object::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, UINT RootParameterIndex)
{
	UINT ncbElementBytes = ((sizeof(OBJECT_INFO) + 255) & ~255);

	m_ObjectInfo.xmf4x4World._41 = m_xmf3Position.x;
	m_ObjectInfo.xmf4x4World._42 = m_xmf3Position.y;
	m_ObjectInfo.xmf4x4World._43 = m_xmf3Position.z;

	XMStoreFloat4x4(&m_pMappedObjectInfo->xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ObjectInfo.xmf4x4World)));

	m_pRenderer->SetGraphicsRootDescriptorTable(pd3dCommandList, RootParameterIndex);
}
void Object::CreateRenderer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_pRenderer->CreateRenderer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pRenderer->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_d3dcbObjects.Get(), sizeof(OBJECT_INFO));
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
	if (m_pRenderer == nullptr)
		m_pRenderer = pRenderer;
	else
	{
		delete m_pRenderer;
		m_pRenderer = pRenderer;
	}
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
	m_pRenderer->OnPrepareForRender(pd3dCommandList);

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

AnimationObject::AnimationObject() : Object(), m_bRoot(false), m_pParents(nullptr), m_AnimationStack(nullptr)
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
	m_AnimationStack = new AnimationStack<AnimationObject>(INITIAL);
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
void AnimationObject::AdjustClusterIndex(UINT nControllerIndex)
{
	if (!m_bRoot)
		return;
	else
	{
		m_AnimationStack->push(this);

		while (true)
		{
			if (m_AnimationStack->empty())
				break;

			AnimationObject* pCurr(m_AnimationStack->top());

			if (pCurr->GetClusterIndicesSize() <= nControllerIndex)
			{
				pCurr->AddClusterIndex(INVALID);
			}

			for (int i = 0; i < pCurr->GetChildCount(); i++)
			{
				m_AnimationStack->push(pCurr->GetChild(i));
			}
		}
	}
}
void AnimationObject::AddClusterIndex(UINT nIndex)
{
	m_vClusterIndices.push_back(nIndex);
}
UINT AnimationObject::GetClusterIndicesSize()
{
	return m_vClusterIndices.size();
}
UINT AnimationObject::GetClusterIndex(UINT nControllerIndex)
{
	return m_vClusterIndices[nControllerIndex];
}
bool AnimationObject::GetRoot()
{
	return m_bRoot;
}
AnimationObject* AnimationObject::GetChild(int nIndex)
{
	assert(nIndex < m_vpChild.size());
	return m_vpChild[nIndex];
}
int AnimationObject::GetChildCount()
{
	return m_vpChild.size();
}
AnimationObject* AnimationObject::GetParentsOrNull()
{
	return m_pParents;
}
XMFLOAT4X4& AnimationObject::GetToRootTransform()
{
	return m_xmf4x4ToRootTransform;
}
XMFLOAT4X4& AnimationObject::GetToParentTransform()
{
	return m_xmf4x4ToParentTransform;
}
void AnimationObject::SetAnimationTransform(UINT nIndex, const XMFLOAT4X4& xmf4x4Animation)
{
	XMStoreFloat4x4(&m_pMappedObjectInfo->xmf4x4Animation[nIndex], XMMatrixTranspose(XMLoadFloat4x4(&xmf4x4Animation)));
}
AnimationObject* AnimationObject::GetObjectOrNullByClursterNum(UINT nCluster, UINT nControllerIndex)
{
	if (!m_bRoot)
		return nullptr;
	else
	{
		stack<AnimationObject*> s;
		s.push(this);

		while (true)
		{
			if (s.empty())
				break;

			AnimationObject* pCurr(s.top());
			s.pop();
			
			if (pCurr->GetClusterIndex(nControllerIndex) == nCluster)
				return pCurr;

			for (int i = 0; i < pCurr->GetChildCount(); i++)
			{
				s.push(pCurr->GetChild(i));
			}
		}
	}

	return nullptr;
}
void AnimationObject::UpdateTransform(XMFLOAT4X4 *pxmf4x4Parent)
{
	m_ObjectInfo.xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParentTransform, *pxmf4x4Parent) : m_xmf4x4ToParentTransform;

	for (int i = 0; i < m_vpChild.size(); i++)
	{
		m_vpChild[i]->UpdateTransform(&m_ObjectInfo.xmf4x4World);
	}
}
bool AnimationObject::FindObjectByNameAndSetClusterNum(const char * pName, AnimationObject* pParentObject, UINT nClusterIndex)
{
	if (pParentObject->m_strName == pName)
	{
		pParentObject->m_vClusterIndices.push_back(nClusterIndex);
		return true;
	}

	for (int i = 0; i < pParentObject->GetChildCount(); i++)
	{
		if (FindObjectByNameAndSetClusterNum(pName, pParentObject->GetChild(i), nClusterIndex))
			return true;
	}

	return false;
}
AnimationObject* AnimationObject::FindObjectByName(const char * pName, AnimationObject* pParentObject)
{
	if (pParentObject->m_strName == pName)
	{
		return pParentObject;
	}
	else
	{
		for (int i = 0; i < pParentObject->GetChildCount(); i++)
		{
			AnimationObject* pObject (FindObjectByName(pName, pParentObject->GetChild(i)));

			if (pObject != nullptr)
				return pObject;
		}

		return nullptr;
	}
}
AnimationController* AnimationObject::GetAnimationControllerOrNull()
{
	return m_pAnimationController;
}
void AnimationObject::CreateAnimationController(UINT nIndex)
{
	m_pAnimationController = new AnimationController(nIndex);
}
void AnimationObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT fTimeElapsed, AnimationObject* pRoot)
{
	if (m_bRoot)
	{
		UpdateShaderVariables(pd3dCommandList, 4);
		m_pRenderer->OnPrepareForRender(pd3dCommandList);
	}

	for (int i = 0; i < m_vpMeshes.size(); i++)
	{
		if (m_pAnimationController != nullptr)
			m_pAnimationController->AdvanceAnimation(pd3dCommandList, fTimeElapsed, pRoot);
		m_vpMeshes[i]->Render(pd3dCommandList);
	}

	for (int i = 0; i < m_vpChild.size(); i++)
	{
		m_vpChild[i]->Render(pd3dCommandList, fTimeElapsed, pRoot);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
