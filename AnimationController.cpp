#include "stdafx.h"
#include "AnimationController.h"
#include "Object.h"

template <typename T>
AnimationStack<T>::AnimationStack(UINT nBoneNum) : m_nCapacity(nBoneNum)
{
	m_nLastIndex = 0;
	m_ppTarr = new T*[nBoneNum];
	for (int i = 0; i < nBoneNum; i++)
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

AnimationController::AnimationController() : m_AnimationStack(INITIAL), m_nCurrentAnimation(0), m_nNewAnimation(INVALID)
{
}

AnimationController::~AnimationController()
{
}

void AnimationController::AddBindPoseTransform(const XMFLOAT4X4& mBindPose)
{
	m_AnimationResource.AddBindPoseTransform(mBindPose);
}

UINT AnimationController::GetIndex(const DWORD& nTime, UINT nCluster)
{
	for (int i = 0; i < m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster).size(); i++)
	{
		if (m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[i].nTime > nTime)
			return (i - 1);
	}
};
void AnimationController::FrameInterpolate(UINT nCluster, const DWORD& nTime, SQT& result)
{
	const UINT prev(GetIndex(nTime, nCluster));
	const UINT next(prev + 1);


	if (m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].nTime == nTime)
	{
		result.S = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.S;
		result.Q = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.Q;
		result.T = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.T;
	}
	else
	{
		float fTimeRate((nTime - prev) / static_cast<float>(next - prev));

		XMStoreFloat4(&result.Q, XMQuaternionSlerp(XMLoadFloat4(&m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.Q)
			, XMLoadFloat4(&m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[next].srt.Q), fTimeRate));

		float m(0.0f);
		float b(0.0f);

		m = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[next].srt.T.x - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.T.x;
		b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.T.x;
		result.T.x = m * fTimeRate + b;

		m = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[next].srt.T.y - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.T.y;
		b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.T.y;
		result.T.y = m * fTimeRate + b;

		m = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[next].srt.T.z - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.T.z;
		b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.T.z;
		result.T.z = m * fTimeRate + b;

		m = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[next].srt.S.x - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.S.x;
		b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.S.x;
		result.S.x = m * fTimeRate + b;

		m = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[next].srt.S.y - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.S.y;
		b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.S.y;
		result.S.y = m * fTimeRate + b;

		m = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[next].srt.S.z - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.S.z;
		b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[prev].srt.S.z;
		result.S.z = m * fTimeRate + b;
	}
}
void AnimationController::ChangeInterpolate(UINT nCluster, const DWORD& nTime, SQT& result)
{
	float fTimeRate = nTime / CHANGE_TIME; 

	float m;
	float b;

	XMStoreFloat4(&result.Q, XMQuaternionSlerp(XMLoadFloat4(&m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.Q)
		, XMLoadFloat4(&m_AnimationResource.GetAnimInfo(m_nNewAnimation, nCluster)[0].srt.Q), fTimeRate));

	m = m_AnimationResource.GetAnimInfo(m_nNewAnimation, nCluster)[0].srt.T.x - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.T.x;
	b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.T.x;
	result.T.x = m * fTimeRate + b;

	m = m_AnimationResource.GetAnimInfo(m_nNewAnimation, nCluster)[0].srt.T.y - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.T.y;
	b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.T.y;
	result.T.y = m * fTimeRate + b;

	m = m_AnimationResource.GetAnimInfo(m_nNewAnimation, nCluster)[0].srt.T.z - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.T.z;
	b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.T.z;
	result.T.z = m * fTimeRate + b;

	m = m_AnimationResource.GetAnimInfo(m_nNewAnimation, nCluster)[0].srt.S.x - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.S.x;
	b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.S.x;
	result.S.x = m * fTimeRate + b;

	m = m_AnimationResource.GetAnimInfo(m_nNewAnimation, nCluster)[0].srt.S.y - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.S.y;
	b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.S.y;
	result.S.y = m * fTimeRate + b;

	m = m_AnimationResource.GetAnimInfo(m_nNewAnimation, nCluster)[0].srt.S.z - m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.S.z;
	b = m_AnimationResource.GetAnimInfo(m_nCurrentAnimation, nCluster)[m_nSaveLastFrame].srt.S.z;
	result.S.z = m * fTimeRate + b;
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
			SQT sqt;
			// 애니메이션과 애니메이션 사이는 특정시간동안 보간하여
			// 애니메이션 전환을 자연스럽게 한다.
			if (m_nCurrentAnimation != CHANG_INDEX)
				FrameInterpolate(i, fTimeElapsed, sqt);
			else
				ChangeInterpolate(i, fTimeElapsed, sqt);

			XMVECTOR S = XMLoadFloat3(&sqt.S);
			XMVECTOR P = XMLoadFloat3(&sqt.T);
			XMVECTOR Q = XMLoadFloat4(&sqt.Q);

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
		if (TargetFrame->GetParentsOrNull() != nullptr && !TargetFrame->GetParentsOrNull()->GetRoot())
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
	DWORD dwdTime(m_AnimationResource.GetAnimationTime().GetCurrTime() + fTimeElapsed);
	m_AnimationResource.GetAnimationTime().SetCurrTime(dwdTime);
	dwdTime -= m_AnimationResource.GetAnimationTime().GetStartTime();

	if (dwdTime > m_AnimationResource.GetAnimationTime().GetTotalTime(m_nCurrentAnimation).nEndTime)
	{
		dwdTime -= m_AnimationResource.GetAnimationTime().GetTotalTime(m_nCurrentAnimation).nEndTime;
		m_AnimationResource.GetAnimationTime().SetStartTime(m_AnimationResource.GetAnimationTime().GetCurrTime() - dwdTime);
		m_AnimationResource.GetAnimationTime().SetCurrTime(dwdTime);
	}

	SetToParentTransforms(dwdTime, pRootObject); // 처음으로 부터 경과된 시간
	SetToRootTransforms(pRootObject);
	
	XMFLOAT4X4 xmf4x4Animation(Matrix4x4::Identity());

	for (UINT i = 0; i < m_AnimationResource.GetBindPoseTransformSize(); i++)
	{
		// 월드 좌표계에서 해당 노드의 좌표계로 이동시키는 행렬
		XMMATRIX offset = XMLoadFloat4x4(&m_AnimationResource.GetBindPoseTransform(i));
		AnimationObject* m_pBoneObject(pRootObject->GetObjectOrNullByClursterNum(i));
		// 애니메이션 정보를 담고, 해당 노드의 좌표계에서 월드 좌표계로 이동시키는 행렬
		XMMATRIX toRoot = XMLoadFloat4x4(&m_pBoneObject->GetToRootTransform());
		XMMATRIX finalTransform = XMMatrixMultiply(XMMatrixTranspose(offset), (toRoot));

		XMStoreFloat4x4(&xmf4x4Animation, XMMatrixTranspose(finalTransform));

		pRootObject->SetAnimationTransform(i, xmf4x4Animation);
	}
}
