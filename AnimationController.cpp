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
	m_AnimationStack.push(pRootObject); // ��Ʈ ��� ���� ����

	for (int i = 0;; i++)
	{// ���ÿ� ���̻� ��尡 ������ ������ ���� ���´�.
		if (m_AnimationStack.empty())
			break;
		// �ִϸ��̼� �����͸� ������ ��, ���̿켱 ������� �����߱�
		// �� ������� ��带 ���鼭 ����� ä�� �ִ´�.
		AnimationObject* TargetFrame = m_AnimationStack.top();

		if (TargetFrame != pRootObject)
		{
			SQT sqt;
			// �ִϸ��̼ǰ� �ִϸ��̼� ���̴� Ư���ð����� �����Ͽ�
			// �ִϸ��̼� ��ȯ�� �ڿ������� �Ѵ�.
			if (m_nCurrentAnimation != CHANG_INDEX)
				FrameInterpolate(i, fTimeElapsed, sqt);
			else
				ChangeInterpolate(i, fTimeElapsed, sqt);

			XMVECTOR S = XMLoadFloat3(&sqt.S);
			XMVECTOR P = XMLoadFloat3(&sqt.T);
			XMVECTOR Q = XMLoadFloat4(&sqt.Q);

			XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			// �ִϸ��̼ǵ����ͷ� ���� ����� Position���� Quaternion���� ������ ����� �����, ToParent��Ŀ� ����.
			XMStoreFloat4x4(&TargetFrame->GetToParentTransform(), XMMatrixAffineTransformation(S, zero, Q, P));
		}

		for (int i = 0; i < TargetFrame->GetChildCount(); i++)
		{
			m_AnimationStack.push(TargetFrame->GetChild(i));
		}
	}
}
void AnimationController::SetToRootTransforms(AnimationObject* pRootObject)
{// ���� �켱 Ž���� �ǽ��Ѵ�.
	m_AnimationStack.push(pRootObject);

	while (true)
	{
		if (m_AnimationStack.empty())
			break;

		AnimationObject* TargetFrame = m_AnimationStack.top();

		// ��Ʈ�� �ڽ� ������ �����ؼ�, ������ �Ʒ��� ToParent����� ���ؼ� ������, ToRoot����� �����.
		if (TargetFrame->GetParentsOrNull() != nullptr && !TargetFrame->GetParentsOrNull()->GetRoot())
		{
			TargetFrame->GetToParentTransform() = Matrix4x4::Multiply(TargetFrame->GetToParentTransform(), TargetFrame->GetParentsOrNull()->GetToParentTransform());;
		}
		else // ���� �θ� ��尡 ������ �ڽ��� ToParent����� �� ToRoot����� �ȴ�.
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

	SetToParentTransforms(dwdTime, pRootObject); // ó������ ���� ����� �ð�
	SetToRootTransforms(pRootObject);
	
	XMFLOAT4X4 xmf4x4Animation(Matrix4x4::Identity());

	for (UINT i = 0; i < m_AnimationResource.GetBindPoseTransformSize(); i++)
	{
		// ���� ��ǥ�迡�� �ش� ����� ��ǥ��� �̵���Ű�� ���
		XMMATRIX offset = XMLoadFloat4x4(&m_AnimationResource.GetBindPoseTransform(i));
		AnimationObject* m_pBoneObject(pRootObject->GetObjectOrNullByClursterNum(i));
		// �ִϸ��̼� ������ ���, �ش� ����� ��ǥ�迡�� ���� ��ǥ��� �̵���Ű�� ���
		XMMATRIX toRoot = XMLoadFloat4x4(&m_pBoneObject->GetToRootTransform());
		XMMATRIX finalTransform = XMMatrixMultiply(XMMatrixTranspose(offset), (toRoot));

		XMStoreFloat4x4(&xmf4x4Animation, XMMatrixTranspose(finalTransform));

		pRootObject->SetAnimationTransform(i, xmf4x4Animation);
	}
}
