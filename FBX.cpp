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
	   
	pfbxImporter->Destroy();

	return S_OK; 
}

void FBX::GetPosition(FbxMesh* pFbxMesh, vector<XMFLOAT3>& xmf3Position)
{
	UINT nCtrlPointCount = pFbxMesh->GetControlPointsCount();

	for (UINT i = 0; i < nCtrlPointCount; ++i)
	{
		XMFLOAT3 currPosition;
		currPosition.x = static_cast<float>(pFbxMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(pFbxMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(pFbxMesh->GetControlPointAt(i).mData[2]);
		xmf3Position.push_back(currPosition);
	}
}

void FBX::GetNormal(FbxMesh* pFbxMesh, vector<XMFLOAT3>& vxmf3Normal)
{
	if (pFbxMesh != nullptr)
	{
		//print mesh node name
		FBXSDK_printf("current mesh node: %s\n", pFbxMesh->GetName());

		//get the normal element
		FbxGeometryElementNormal* lNormalElement = pFbxMesh->GetElementNormal();

		if (lNormalElement != nullptr)
		{
			//mapping mode is by control points. The mesh should be smooth and soft.
			//we can get normals by retrieving each control point
			if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				//Let's get normals of each vertex, since the mapping mode of normal element is by control point
				for (int lVertexIndex = 0; lVertexIndex < pFbxMesh->GetControlPointsCount(); lVertexIndex++)
				{
					int lNormalIndex = 0;
					//reference mode is direct, the normal index is same as vertex index.
					//get normals by the index of control vertex
					if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						lNormalIndex = lVertexIndex;

					//reference mode is index-to-direct, get normals by the index-to-direct
					if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						lNormalIndex = lNormalElement->GetIndexArray().GetAt(lVertexIndex);

					//Got normals of each vertex.
					FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);

					XMFLOAT3 xmf3Normal;
					xmf3Normal.x = lNormal.mData[0];
					xmf3Normal.y = lNormal.mData[1];
					xmf3Normal.z = lNormal.mData[2];

					vxmf3Normal.push_back(xmf3Normal);
						
					//add your custom code here, to output normals or get them into a list, such as KArrayTemplate<FbxVector4>
					//. . .
				}//end for lVertexIndex
			}//end eByControlPoint
			//mapping mode is by polygon-vertex.
			//we can get normals by retrieving polygon-vertex.
			else if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				int lIndexByPolygonVertex = 0;
				//Let's get normals of each polygon, since the mapping mode of normal element is by polygon-vertex.
				for (int lPolygonIndex = 0; lPolygonIndex < pFbxMesh->GetPolygonCount(); lPolygonIndex++)
				{
					//get polygon size, you know how many vertices in current polygon.
					int lPolygonSize = pFbxMesh->GetPolygonSize(lPolygonIndex);
					//retrieve each vertex of current polygon.
					for (int i = 0; i < lPolygonSize; i++)
					{
						int lNormalIndex = 0;
						//reference mode is direct, the normal index is same as lIndexByPolygonVertex.
						if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
							lNormalIndex = lIndexByPolygonVertex;

						//reference mode is index-to-direct, get normals by the index-to-direct
						if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
							lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

						//Got normals of each polygon-vertex.
						FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
						
						XMFLOAT3 xmf3Normal;
						xmf3Normal.x = lNormal.mData[0];
						xmf3Normal.y = lNormal.mData[1];
						xmf3Normal.z = lNormal.mData[2];

						vxmf3Normal.push_back(xmf3Normal);

						//add your custom code here, to output normals or get them into a list, such as KArrayTemplate<FbxVector4>
						//. . .

						lIndexByPolygonVertex++;
					}//end for i //lPolygonSize
				}//end for lPolygonIndex //PolygonCount

			}//end eByPolygonVertex
		}//end if lNormalElement
	}
}
void FBX::GetBiNormal(FbxMesh* pFbxMesh, vector<XMFLOAT3>& vxmf3BiNormal)
{
}
void FBX::GetTangent(FbxMesh* pFbxMesh, vector<XMFLOAT3>& xmf3Tangent)
{
}
void FBX::GetUV(FbxMesh* pFbxMesh, vector<XMFLOAT2>& xmf3UV)
{
	if (pFbxMesh != nullptr)
	{
		FbxStringList strUVSetNameList;
		pFbxMesh->GetUVSetNames(strUVSetNameList);

		FbxGeometryElementUV* IUVElement = pFbxMesh->GetElementUV();

		//iterating over all uv sets
		for (int lUVSetIndex = 0; lUVSetIndex < 1/*strUVSetNameList.GetCount()*/; lUVSetIndex++)
		{
			//get lUVSetIndex-th uv set
			const char* lUVSetName = strUVSetNameList.GetStringAt(lUVSetIndex);
			const FbxGeometryElementUV* lUVElement = pFbxMesh->GetElementUV(lUVSetName);

			if (!lUVElement)
				continue;

			// only support mapping mode eByPolygonVertex and eByControlPoint
			if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
				lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
				return;

			//index array, where holds the index referenced to the uv data
			const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
			const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

			//iterating through the data by polygon
			const int lPolyCount = pFbxMesh->GetPolygonCount();

			if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
				{
					// build the max index array that we need to pass into MakePoly
					const int lPolySize = pFbxMesh->GetPolygonSize(lPolyIndex);
					for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
					{
						FbxVector2 lUVValue;
						XMFLOAT2 xmf2UV;

						//get the index of the current vertex in control points array
						int lPolyVertIndex = pFbxMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);
						xmf2UV.x = lUVValue.mData[0];
						xmf2UV.y = lUVValue.mData[1];

						xmf3UV.push_back(xmf2UV);

						//User TODO:
						//Print out the value of UV(lUVValue) or log it to a file
					}
				}
			}
			else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				int lPolyIndexCounter = 0;
				for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
				{
					// build the max index array that we need to pass into MakePoly
					const int lPolySize = pFbxMesh->GetPolygonSize(lPolyIndex);
					for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
					{
						if (lPolyIndexCounter < lIndexCount)
						{
							FbxVector2 lUVValue;
							XMFLOAT2 xmf2UV;

							//the UV index depends on the reference mode
							int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

							lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

							//User TODO:
							//Print out the value of UV(lUVValue) or log it to a file
							xmf2UV.x = lUVValue.mData[0];
							xmf2UV.y = lUVValue.mData[1];

							xmf3UV.push_back(xmf2UV);

							lPolyIndexCounter++;
						}
					}
				}
			}
		}
	}
}

void FBX::SetModel(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, FbxNode* pNode, Object* pOutObject)
{
	if (pNode == nullptr)
		return;

	stack<FbxMesh*> fbxMeshes;
	stack<FbxNode*> fbxAllNode;

	fbxAllNode.push(pNode);

	while (1)
	{
		if (fbxAllNode.empty())
			break;

		FbxNode* pFbxNode =  fbxAllNode.top();
		fbxAllNode.pop();

		FbxMesh* pFM = pFbxNode->GetMesh();
		if (pFM != nullptr)
			fbxMeshes.push(pFM);

		for (int i = 0; i < pFbxNode->GetChildCount(); i++)
		{
			fbxAllNode.push(pFbxNode->GetChild(i));
		}
	}

	vector<XMFLOAT3> xmf3Position;
	vector<XMFLOAT2> xmf2UV;
	vector<XMFLOAT3> xmf3Tangent;
	vector<XMFLOAT3> xmf3Normal;
	vector<XMFLOAT4> xmf4Color;

	vector<UINT> Indices;

	FbxMesh* pFbxMesh = fbxMeshes.top();

	if (pFbxMesh == nullptr)
		return;

	GetPosition(pFbxMesh, xmf3Position);
	GetNormal(pFbxMesh, xmf3Normal);
	GetTangent(pFbxMesh, xmf3Tangent);
	GetUV(pFbxMesh, xmf2UV);

	if (xmf3Position.size() > 0)
	{
		Vertex* pV;
		UINT nVertices(xmf3Position.size());
		UINT nIndices(Indices.size());

		XMFLOAT3* pxmf3Position = new XMFLOAT3[nVertices];
		XMFLOAT2* pxmf2UV(nullptr);
		XMFLOAT3* pxmf3Tangent(nullptr);
		XMFLOAT3* pxmf3Normal(nullptr);
		XMFLOAT4* pxmf4Color(nullptr);
		UINT* pIndices(nullptr);

		if (xmf2UV.size() > 0)
		{
			pxmf2UV = new XMFLOAT2[nVertices];
		}
		if (xmf3Tangent.size() > 0)
		{
			pxmf3Tangent = new XMFLOAT3[nVertices];
		}
		if (xmf3Normal.size() > 0)
		{
			pxmf3Normal = new XMFLOAT3[nVertices];
		}
		if (xmf4Color.size() > 0)
		{
			pxmf4Color = new XMFLOAT4[nVertices];
		}
		if (Indices.size() > 0)
		{
			pIndices = new UINT[nVertices];
		}


		for (int i = 0; i < nVertices; i++)
		{
			pxmf3Position[i] = xmf3Position[i];

			if (xmf2UV.size() > 0)
			{
				pxmf2UV[i] = xmf2UV[i];
			}
			if (xmf3Tangent.size() > 0)
			{
				pxmf3Tangent[i] = xmf3Tangent[i];
			}
			if (xmf3Normal.size() > 0)
			{
				pxmf3Normal[i] = xmf3Normal[i];
			}
			if (xmf4Color.size() > 0)
			{
				pxmf4Color[i] = xmf4Color[i];
			}				
		}

		if (Indices.size() > 0)
		{
			for (int i = 0; i < nIndices; i++)
			{
				pIndices[i] = Indices[i];
			}
		}

		pV = new Vertex(nVertices);
		pV->SetPosition(pxmf3Position);

		if (xmf2UV.size() > 0)
		{
			pV->SetUV(pxmf2UV);
		}
		if (xmf3Tangent.size() > 0)
		{
			pV->SetTangent(pxmf3Tangent);
		}
		if (xmf3Normal.size() > 0)
		{
			pV->SetNormal(pxmf3Normal);
		}
		if (xmf4Color.size() > 0)
		{
			pV->SetColor(pxmf4Color);
		}
		if (Indices.size() > 0)
		{
			pV->SetIndices(pIndices);
		}

		Mesh* pM;
		pM = new Mesh(pd3dDevice, pd3dCommandList, pV);
		pOutObject->SetMesh(pM);
	}
	
}