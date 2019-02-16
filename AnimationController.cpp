#include "stdafx.h"
#include "AnimationController.h"
#include "Object.h"

template <typename T>
AnimationStack<T>::AnimationStack(UINT nBoneNum) : m_nCapacity(nBoneNum)
{
	m_nLastIndex = 0;
	m_ppTarr = new T*[BoneNum];
	for (int i = 0; i < BoneNum; i++)
	{
		m_ppTarr[i] = NULL;
	}
}
template <typename T>
AnimationStack<T>::~AnimationStack()
{
	delete m_ppTarr;
}

template <typename T>
void AnimationStack<T>::push(T* Tvalue)
{
	m_ppTarr[m_nLastIndex] = Tvalue;
	m_nLastIndex++;
}
template <typename T>
T* AnimationStack<T>::top()
{
	assert(!empty());

	T* TopValue = m_ppTarr[m_nLastIndex - 1];
	m_ppTarr[m_nLastIndex - 1] = NULL;
	m_nLastIndex--;

	return TopValue;
}
template <typename T>
bool AnimationStack<T>::empty()
{
	if (m_nLastIndex == 0)
		return true;
	else
		return false;
}

AnimationResource & AnimationController::GetAnimationResource()
{
	return m_AnimationResource;
}

AnimationController::AnimationController() : m_AnimationStack(1)
{
}


AnimationController::~AnimationController()
{
}

void AnimationController::AddBindPoseTransform(const XMFLOAT4X4& mBindPose)
{
	m_vBindPoses.push_back(mBindPose);
}

void AnimationController::FrameInterpolate(int iBoneNum, SRT& result)
{
	float prev;
	float next;

	result.S = XMFLOAT3(1, 1, 1);

	prev = (UINT)m_pRootObject->m_pAnimationTime->m_fCurrentFrame;
	next = (UINT)(m_pRootObject->m_pAnimationTime->m_fCurrentFrame + 1);

	if (prev == m_pRootObject->m_pAnimationTime->m_fCurrentFrame)
	{
		result.R = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].RotationQuat;
		result.T = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].Translation;
	}
	else
	{
		float m;
		float b;
		float x = m_pRootObject->m_pAnimationTime->m_fCurrentFrame - prev;

		m = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)next + iBoneNum].RotationQuat.x - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].RotationQuat.x;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].RotationQuat.x;
		result.R.x = m * x + b;

		m = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)next + iBoneNum].RotationQuat.y - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].RotationQuat.y;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].RotationQuat.y;
		result.R.y = m * x + b;

		m = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)next + iBoneNum].RotationQuat.z - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].RotationQuat.z;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].RotationQuat.z;
		result.R.z = m * x + b;

		m = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)next + iBoneNum].RotationQuat.w - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].RotationQuat.w;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].RotationQuat.w;
		result.R.w = m * x + b;

		result.R = Vector4::Normalize(result.R);

		m = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)next + iBoneNum].Translation.x - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].Translation.x;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].Translation.x;
		result.T.x = m * x + b;

		m = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)next + iBoneNum].Translation.y - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].Translation.y;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].Translation.y;
		result.T.y = m * x + b;

		m = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)next + iBoneNum].Translation.z - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].Translation.z;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iState].pFrame[m_nBone*(UINT)prev + iBoneNum].Translation.z;
		result.T.z = m * x + b;
	}
}
void AnimationController::ChangeInterpolate(int iBoneNum, float fTime, SRT& result)
{
	float fTimeRate = fTime / CHANGE_TIME; // 0 ~ 1사이값

	float m;
	float b;

	result.S = XMFLOAT3(fRendererScale, fRendererScale, fRendererScale);

	if (m_pRootObject->m_pAnimationTime->m_iNewState >= 0)
	{
		XMStoreFloat4(&result.R, XMQuaternionSlerp(XMLoadFloat4(&m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].RotationQuat)
			, XMLoadFloat4(&m_pAnimation[m_pRootObject->m_pAnimationTime->m_iNewState].pFrame[iBoneNum].RotationQuat), fTimeRate));

		m = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iNewState].pFrame[iBoneNum].Translation.x - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.x;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.x;
		result.T.x = m * fTimeRate + b;

		m = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iNewState].pFrame[iBoneNum].Translation.y - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.y;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.y;
		result.T.y = m * fTimeRate + b;

		m = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iNewState].pFrame[iBoneNum].Translation.z - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.z;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.z;
		result.T.z = m * fTimeRate + b;
	}
	else if (m_pRootObject->m_pAnimationTime->m_iNewState < 0)
	{
		XMStoreFloat4(&result.R, XMQuaternionSlerp(XMLoadFloat4(&m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].RotationQuat)
			, XMLoadFloat4(&m_pAnimation[-m_pRootObject->m_pAnimationTime->m_iNewState].pFrame[m_nBone*m_pAnimation[-m_pRootObject->m_pAnimationTime->m_iNewState].nFrame + iBoneNum].RotationQuat), fTimeRate));

		m = m_pAnimation[-m_pRootObject->m_pAnimationTime->m_iNewState].pFrame[m_nBone*m_pAnimation[-m_pRootObject->m_pAnimationTime->m_iNewState].nFrame + iBoneNum].Translation.x - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.x;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.x;
		result.T.x = m * fTimeRate + b;

		m = m_pAnimation[-m_pRootObject->m_pAnimationTime->m_iNewState].pFrame[m_nBone*m_pAnimation[-m_pRootObject->m_pAnimationTime->m_iNewState].nFrame + iBoneNum].Translation.y - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.y;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.y;
		result.T.y = m * fTimeRate + b;

		m = m_pAnimation[-m_pRootObject->m_pAnimationTime->m_iNewState].pFrame[m_nBone*m_pAnimation[-m_pRootObject->m_pAnimationTime->m_iNewState].nFrame + iBoneNum].Translation.z - m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.z;
		b = m_pAnimation[m_pRootObject->m_pAnimationTime->m_iSaveState].pFrame[m_nBone*(UINT)m_pRootObject->m_pAnimationTime->m_fSaveLastFrame + iBoneNum].Translation.z;
		result.T.z = m * fTimeRate + b;
	}
}
void AnimationController::SetToParentTransforms(UINT fTimeElapsed, AnimationObject* pRootObject)
{
	m_AnimationStack.push(pRootObject); // 루트 노드 부터 시작

	for (int i = 0;; i++)
	{// 스택에 더이상 노드가 없으면 루프를 빠져 나온다.
		if (m_AnimationStack.empty())
			break;
		// 애니메이션 데이터를 저장할 때, 깊이우선 방식으로 저장했기
		// 그 순서대로 노드를 돌면서 행렬을 채워 넣는다.
		AnimationObject* TargetFrame = m_AnimationStack.top();

		if (TargetFrame != pRootObject)
		{
			SRT srt;
			// 애니메이션과 애니메이션 사이는 특정시간동안 보간하여
			// 애니메이션 전환을 자연스럽게 한다.
			if (pRootObject->m_pAnimationTime->m_iState != CHANG_INDEX)
				FrameInterpolate(i, srt);
			else
				ChangeInterpolate(i, fTime, srt);

			XMVECTOR S = XMLoadFloat3(&srt.S);
			XMVECTOR P = XMLoadFloat3(&srt.T);
			XMVECTOR Q = XMLoadFloat4(&srt.R);

			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			// 애니메이션데이터로 부터 얻어진 Position값과 Quaternion값을 가지고 행렬을 만들어, ToParent행렬에 대입.
			XMStoreFloat4x4(&TargetFrame->GetToParentTransform(), XMMatrixAffineTransformation(S, zero, Q, P));
		}

		for (int i = 0; i < TargetFrame->GetChildCount(); i++)
		{
			m_AnimationStack.push(TargetFrame->GetChild(i));
		}
	}
}
void AnimationController::SetToRootTransforms(AnimationObject* pRootObject)
{// 깊이 우선 탐색을 실시한다.
	m_AnimationStack.push(pRootObject);

	while (true)
	{
		if (m_AnimationStack.empty())
			break;

		AnimationObject* TargetFrame = m_AnimationStack.top();

		// 루트의 자식 노드부터 시작해서, 위부터 아래로 ToParent행렬을 곱해서 갱신해, ToRoot행렬을 만든다.
		if (TargetFrame->GetParentsOrNull() != nullptr && TargetFrame != pRootObject->m_pChild)
		{
			TargetFrame->GetToParentTransform() = Matrix4x4::Multiply(TargetFrame->GetToParentTransform(), TargetFrame->GetParentsOrNull()->GetToParentTransform());;
		}
		else // 위로 부모 노드가 없으면 자신의 ToParent행렬이 곧 ToRoot행렬이 된다.
			TargetFrame->GetToParentTransform() = TargetFrame->GetToParentTransform();

		for (int i = 0; i < TargetFrame->GetChildCount(); i++)
		{
			m_AnimationStack.push(TargetFrame->GetChild(i));
		}
	}
}
void AnimationController::AdvanceAnimation(ID3D12GraphicsCommandList* pd3dCommandList, UINT fTimeElapsed, AnimationObject* pRootObject)
{
	SetToParentTransforms(fTimeElapsed, pRootObject); // 시간에 맞추어 m_xmf4x4ToParentTransform에 맞는 값을 넣어준다. 
	SetToRootTransforms(pRootObject);
	for (UINT i = 0; i < m_vBindPoses.size() ; i++)
	{
		// 월드 좌표계에서 해당 노드의 좌표계로 이동시키는 행렬
		XMMATRIX offset = XMLoadFloat4x4(&m_vBindPoses[i]);
		// 애니메이션 정보를 담고, 해당 노드의 좌표계에서 월드 좌표계로 이동시키는 행렬
		XMMATRIX toRoot = XMLoadFloat4x4(&m_ppBoneObject[i]->m_xmf4x4ToRootTransform);
		XMMATRIX finalTransform = XMMatrixMultiply(XMMatrixTranspose(offset), (toRoot));

		// 상수버퍼는 제한이 있기때문에 한번에 32개의 행렬밖에 쉐이더로 넘겨주지 못한다.
		// 하여 상수버퍼 3개를 사용해 값을 전달한다. 
		XMStoreFloat4x4(&m_pFactorObject->m_pAnimationFactors->m_BoneTransforms->m_xmf4x4BoneTransform[i], XMMatrixTranspose(finalTransform));
	}
}
