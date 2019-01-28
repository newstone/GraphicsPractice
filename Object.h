#pragma once
#include "Mesh.h"
#include "Renderer.h"

class Object
{
protected:
	string strName;
	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT3 m_xmf3Position;
	
	vector<Mesh*> m_vpMeshes;
	int m_nMeshes;
	int m_nMeshIndex;

	int m_nMaterials;
	int m_nMaterialIndex;
	ComPtr<ID3D12DescriptorHeap> m_d3dDescriptorHeap;

	vector<Renderer*> m_vpRenderer;
public:
	Object(int nMeshes);
	~Object();

	void CreateRenderer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	XMFLOAT3 GetPosition();
	void SetPosition(const XMFLOAT3& xmf3Position);
	void SetPosition(float fX, float fY, float fZ);

	void SetMesh(Mesh* pMesh);
	void SetRenderer(Renderer* pRenderer);
	Mesh* GetMesh(int nIndex);
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

	AnimationObject* m_pChild;
	AnimationObject* m_pParents;
	AnimationObject* m_pSibling;

public:
	AnimationObject(int nMeshes);
	~AnimationObject();
};


