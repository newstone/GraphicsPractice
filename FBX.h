#pragma once

#include "Player.h"
#include <map>
#include <stack>

#define WEIGHTS 4

struct BlendInfo
{
	UINT Index;
	float Weight; 
};

class FBX
{
private:

	FbxManager* m_pfbxSdkManager = nullptr;
	FbxScene* m_pfbxScene = nullptr;

public:
	FBX();
	~FBX();
	   
	HRESULT LoadFBXFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const string& strPath, AnimationObject* pOutObject);
	void SetModel(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FbxMesh* fbxMesh, AnimationObject* pOutObject);
	void CreateHierarchy(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FbxNode* pfbxNode, AnimationObject* pParentsObject);
	void SetAnimation(FbxNode* pfbxNode, AnimationObject* pRootObject, AnimationController* pAnimationController);

	void GetAnimationData(FbxNode* pfbxNode, AnimationController* pAnimationController);
	FbxAMatrix GetGeometricTransform(FbxNode *pfbxNode);
	void GetBindposeTransform(FbxNode* pfbxNode, FbxMesh* pfbxMesh, AnimationObject* pRootObject, AnimationController* pAnimationController);
	void GetVertexInfo(FbxMesh*pMesh, UINT nVertices, vector<XMFLOAT3>& xmf3Normal, vector<XMFLOAT2>& xmf3UV, vector<XMFLOAT3>& xmf3Tangent, vector<XMFLOAT3>& vxmf3BiNormal);
	void GetBlendInfo(FbxMesh* pfbxMesh, map<int, vector<BlendInfo>>& mWeight);
	void GetIndeices(FbxMesh* pMesh, vector<UINT>& Indices, int nPolygonIndex);
	void GetBiNormal(FbxMesh* pMesh, vector<XMFLOAT3>& vxmf3BiNormal, int nVertexID, int nPolygonIndex, int nPolygonSizeIndex);
	void GetNormal(FbxMesh* pMesh, vector<XMFLOAT3>& xmf3Normal, int nVertexID, int nPolygonIndex, int nPolygonSizeIndex);
	void GetTangent(FbxMesh* pMesh, vector<XMFLOAT3>& xmf3Tangent, int nVertexID, int nPolygonIndex, int nPolygonSizeIndex);
	void GetUV(FbxMesh* pMesh, vector<XMFLOAT2>& xmf3UV, int nUV, int nPolygonIndex, int nPolygonSizeIndex);
	void GetColor(FbxMesh* pMesh, vector<XMFLOAT4>& xmf4Color, int nVertexID, int nPolygonIndex, int nPolygonSizeIndex);
};

