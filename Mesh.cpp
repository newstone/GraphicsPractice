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
	m_nStride = 12;/*pVertex->GetVertexSize();*/
	m_nVertices = pVertex->GetVertices();

	m_d3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertex->GetPosition(), m_nStride * m_nVertices
		, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_d3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_d3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}
Mesh::~Mesh()
{}

ID3D12Resource* Mesh::CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData
	, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource **ppd3dUploadBuffer)
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
	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD)
		d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK)
		d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

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

				D3D12_SUBRESOURCE_DATA d3dSubResourceData;
				::ZeroMemory(&d3dSubResourceData, sizeof(D3D12_SUBRESOURCE_DATA));
				d3dSubResourceData.pData = pData;
				d3dSubResourceData.SlicePitch = d3dSubResourceData.RowPitch = nBytes;
				::UpdateSubresources<1>(pd3dCommandList, pd3dBuffer, *ppd3dUploadBuffer, 0, 0, 1, &d3dSubResourceData);

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