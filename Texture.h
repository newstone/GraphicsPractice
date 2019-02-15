#pragma once
#include "DDSTextureLoader12.h"


#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
};

class Texture
{
public:
	Texture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE2D, int nSamplers = 0);
	virtual ~Texture();

private:
	UINT							m_nTextureType = RESOURCE_TEXTURE2D;
	int								m_nTextures = 0;
	ComPtr<ID3D12Resource>*					m_ppd3dTextures;
	ComPtr<ID3D12Resource>*					m_ppd3dTextureUploadBuffers;
	SRVROOTARGUMENTINFO				*m_pRootArgumentInfos = nullptr;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pd3dSamplerGpuDescriptorHandles = nullptr;

public:
	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	ID3D12Resource *CreateTexture(ID3D12Device *pd3dDevice, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE *pd3dClearValue, UINT nIndex);
	SRVROOTARGUMENTINFO& GetArgumentInfos(UINT nIndex)
	{
		return m_pRootArgumentInfos[nIndex];
	}

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, wchar_t *pszFileName, UINT nIndex);

	int GetTextureCount() 
	{
		return(m_nTextures);
	}
	ID3D12Resource *GetTexture(int nIndex)
	{
		return(m_ppd3dTextures[nIndex].Get());
	}
	UINT GetTextureType()
	{
		return(m_nTextureType);
	}

	void ReleaseUploadBuffers();
};


