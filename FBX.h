#pragma once

#include <fbxsdk.h>
#include "Player.h"

class FBX
{
private:

	FbxManager* m_pfbxSdkManager = nullptr;
	FbxScene* m_pfbxScene = nullptr;

public:
	FBX();
	~FBX();
	   
	HRESULT LoadFBXFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const string& strPath, Object* pOutObject);
	void SetModel(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FbxNode* pRootNode, Object* pOutObject);

	void GetPosition(FbxMesh* pFbxMesh, vector<XMFLOAT3>& xmf3Position);
	void GetBiNormal(FbxMesh* pFbxMesh, vector<XMFLOAT3>& vxmf3BiNormal);
	void GetNormal(FbxMesh* pFbxMesh, vector<XMFLOAT3>& xmf3Normal);
	void GetTangent(FbxMesh* pFbxMesh, vector<XMFLOAT3>& xmf3Tangent);
	void GetUV(FbxMesh* pFbxMesh, vector<XMFLOAT2>& xmf3UV);

	void GetIndeices(FbxMesh* pMesh, vector<UINT>& Indices, int nPolygonIndex);
	void GetBiNormal(FbxMesh* pMesh, vector<XMFLOAT3>& vxmf3BiNormal, int nVertexID, int nPolygonIndex, int nPolygonSizeIndex);
	void GetNormal(FbxMesh* pMesh, vector<XMFLOAT3>& xmf3Normal, int nVertexID, int nPolygonIndex, int nPolygonSizeIndex);
	void GetTangent(FbxMesh* pMesh, vector<XMFLOAT3>& xmf3Tangent, int nVertexID, int nPolygonIndex, int nPolygonSizeIndex);
	void GetUV(FbxMesh* pMesh, vector<XMFLOAT2>& xmf3UV, int nUV, int nPolygonIndex, int nPolygonSizeIndex);
};

