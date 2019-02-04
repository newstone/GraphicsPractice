#pragma once

#include <string>
#include "XMMath.h"

using namespace std;

class Vertex
{
private:
	XMFLOAT3* m_pxmf3Position;
	XMFLOAT2* m_pxmf2UV;
	XMFLOAT3* m_pxmf3Tangent;
	XMFLOAT3* m_pxmf3Normal;
	XMFLOAT3* m_pxmf3Binormal;
	XMFLOAT4* m_pxmf4Color;

	UINT* m_pIndices;
	UINT m_nVertices;
public:
	Vertex(UINT nVertices) : m_nVertices(nVertices)
	{}
	~Vertex() 
	{}
	virtual void SetPosition(XMFLOAT3* pxmf3Position);
	virtual void SetUV(XMFLOAT2* pxmf2UV);
	virtual void SetTangent(XMFLOAT3* pxmf3Tangent);
	virtual void SetNormal(XMFLOAT3* pxmf3Normal);
	virtual void SetBinormal(XMFLOAT3* pxmf3Binormal);
	virtual void SetColor(XMFLOAT4* pxmf3Color);
	virtual void SetIndices(UINT* pIndices);

	virtual UINT GetVertices()
	{
		return m_nVertices;
	}
	virtual XMFLOAT3* GetPosition()
	{
		return m_pxmf3Position;
	};
	virtual XMFLOAT2* GetUV()
	{
		return m_pxmf2UV;
	};
	virtual XMFLOAT3* GetTangent()
	{
		return m_pxmf3Position;
	};
	virtual XMFLOAT3* GetNormal()
	{
		return m_pxmf3Normal;
	};
	virtual XMFLOAT3* GetBinormal()
	{
		return m_pxmf3Binormal;
	};
	virtual XMFLOAT4* GetColor()
	{
		return m_pxmf4Color;
	};
	virtual UINT* GetIndices()
	{
		return m_pIndices;
	}

	int GetVertexSize();
};

class Mesh
{
protected:
	int m_nType;
	UINT	m_nVertices = 0;
	Vertex* m_pVertex;

	ComPtr<ID3D12Resource>					m_d3dVertexBuffer ;
	ComPtr<ID3D12Resource>					m_d3dVertexUploadBuffer;

	ComPtr<ID3D12Resource>					m_d3dIndexBuffer;
	ComPtr<ID3D12Resource>					m_d3dIndexUploadBuffer;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nStride = 0;
	UINT							m_nOffset = 0;

	UINT							m_nIndices = 0;
public:
	Mesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, Vertex* pVertex);
	Mesh() {};
	~Mesh();

	static ID3D12Resource* CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData
		, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource **ppd3dUploadBuffer);
	Vertex* GetVertex()
	{
		return m_pVertex;
	}

	void Render(ID3D12GraphicsCommandList *pd3dCommandList);
};

class cm : public Mesh
{
public:
	cm(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f);
	~cm() {};
};
class tmesh : public Mesh
{
public:
	tmesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	~tmesh();
};
