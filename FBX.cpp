#include "stdafx.h"
#include "FBX.h"

FBX::FBX()
{
}

FBX::~FBX()
{
}

HRESULT FBX::LoadFBXFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const string& strPath, AnimationObject* pOutObject)
{
	m_pfbxSdkManager = FbxManager::Create();
	
	if (!m_pfbxSdkManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}

	FbxImporter* pfbxImporter = FbxImporter::Create(m_pfbxSdkManager, "FBX IMPORTER");

	FbxIOSettings* ios = FbxIOSettings::Create(m_pfbxSdkManager, IOSROOT);
	m_pfbxSdkManager->SetIOSettings(ios);
	FbxString lPath = FbxGetApplicationDirectory();
	m_pfbxSdkManager->LoadPluginsDirectory(lPath.Buffer());

	m_pfbxScene = FbxScene::Create(m_pfbxSdkManager, "My Scene");
	
	if (!m_pfbxScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}

	bool bSuccess = pfbxImporter->Initialize(strPath.c_str(), -1, m_pfbxSdkManager->GetIOSettings());
	
	if (!bSuccess) 
		return E_FAIL;

	bSuccess = pfbxImporter->Import(m_pfbxScene);
	
	if (!bSuccess) 
		return E_FAIL;
	pOutObject->SetRoot(true);
	CreateHierarchy(pd3dDevice, pd3dCommandList, m_pfbxScene->GetNode(0), pOutObject);
	FbxAnimStack* fs = m_pfbxScene->GetCurrentAnimationStack();

	pfbxImporter->Destroy();

	return S_OK; 
}
void FBX::CreateHierarchy(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FbxNode* pfbxNode, AnimationObject* pParentsObject)
{
	if (pfbxNode == nullptr)
		return;

	FbxMesh* pFM = pfbxNode->GetMesh();
	if (pFM != nullptr)
	{
		SetModel(pd3dDevice, pd3dCommandList, pFM, pParentsObject);
	}

	for (int i = 0; i < pfbxNode->GetChildCount(); i++)
	{
		AnimationObject* pChildObject = new AnimationObject();
		pChildObject->SetParents(pParentsObject);
		pChildObject->SetName(pfbxNode->GetChild(i)->GetName());
		
		pParentsObject->AddChild(pChildObject);

		CreateHierarchy(pd3dDevice, pd3dCommandList, pfbxNode->GetChild(i), pChildObject);
	}
}
void FBX::GetBlendInfo(FbxMesh* pfbxMesh, map<int, vector<BlendInfo>>& mWeight)
{
	FbxSkin* pSkin = reinterpret_cast<FbxSkin*>(pfbxMesh->GetDeformer(0, FbxDeformer::eSkin));
	
	if (pSkin == nullptr)
		return;

	UINT nClusters = pSkin->GetClusterCount();

	for (int i = 0; i < nClusters; i++)
	{
		FbxCluster* pCurrCluster = pSkin->GetCluster(i);
		std::string currJointName = pCurrCluster->GetLink()->GetName();

		UINT nIndices = pCurrCluster->GetControlPointIndicesCount();
		int* plIndices = pCurrCluster->GetControlPointIndices();
		double* pdWeight = pCurrCluster->GetControlPointWeights();

		for (int j = 0; j < nIndices; j++)
		{
			BlendInfo bi;
			bi.Weight = static_cast<float>(pdWeight[j]);
			bi.Index = i;

			if (mWeight.find(plIndices[j]) != mWeight.end())
			{
				mWeight[plIndices[j]].push_back(bi);
			}
			else
			{
				vector<BlendInfo> vfWeight;
				vfWeight.push_back(bi);

				mWeight.insert(make_pair(plIndices[j], vfWeight));
			}
		}
	}
}
void FBX::SetModel(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FbxMesh* fbxMesh, AnimationObject* pOutObject)
{
	if (fbxMesh == nullptr)
		return;

	vector<XMFLOAT3> vxmf3Position;
	vector<XMFLOAT2> vxmf2UV;
	vector<XMFLOAT3> vxmf3Tangent;
	vector<XMFLOAT3> vxmf3Normal;
	vector<XMFLOAT3> vxmf3Binormal;
	vector<XMFLOAT4> vxmf4Weight;
	vector<XMUINT4> vxmn4Cluster;
	vector<XMFLOAT4> vxmf4Color;

	UINT* pIndices;

	FbxMesh* pFbxMesh = fbxMesh;

	map<int, vector<BlendInfo>> mWeight;

	GetBlendInfo(pFbxMesh, mWeight);

	for (map<int, vector<BlendInfo>>::iterator it = mWeight.begin(); it != mWeight.end(); it++)
	{
		if (it->second.size() < WEIGHTS)
		{
			for (int i = 0; it->second.size() < WEIGHTS; i++)
			{
				BlendInfo bi;
				bi.Index = 0;
				bi.Weight = 0.0f;
				it->second.push_back(bi);
			}
		}
		else if (it->second.size() > WEIGHTS)
		{
			it->second[3].Weight = 1.0f - it->second[0].Weight - it->second[1].Weight - it->second[2].Weight;
		}
	}

	if (pFbxMesh == nullptr)
		return;
	
	int nTriangleCount(pFbxMesh->GetPolygonCount());
	int nIndices(0);
	int nVertexCounter(0);
	FbxVector4* pControlPoints = pFbxMesh->GetControlPoints();
	int nV(pFbxMesh->GetControlPointsCount());
	
	for (int i = 0; i < nTriangleCount; i++)
	{
		nIndices += pFbxMesh->GetPolygonSize(i);
	}

	pIndices = new UINT[nIndices];
	FbxVector4 *pfbxv4Vertices = new FbxVector4[nV];
	::memcpy(pfbxv4Vertices, pFbxMesh->GetControlPoints(), nV * sizeof(FbxVector4));

	for (int i = 0; i < nV; i++)
		vxmf3Position.push_back(XMFLOAT3((float)pfbxv4Vertices[i][0], (float)pfbxv4Vertices[i][1], (float)pfbxv4Vertices[i][2]));

	GetNormal(pFbxMesh, vxmf3Normal, nV, 0, 0);
	for (unsigned int i = 0, k = 0; i < nTriangleCount; ++i)
	{
		for (unsigned int j = 0; j < 3/*pFbxMesh->GetPolygonSize(i)*/; ++j) // 오직 삼각형의 메쉬로만 그려진 오브젝트만 그릴 수 있다.
		{
			int nControlPointIndex = pFbxMesh->GetPolygonVertex(i, j);
			pIndices[k++] = nControlPointIndex;
		}
	}

	GetVertexInfo(pFbxMesh, nV, vxmf3Normal, vxmf2UV, vxmf3Tangent, vxmf3Binormal);

	if (vxmf3Position.size() > 0)
	{
		UINT nVertices(vxmf3Position.size());
		bool bFlags[6] = { false };
		
		if (vxmf2UV.size() > 0)
		{
			bFlags[1] = true;
		}
		if (vxmf3Tangent.size() > 0)
		{
			//bFlags[2] = true;
		}
		if (vxmf3Normal.size() > 0)
		{
			bFlags[0] = true;
		}
		if (vxmf3Binormal.size() > 0)
		{
			//bFlags[3] = true;
		}
		if (vxmf4Color.size() > 0)
		{
			//bFlags[4] = true;
		}
		if (vxmf4Weight.size() > 0)
		{
			bFlags[5] = true;
		}


		void* pVertices(nullptr);
		UINT nSize(sizeof(XMFLOAT3));

		//0:노말, 1: UV,  2:탄젠트, 3:바이노말 4: 디퓨즈 
		if (!bFlags[0] && !bFlags[1] && !bFlags[2] && !bFlags[3] && !bFlags[4]&&!bFlags[5])
		{
			pVertices = new V1[nVertices];
		}
		else if (bFlags[0] && !bFlags[1] && !bFlags[2] && !bFlags[3] && !bFlags[4] && !bFlags[5])
		{
			pVertices = new V2[nVertices];
			nSize += sizeof(XMFLOAT3);
		}
		else if (bFlags[0] && bFlags[1] && !bFlags[2] && !bFlags[3] && !bFlags[4] && !bFlags[5])
		{
			pVertices = new V3[nVertices];
			nSize += sizeof(XMFLOAT3);
			nSize += sizeof(XMFLOAT2);
		}
		else if (bFlags[0] && bFlags[1] && !bFlags[2] && !bFlags[3] && !bFlags[4] && bFlags[5])
		{
			pVertices = new V5[nVertices];
			nSize += sizeof(XMFLOAT3);
			nSize += sizeof(XMFLOAT2);
			nSize += sizeof(XMFLOAT4);
			nSize += sizeof(XMUINT4);
		}
		void* pDes(pVertices);

		for (int i = 0; i < nVertices; i++)
		{				
			memcpy(pDes, &vxmf3Position[i], sizeof(XMFLOAT3));
			pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT3));
			
			if (bFlags[0]) 
			{
				memcpy(pDes, &vxmf3Normal[i], sizeof(XMFLOAT3));
				pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT3));
			}
			if (bFlags[1])
			{
				memcpy(pDes, &vxmf2UV[i], sizeof(XMFLOAT2));
				pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT2));
			}
			if (bFlags[2])
			{
				memcpy(pDes, &vxmf3Tangent[i], sizeof(XMFLOAT3));
				pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT3));
			}				
			if (bFlags[3])
			{
				memcpy(pDes, &vxmf3Binormal[i], sizeof(XMFLOAT3));
				pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT3));
			}
			if (bFlags[4])
			{
				memcpy(pDes, &vxmf4Color[i], sizeof(XMFLOAT4));
				pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT4));
			}
			if (bFlags[5])
			{
				memcpy(pDes, &vxmn4Cluster[i], sizeof(XMUINT4));
				pDes = (void*)((UINT8*)pDes + sizeof(XMUINT4));
				memcpy(pDes, &vxmf4Weight[i], sizeof(XMFLOAT4));
				pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT4));
			}
		}

		Vertex* pV;
		pV = new Vertex(nVertices);
		pV->SetStride(nSize);
		pV->SetVertices(pVertices);
		if (nIndices > 0)
		{
			pV->SetIndices(pIndices, nIndices);
		}

		Mesh* pM;
		pM = new Mesh(pd3dDevice, pd3dCommandList, pV);
		pOutObject->SetMesh(pM);
	}	
}

void FBX::GetVertexInfo(FbxMesh*pMesh, UINT nVertices, vector<XMFLOAT3>& xmf3Normal, vector<XMFLOAT2>& xmf3UV, vector<XMFLOAT3>& xmf3Tangent, vector<XMFLOAT3>& vxmf3BiNormal)
{
	FbxGeometryElementNormal* pGeometryElementNormal = pMesh->GetElementNormal(0);
	FbxGeometryElementUV* pGeometryElementUV = pMesh->GetElementUV(0);
	FbxGeometryElementTangent* pGeometryElementTangent = pMesh->GetElementTangent(0);
	FbxGeometryElementBinormal* pGeometryElementBinormal = pMesh->GetElementBinormal(0);

	for (int i = 0; i < nVertices; i++)
	{
		if (pGeometryElementNormal != nullptr)
		{
			FbxVector4 f4Normal = pGeometryElementNormal->GetDirectArray().GetAt(i);
			xmf3Normal.push_back(XMFLOAT3(f4Normal.mData[0], f4Normal.mData[1], f4Normal.mData[2]));
		}
		if (pGeometryElementUV != nullptr)
		{
			FbxVector2 f2UV = pGeometryElementUV->GetDirectArray().GetAt(i);
			xmf3UV.push_back(XMFLOAT2(f2UV.mData[0], f2UV.mData[1]));
		}
		if (pGeometryElementTangent != nullptr)
		{
			FbxVector4 f4Tangent = pGeometryElementTangent->GetDirectArray().GetAt(i);
			xmf3Tangent.push_back(XMFLOAT3(f4Tangent.mData[0], f4Tangent.mData[1], f4Tangent.mData[2]));
		}
		if (pGeometryElementBinormal != nullptr)
		{
			FbxVector4 f4BiNormal = pGeometryElementBinormal->GetDirectArray().GetAt(i);
			vxmf3BiNormal.push_back(XMFLOAT3(f4BiNormal.mData[0], f4BiNormal.mData[1], f4BiNormal.mData[2]));
		}
	}
}

void FBX::GetIndeices(FbxMesh* pMesh, vector<UINT>& Indices, int nPolygonIndex)
{
}
void FBX::GetBiNormal(FbxMesh* pMesh, vector<XMFLOAT3>& vxmf3BiNormal, int nVertexID, int nPolygonIndex, int nPolygonSizeIndex)
{
	FbxGeometryElementBinormal* pBinormal = pMesh->GetElementBinormal(0);

	if (pBinormal == nullptr)
		return;

	if (pBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (pBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			vxmf3BiNormal.push_back(XMFLOAT3(pBinormal->GetDirectArray().GetAt(nVertexID).mData[0],
				pBinormal->GetDirectArray().GetAt(nVertexID).mData[1], 
				pBinormal->GetDirectArray().GetAt(nVertexID).mData[2]));
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pBinormal->GetIndexArray().GetAt(nVertexID);
			vxmf3BiNormal.push_back(XMFLOAT3(pBinormal->GetDirectArray().GetAt(id).mData[0],
				pBinormal->GetDirectArray().GetAt(id).mData[1],
				pBinormal->GetDirectArray().GetAt(id).mData[2]));
		}
		break;
		default:
			break; // other reference modes not shown here!
		}
	}
}
void FBX::GetNormal(FbxMesh* pMesh, vector<XMFLOAT3>& xmf3Normal, int nVertexID, int nPolygonIndex, int nPolygonSizeIndex)
{
	FbxGeometryElementNormal* pGeometryElementNormal = pMesh->GetElementNormal(0);

	if (pGeometryElementNormal == nullptr)
		return;

	int nControlPointIndex = pMesh->GetPolygonVertex(nPolygonIndex, nPolygonSizeIndex);
	FbxVector4 f4Normal;

	for (int i = 0; i < nVertexID + 100; i++)
	{

		f4Normal = pGeometryElementNormal->GetDirectArray().GetAt(i);
		xmf3Normal.push_back(XMFLOAT3(f4Normal.mData[0], f4Normal.mData[1], f4Normal.mData[2]));

		if (nVertexID+90 < i)
		{

			f4Normal = pGeometryElementNormal->GetDirectArray().GetAt(i);
			xmf3Normal.push_back(XMFLOAT3(f4Normal.mData[0], f4Normal.mData[1], f4Normal.mData[2]));
		}
	}
	
}
void FBX::GetTangent(FbxMesh* pMesh, vector<XMFLOAT3>& xmf3Tangent, int nVertexID, int nPolygonIndex, int nPolygonSizeIndex)
{
	FbxGeometryElementTangent* pGeometryElementTangent = pMesh->GetElementTangent(0);

	if (pGeometryElementTangent == nullptr)
		return;

	if (pGeometryElementTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (pGeometryElementTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			xmf3Tangent.push_back(XMFLOAT3(pGeometryElementTangent->GetDirectArray().GetAt(nVertexID).mData[0]
				, pGeometryElementTangent->GetDirectArray().GetAt(nVertexID).mData[1]
				, pGeometryElementTangent->GetDirectArray().GetAt(nVertexID).mData[2]));
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int nID = pGeometryElementTangent->GetIndexArray().GetAt(nVertexID);
			xmf3Tangent.push_back(XMFLOAT3(pGeometryElementTangent->GetDirectArray().GetAt(nID).mData[0]
				, pGeometryElementTangent->GetDirectArray().GetAt(nID).mData[1]
				, pGeometryElementTangent->GetDirectArray().GetAt(nID).mData[2]));
		}
		break;
		default:
			break; // other reference modes not shown here!
		}
	}
}
void FBX::GetUV(FbxMesh* pMesh, vector<XMFLOAT2>& xmf3UV, int nUV, int nPolygonIndex, int nPolygonSizeIndex)
{
	FbxGeometryElementUV* pGeometryElementUV = pMesh->GetElementUV(nUV);

	if (pGeometryElementUV == nullptr)
		return;

	int nControlPointIndex = pMesh->GetPolygonVertex(nPolygonIndex, nPolygonSizeIndex);
	   
	switch (pGeometryElementUV->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
		switch (pGeometryElementUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			pGeometryElementUV->GetDirectArray().GetAt(nControlPointIndex);
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int id = pGeometryElementUV->GetIndexArray().GetAt(nControlPointIndex);
			pGeometryElementUV->GetDirectArray().GetAt(id);
		}
		break;
		default:
			break; // other reference modes not shown here!
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
	{
		int lTextureUVIndex = pMesh->GetTextureUVIndex(nPolygonIndex, nPolygonSizeIndex);

		switch (pGeometryElementUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			FbxVector2 f2UV = pGeometryElementUV->GetDirectArray().GetAt(lTextureUVIndex);
			xmf3UV.push_back(XMFLOAT2(f2UV.mData[0], f2UV.mData[1]));
		}
		break;
		default:
			break; // other reference modes not shown here!
		}
	}
	break;

	case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
	case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
	case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
		break;
	}
}
void FBX::GetColor(FbxMesh* pMesh, vector<XMFLOAT4>& xmf4Color, int nVertexID, int nPolygonIndex, int nPolygonSizeIndex)
{
	FbxGeometryElementVertexColor* pGeometryElementVertexColor = pMesh->GetElementVertexColor(0);

	if (pGeometryElementVertexColor == nullptr)
		return;

	int nControlPointIndex = pMesh->GetPolygonVertex(nPolygonIndex, nPolygonSizeIndex);

	switch (pGeometryElementVertexColor->GetMappingMode())
	{
	default:
		break;
	case FbxGeometryElement::eByControlPoint:
		switch (pGeometryElementVertexColor->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			xmf4Color.push_back(XMFLOAT4(pGeometryElementVertexColor->GetDirectArray().GetAt(nControlPointIndex).mRed
				, pGeometryElementVertexColor->GetDirectArray().GetAt(nControlPointIndex).mGreen,
				pGeometryElementVertexColor->GetDirectArray().GetAt(nControlPointIndex).mBlue, 
				pGeometryElementVertexColor->GetDirectArray().GetAt(nControlPointIndex).mAlpha));
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int nID = pGeometryElementVertexColor->GetIndexArray().GetAt(nControlPointIndex);

			xmf4Color.push_back(XMFLOAT4(pGeometryElementVertexColor->GetDirectArray().GetAt(nID).mRed,
				pGeometryElementVertexColor->GetDirectArray().GetAt(nID).mGreen,
				pGeometryElementVertexColor->GetDirectArray().GetAt(nID).mBlue,
				pGeometryElementVertexColor->GetDirectArray().GetAt(nID).mAlpha));
		}
		break;
		default:
			break; // other reference modes not shown here!
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (pGeometryElementVertexColor->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			xmf4Color.push_back(XMFLOAT4(pGeometryElementVertexColor->GetDirectArray().GetAt(nVertexID).mRed,
				pGeometryElementVertexColor->GetDirectArray().GetAt(nVertexID).mGreen,
				pGeometryElementVertexColor->GetDirectArray().GetAt(nVertexID).mBlue,
				pGeometryElementVertexColor->GetDirectArray().GetAt(nVertexID).mAlpha));
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int nID = pGeometryElementVertexColor->GetIndexArray().GetAt(nVertexID);
			
			xmf4Color.push_back(XMFLOAT4(pGeometryElementVertexColor->GetDirectArray().GetAt(nID).mRed,
				pGeometryElementVertexColor->GetDirectArray().GetAt(nID).mGreen,
				pGeometryElementVertexColor->GetDirectArray().GetAt(nID).mBlue,
				pGeometryElementVertexColor->GetDirectArray().GetAt(nID).mAlpha));
		}
		break;
		default:
			break; // other reference modes not shown here!
		}
	}
	break;

	case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
	case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
	case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
		break;
	}
}