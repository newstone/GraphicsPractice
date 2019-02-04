#include "stdafx.h"
#include "Mesh.h"


void Vertex::SetPosition(XMFLOAT3* pxmf3Position)
{
	m_pxmf3Position = pxmf3Position;
}
void Vertex::SetUV(XMFLOAT2* pxmf2UV)
{
	m_pxmf2UV = pxmf2UV;
}
void Vertex::SetTangent(XMFLOAT3* pxmf3Tangent)
{
	m_pxmf3Tangent = pxmf3Tangent;
}
void Vertex::SetNormal(XMFLOAT3* pxmf3Normal)
{
	m_pxmf3Normal = pxmf3Normal;
}
void Vertex::SetBinormal(XMFLOAT3* pxmf3Binormal)
{
	m_pxmf3Binormal = pxmf3Binormal;
}
void Vertex::SetColor(XMFLOAT4* pxmf4Color)
{
	m_pxmf4Color = pxmf4Color;
}
void Vertex::SetIndices(UINT* pIndices)
{
	m_pIndices = pIndices;
}

int Vertex::GetVertexSize()
{
	int nSizeSum(0);
	
	if (m_pxmf3Position != nullptr)
	{
		nSizeSum += sizeof(XMFLOAT3);
	}
	if (m_pxmf2UV != nullptr)
	{
		nSizeSum += sizeof(XMFLOAT2);
	}
	if (m_pxmf3Tangent != nullptr)
	{
		nSizeSum += sizeof(XMFLOAT3);
	}
	if (m_pxmf3Normal != nullptr)
	{
		nSizeSum += sizeof(XMFLOAT3);
	}
	if (m_pxmf3Binormal != nullptr)
	{
		nSizeSum += sizeof(XMFLOAT3);
	}
	if (m_pxmf4Color != nullptr)
	{
		nSizeSum += sizeof(XMFLOAT4);
	}

	return nSizeSum;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, Vertex* pVertex) : m_pVertex(pVertex)
{
	m_nStride = sizeof(XMFLOAT3);/*pVertex->GetVertexSize();*/
	m_nVertices = pVertex->GetVertices();

	m_d3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertex->GetPosition(), m_nStride * m_nVertices
		, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_d3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_d3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
Mesh::~Mesh()
{}
ID3D12Resource* Mesh::CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList 
	*pd3dCommandList, void *pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType,
	D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource **ppd3dUploadBuffer)
{
	ID3D12Resource *pd3dBuffer = NULL;

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = d3dHeapType;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, NULL, __uuidof(ID3D12Resource), (void **)&pd3dBuffer);

	if (pData)
	{
		switch (d3dHeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppd3dUploadBuffer)
			{
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void **)ppd3dUploadBuffer);
#ifdef _WITH_MAPPING
				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8 *pBufferDataBegin = NULL;
				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void **)&pBufferDataBegin);
				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppd3dUploadBuffer)->Unmap(0, NULL);

				pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);
#else
				D3D12_SUBRESOURCE_DATA d3dSubResourceData;
				::ZeroMemory(&d3dSubResourceData, sizeof(D3D12_SUBRESOURCE_DATA));
				d3dSubResourceData.pData = pData;
				d3dSubResourceData.SlicePitch = d3dSubResourceData.RowPitch = nBytes;
				::UpdateSubresources<1>(pd3dCommandList, pd3dBuffer, *ppd3dUploadBuffer, 0, 0, 1, &d3dSubResourceData);

#endif
				D3D12_RESOURCE_BARRIER d3dResourceBarrier;
				::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
				d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				d3dResourceBarrier.Transition.pResource = pd3dBuffer;
				d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
				d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8 *pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void **)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return(pd3dBuffer);
}

void Mesh::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	if (m_d3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

cm::cm(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float fWidth, float fHeight, float fDepth) 
{
	m_nVertices = 8;
	m_nStride = sizeof(XMFLOAT3);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	XMFLOAT3 pVertices[8];

	pVertices[0] = XMFLOAT3(-fx, +fy, -fz);
	pVertices[1] = XMFLOAT3(+fx, +fy, -fz);
	pVertices[2] = XMFLOAT3(+fx, +fy, +fz);
	pVertices[3] = XMFLOAT3(-fx, +fy, +fz);
	pVertices[4] = XMFLOAT3(-fx, -fy, -fz);
	pVertices[5] = XMFLOAT3(+fx, -fy, -fz);
	pVertices[6] = XMFLOAT3(+fx, -fy, +fz);
	pVertices[7] = XMFLOAT3(-fx, -fy, +fz);

	m_d3dVertexBuffer = Mesh::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_d3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_d3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_nIndices = 36;
	UINT pnIndices[36];

	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	m_d3dIndexBuffer = Mesh::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_d3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_d3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

tmesh::tmesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_nVertices = 3;
	m_nStride = sizeof(XMFLOAT3);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	XMFLOAT3 pVertices[3];

	pVertices[0] = XMFLOAT3(0.0f, 30.5f, 0.0f);
	pVertices[1] = XMFLOAT3(30.5f, -30.5f, 0.0f);
	pVertices[2] = XMFLOAT3(-30.5f, -30.5f, 0.0f);

	m_d3dVertexBuffer = Mesh::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices
, m_nStride * m_nVertices, D3D12_HEAP_TYPE_UPLOAD
, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_d3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_d3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
tmesh::~tmesh()
{}