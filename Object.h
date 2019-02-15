#pragma once
#include "Mesh.h"
#include "Renderer.h"
#include "AnimationController.h"

struct OBJECT_INFO
{
	XMFLOAT4X4 xmf4x4World;
};

class Object
{
protected:
	string m_strName;
	OBJECT_INFO m_ObjectInfo;
	OBJECT_INFO* m_pMappedObjectInfo;
	XMFLOAT3 m_xmf3Position;
	
	vector<Mesh*> m_vpMeshes;
	int m_nMeshIndex;

	int m_nMaterials;
	int m_nMaterialIndex;
	ComPtr<ID3D12DescriptorHeap> m_d3dDescriptorHeap;
	ComPtr<ID3D12Resource> m_d3dcbObjects;

	vector<Renderer*> m_vpRenderer;
public:
	Object();
	~Object();
	virtual void RelaseUploadBuffer();

	void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void CreateRenderer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);

	XMFLOAT3 GetPosition();
	void SetPosition(const XMFLOAT3& xmf3Position);
	void SetPosition(float fX, float fY, float fZ);

	void SetMesh(Mesh* pMesh);
	void SetRenderer(Renderer* pRenderer);
	void SetName(const string& strName);
	Mesh* GetMesh(int nIndex);
	
	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList, UINT RootParameterIndex);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class StaticObject : public Object
{
public:
	StaticObject(int nMeshes);
	~StaticObject();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AnimationObject : public Object
{
private:
	bool m_bRoot;

	vector<AnimationObject*> m_vpChild;
	AnimationObject* m_pParents;

	XMFLOAT4X4	m_xmf4x4ToRootTransform;
	XMFLOAT4X4	m_xmf4x4ToParentTransform;
public:
	AnimationObject();
	~AnimationObject();

	virtual void RelaseUploadBuffer();

	void SetRoot(bool bIsRoot);
	void AddChild(AnimationObject* pChild);
	void SetParents(AnimationObject* pParents);

	AnimationObject* GetChild(int nIndex);
	AnimationObject* GetParentsOrNull();
	XMFLOAT4X4& GetToRootTransform();
	XMFLOAT4X4& GetToParentTransform();

	int GetChildCount();

	void Render(ID3D12GraphicsCommandList* pd3dCommandList);
};


