#pragma once
#include "AnimationResource.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define INITIAL 10
#define CHANG_INDEX -1
#define CHANGE_TIME 500

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
	UINT m_nControllerIndex;

	AnimationResource m_AnimationResource;
	UINT m_nCurrentAnimation;
	UINT m_nNewAnimation;

	UINT m_nSaveLastFrame;
	AnimationStack<AnimationObject> m_AnimationStack;
public:
	void FrameInterpolate(UINT nCluster, const DWORD& nTime, SQT& result);
	void ChangeInterpolate(UINT nCluster, const DWORD& nTime, SQT& result);
	void SetToParentTransforms(UINT fTimeElapsed, AnimationObject* pRootObject);
	void SetToRootTransforms(AnimationObject* pRootObject);

	void AddBindPoseTransform(const XMFLOAT4X4& mBindPose);
	void AdvanceAnimation(ID3D12GraphicsCommandList* pd3dCommandList, UINT fTimeElapsed, AnimationObject* pRootObject);

	AnimationResource& GetAnimationResource();
	UINT GetIndex(const DWORD& nTime, UINT nCluster);
	AnimationController(UINT nIndex);
	~AnimationController();
};

