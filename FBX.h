#pragma once

#include <fbxsdk.h>
#include "Renderer.h"

class FBX
{
private:

	FbxManager* m_pfbxSdkManager = nullptr;
	FbxScene* m_pfbxScene = nullptr;

public:
	FBX();
	~FBX();
	   
	HRESULT LoadFBXFile(const string& strPath, Object* pOutObject);
	void SetModel(FbxNode* pRootNode, Object* pOutObject);

	void GetPosition(FbxMesh* pFbxMesh, vector<XMFLOAT3>& xmf3Position);
	void GetBiNormal(FbxMesh* pFbxMesh, vector<XMFLOAT3>& vxmf3BiNormal);
	void GetNormal(FbxMesh* pFbxMesh, vector<XMFLOAT3>& xmf3Normal);
	void GetTangent(FbxMesh* pFbxMesh, vector<XMFLOAT3>& xmf3Tangent);
	void GetUV(FbxMesh* pFbxMesh, vector<XMFLOAT2>& xmf3UV);
};

