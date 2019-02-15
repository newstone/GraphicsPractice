#pragma once

#include <string>
#include "XMMath.h"

struct V1
{
	XMFLOAT3 xmf3Position;
};
struct V2 : public V1
{
	XMFLOAT3 xmf3Normal;
};
struct V3 : public V2
{
	XMFLOAT2 xmf2UV;
};
struct V4 : public V3
{
	XMFLOAT3 xmf3Tangent;
};
struct V5 : public V4
{
	XMUINT4 xmn4Cluster;
	XMFLOAT4 xmf4Weight;
};

class Vertex
{
private:
	void* m_pVertices;

	UINT		m_nStride;
	UINT m_nIndices;
	UINT* m_pIndices;
	UINT m_nVertices;
public:
	Vertex(UINT nVertices) : m_nVertices(nVertices)
	{}
	~Vertex() 
	{}
	virtual void SetVertices(void* pV);
	virtual void SetIndices(UINT* pIndices, UINT nIndices);
	void SetStride(UINT nStride)
	{
		m_nStride = nStride;
	}

	UINT GetStride()
	{
		return m_nStride;
	}
	virtual void* GetVertices()
	{
		return m_pVertices;
	}
	virtual UINT GetVerticesSize()
	{
		return m_nVertices;
	}
	virtual UINT GetIndicesSize()
	{
		return m_nIndices;
	}
	virtual UINT* GetIndices()
	{
		return m_pIndices;
	}
};

class Mesh
{
protected:
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

	void RelaseUploadBuffer();
	static ID3D12Resource* CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData
		, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource **ppd3dUploadBuffer);
	Vertex* GetVertex()
	{
		return m_pVertex;
	}

	void Render(ID3D12GraphicsCommandList *pd3dCommandList);
};