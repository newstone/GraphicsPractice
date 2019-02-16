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
	float fTimeRate = fTime / CHANGE_TIME; // 0 ~ 1���̰�

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
			SRT srt;
			// �ִϸ��̼ǰ� �ִϸ��̼� ���̴� Ư���ð����� �����Ͽ�
			// �ִϸ��̼� ��ȯ�� �ڿ������� �Ѵ�.
			if (pRootObject->m_pAnimationTime->m_iState != CHANG_INDEX)
				FrameInterpolate(i, srt);
			else
				ChangeInterpolate(i, fTime, srt);

			XMVECTOR S = XMLoadFloat3(&srt.S);
			XMVECTOR P = XMLoadFloat3(&srt.T);
			XMVECTOR Q = XMLoadFloat4(&srt.R);

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
		if (TargetFrame->GetParentsOrNull() != nullptr && TargetFrame != pRootObject->m_pChild)
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
	SetToParentTransforms(fTimeElapsed, pRootObject); // �ð��� ���߾� m_xmf4x4ToParentTransform�� �´� ���� �־��ش�. 
	SetToRootTransforms(pRootObject);
	for (UINT i = 0; i < m_vBindPoses.size() ; i++)
	{
		// ���� ��ǥ�迡�� �ش� ����� ��ǥ��� �̵���Ű�� ���
		XMMATRIX offset = XMLoadFloat4x4(&m_vBindPoses[i]);
		// �ִϸ��̼� ������ ���, �ش� ����� ��ǥ�迡�� ���� ��ǥ��� �̵���Ű�� ���
		XMMATRIX toRoot = XMLoadFloat4x4(&m_ppBoneObject[i]->m_xmf4x4ToRootTransform);
		XMMATRIX finalTransform = XMMatrixMultiply(XMMatrixTranspose(offset), (toRoot));

		// ������۴� ������ �ֱ⶧���� �ѹ��� 32���� ��Ĺۿ� ���̴��� �Ѱ����� ���Ѵ�.
		// �Ͽ� ������� 3���� ����� ���� �����Ѵ�. 
		XMStoreFloat4x4(&m_pFactorObject->m_pAnimationFactors->m_BoneTransforms->m_xmf4x4BoneTransform[i], XMMatrixTranspose(finalTransform));
	}
}
