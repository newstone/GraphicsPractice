#pragma once
#include "Mesh.h"

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

class Material
{
	Material();
	~Material();

	static D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType);
};

class Object
{
private:
	string strName;

protected:

	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT3 m_xmf3Position;
	
	Mesh** m_ppMeshes;
	int m_nMeshes;
	int m_nMeshIndex;

	Material** m_ppMaterials;
	int m_nMaterials;
	int m_nMaterialIndex;
	ComPtr<ID3D12DescriptorHeap> m_d3dDescriptorHeap;
public:
	Object(int nMeshes);
	~Object();

	XMFLOAT3 GetPosition();
	void SetPosition(const XMFLOAT3& xmf3Position);
	void SetPosition(float fX, float fY, float fZ);

	void SetMesh(Mesh* pMesh);
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


