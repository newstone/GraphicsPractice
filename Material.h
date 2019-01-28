#pragma once
#include "Texture.h"

class Material
{
private:
	Texture* m_pTexture;

public:
	Material();
	~Material();

	void SetTexture(Texture* pTexture)
	{
		m_pTexture = pTexture;
	}
	Texture* GetTexture()
	{
		return m_pTexture;
	}

	static D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType);
};

