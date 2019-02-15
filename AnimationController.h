#pragma once
#include "AnimationResource.h"
#include "AnimationTime.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class AnimationObject;

template <typename T>
class AnimationStack
{
public:
	AnimationStack(UINT BoneNum);
	~AnimationStack();
	void push(T* Tvalue);
	T* top();
	bool empty();
private:
	T * *m_ppTarr;
	UINT m_nLastIndex;
	UINT m_nCapacity;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class AnimationController
{
	AnimationResource* m_pAnimationResource;
	vector<XMFLOAT4X4>	m_vBindPoses;

	AnimationStack<AnimationObject> m_AnimationStack;
public:
	void FrameInterpolate(int iBoneNum, SRT& result);
	void ChangeInterpolate(int iBoneNum, float fTime, SRT& result);
	void SetToParentTransforms(UINT fTimeElapsed, AnimationObject* pRootObject);
	void SetToRootTransforms(AnimationObject* pRootObject);

	void AdvanceAnimation(ID3D12GraphicsCommandList* pd3dCommandList, UINT fTimeElapsed, AnimationObject* pRootObject);

	AnimationController();
	~AnimationController();
};

