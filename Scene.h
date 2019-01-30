#pragma once
#include "FBX.h"

class Camera;

class Scene
{
private:
	FBX* m_pFBX;
	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature;
	Object* m_pObject;

	Camera* m_pCamera;
public:
	Scene();	
	~Scene();
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	void LoadModel(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const string& strPath);

	void SetCamera(Camera* pCamera);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
};

