#pragma once
#include "AnimationTime.h"

struct SQT
{
	XMFLOAT3 S;
	XMFLOAT4 Q;
	XMFLOAT3 T;
};
struct AnimInfo
{
	DWORD nTime;
	SQT srt;
};

class AnimationResource
{
private:
	vector<vector<vector<AnimInfo>>> m_v3Animation;
	AnimationTime m_AnimationTime;

	vector<XMFLOAT4X4>	m_vBindPoses;
public:
	AnimationResource();
	~AnimationResource();

	void ReserveResource(UINT nAnimation, UINT nCluster);
	vector<AnimInfo>& GetAnimInfo(UINT nAimation, UINT nCluster);
	XMFLOAT4X4& GetBindPoseTransform(UINT nIndex);
	UINT GetBindPoseTransformSize();
	AnimationTime& GetAnimationTime();

	void AddBindPoseTransform(const XMFLOAT4X4& mBindPose);
	void AddAnimationStack(FbxAMatrix& fbxAnimationMatrix, const DWORD& nTime, UINT nAnimation, UINT nCluster);
	void SetEndTime(UINT nAnimationCount, const DWORD & fbxEndTime);
};

