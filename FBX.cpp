#include "stdafx.h"
#include "FBX.h"
#include <stack>

FBX::FBX()
{
}

FBX::~FBX()
{
}

HRESULT FBX::LoadFBXFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const string& strPath, Object* pOutObject)
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
	
	SetModel(pd3dDevice, pd3dCommandList, m_pfbxScene->GetNode(0), pOutObject);
	FbxAnimStack* fs = m_pfbxScene->GetCurrentAnimationStack();

	pfbxImporter->Destroy();

	return S_OK; 
}
void FBX::SetModel(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FbxNode* pRootNode, Object* pOutObject)
{
	if (pRootNode == nullptr)
		return;

	stack<FbxMesh*> fbxMeshes;
	stack<FbxNode*> fbxAllNode;

	fbxAllNode.push(pRootNode);

	while (1)
	{
		if (fbxAllNode.empty())
			break;

		FbxNode* pFbxNode = fbxAllNode.top();
		fbxAllNode.pop();

		FbxMesh* pFM = pFbxNode->GetMesh();
		if (pFM != nullptr)
			fbxMeshes.push(pFM);

		for (int i = 0; i < pFbxNode->GetChildCount(); i++)
		{
			fbxAllNode.push(pFbxNode->GetChild(i));
		}
	}

	while (true)
	{
		if (fbxMeshes.empty())
			break;

		vector<XMFLOAT3> xmf3Position;
		vector<XMFLOAT2> xmf2UV;
		vector<XMFLOAT3> xmf3Tangent;
		vector<XMFLOAT3> xmf3Normal;
		vector<XMFLOAT3> xmf3Binormal;
		vector<XMFLOAT4> xmf4Color;

		vector<UINT> Indices;

		FbxMesh* pFbxMesh = fbxMeshes.top();
		fbxMeshes.pop();

		if (pFbxMesh == nullptr)
			return;
		
		//FbxSkin* pFbxSkin = pFbxMesh->GetDeformer(0, FbxDeformer::eSkin));

		int nTriangleCount = pFbxMesh->GetPolygonCount();
		int nVertexCounter = 0;

		for (unsigned int i = 0; i < nTriangleCount; ++i)
		{
			for (unsigned int j = 0; j < 3; ++j)
			{
				FbxVector4* pControlPoints = pFbxMesh->GetControlPoints();
				int nControlPointIndex = pFbxMesh->GetPolygonVertex(i, j);

				xmf3Position.push_back(XMFLOAT3(pControlPoints[nControlPointIndex].mData[0], pControlPoints[nControlPointIndex].mData[1], pControlPoints[nControlPointIndex].mData[2]));

				for (int l = 0; l < 1/*pFbxMesh->GetElementUVCount()*/; ++l)
				{
					GetUV(pFbxMesh, xmf2UV, l, i, j);
				}
				GetNormal(pFbxMesh, xmf3Normal, nVertexCounter, i, j);
				GetTangent(pFbxMesh, xmf3Tangent, nVertexCounter, i, j);
				GetBiNormal(pFbxMesh, xmf3Binormal, nVertexCounter, i, j);
			}
		}

		if (xmf3Position.size() > 0)
		{
			UINT nVertices(xmf3Position.size());
			UINT nIndices(Indices.size());
			bool bFlags[5] = { false };
			UINT* pIndices(nullptr);
			
			if (xmf2UV.size() > 0)
			{
				bFlags[1] = true;
			}
			if (xmf3Tangent.size() > 0)
			{
				//bFlags[2] = true;
			}
			if (xmf3Normal.size() > 0)
			{
				bFlags[0] = true;
			}
			if (xmf3Binormal.size() > 0)
			{
				//bFlags[3] = true;
			}
			if (xmf4Color.size() > 0)
			{
				//bFlags[4] = true;
			}
			if (Indices.size() > 0)
			{
				pIndices = new UINT[nIndices];
			}

			void* pVertices(nullptr);
			UINT nSize(sizeof(XMFLOAT3));

			//0:노말, 1: UV,  2:탄젠트, 3:바이노말 4: 디퓨즈 
			if (!bFlags[0] && !bFlags[1] && !bFlags[2] && !bFlags[3] && !bFlags[4])
			{
				pVertices = new V1[nVertices];
			}
			else if (bFlags[0] && !bFlags[1] && !bFlags[2] && !bFlags[3] && !bFlags[4])
			{
				pVertices = new V2[nVertices];
				nSize += sizeof(XMFLOAT3);
			}
			else if (bFlags[0] && bFlags[1] && !bFlags[2] && !bFlags[3] && !bFlags[4])
			{
				pVertices = new V3[nVertices];
				nSize += sizeof(XMFLOAT3);
				nSize += sizeof(XMFLOAT2);
			}
			void* pDes(pVertices);

			for (int i = 0; i < nVertices; i++)
			{				
				memcpy(pDes, &xmf3Position[i], sizeof(XMFLOAT3));
				pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT3));
				
				if (xmf3Normal.size() > 0)
				{
					memcpy(pDes, &xmf3Position[i], sizeof(XMFLOAT3));
					pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT3));
				}
				if (xmf2UV.size() > 0)
				{
					memcpy(pDes, &xmf3Position[i], sizeof(XMFLOAT2));
					pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT2));
				}
				if (xmf3Tangent.size() > 0)
				{
					//memcpy(pDes, &xmf3Position[i], sizeof(XMFLOAT3));
				//	pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT3));
				}				
				if (xmf3Binormal.size() > 0)
				{
					//memcpy(pDes, &xmf3Position[i], sizeof(XMFLOAT3));
				//	pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT3));
				}
				if (xmf4Color.size() > 0)
				{
					//memcpy(pDes, &xmf3Position[i], sizeof(XMFLOAT4));
					//pDes = (void*)((UINT8*)pDes + sizeof(XMFLOAT4));
				}
			}

			if (Indices.size() > 0)
			{
				for (int i = 0; i < nIndices; i++)
				{
					pIndices[i] = Indices[i];
				}
			}

			Vertex* pV;
			pV = new Vertex(nVertices);
			pV->SetStride(nSize);
			pV->SetVertices(pVertices);
			if (Indices.size() > 0)
			{
				pV->SetIndices(pIndices);
			}

			Mesh* pM;
			pM = new Mesh(pd3dDevice, pd3dCommandList, pV);
			pOutObject->SetMesh(pM);
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

	if (pGeometryElementNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		switch (pGeometryElementNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
			f4Normal = pGeometryElementNormal->GetDirectArray().GetAt(nVertexID);

			xmf3Normal.push_back(XMFLOAT3(f4Normal.mData[0], f4Normal.mData[1], f4Normal.mData[2]));
			break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int nID = pGeometryElementNormal->GetIndexArray().GetAt(nVertexID);
			f4Normal = pGeometryElementNormal->GetDirectArray().GetAt(nID);

			xmf3Normal.push_back(XMFLOAT3(f4Normal.mData[0], f4Normal.mData[1], f4Normal.mData[2]));
		}
		break;
		default:
			break; // other reference modes not shown here!
		}
	}
	else if (pGeometryElementNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		switch (pGeometryElementNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			f4Normal = pGeometryElementNormal->GetDirectArray().GetAt(nControlPointIndex); 
			xmf3Normal.push_back(XMFLOAT3(f4Normal.mData[0], f4Normal.mData[1], f4Normal.mData[2]));
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = pGeometryElementNormal->GetIndexArray().GetAt(nControlPointIndex);
			f4Normal = pGeometryElementNormal->GetDirectArray().GetAt(index);
			xmf3Normal.push_back(XMFLOAT3(f4Normal.mData[0], f4Normal.mData[1], f4Normal.mData[2]));
		}
		break;

		default:
			throw std::exception("Invalid Reference");
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