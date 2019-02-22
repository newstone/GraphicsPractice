#pragma once
#include "FBX.h"

class Camera;

class Scene
{
private:
	FBX* m_pFBX;
	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature;
	vector<AnimationObject*> m_vObjects;
	stack<Object*> m_sReleaseUploadBuffer;

	Camera* m_pCamera;
public:
	Scene();	
	~Scene();
	
	void ReleaseUploadBuffer();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	void LoadModel(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const string& strPath);

	void SetCamera(Camera* pCamera);

	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT fTimeElapsed);
};

