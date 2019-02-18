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
	vector<vector<vector<AnimInfo>>> m_v3Animation;
	AnimationTime m_AnimationTime;
public:
	AnimationResource();
	~AnimationResource();

	void ReserveResource(UINT nAnimation, UINT nCluster);
	vector<AnimInfo>& GetAnimInfo(UINT nAimation, UINT nCluster);
	void AddAnimationStack(FbxAMatrix& fbxAnimationMatrix, const DWORD& nTime, UINT nAnimation, UINT nCluster);
	void SetStartAndEndTime(const DWORD & fbxStartTime, const DWORD & fbxEndTime);
};

