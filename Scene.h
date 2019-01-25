#pragma once
#include "FBX.h"

class Scene
{
private:
	FBX* m_pFBX;
	ID3D12RootSignature			*m_pd3dGraphicsRootSignature = NULL;
	Object* m_pObject;

public:
	Scene();	
	~Scene();
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	void LoadModel(const string& strPath);
};

