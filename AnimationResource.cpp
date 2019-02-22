#include "stdafx.h"
#include "AnimationResource.h"

AnimationResource::AnimationResource()
{
}


AnimationResource::~AnimationResource()
{
}
XMFLOAT4X4& AnimationResource::GetBindPoseTransform(UINT nIndex)
{
	return m_vBindPoses[nIndex];
}
UINT AnimationResource::GetBindPoseTransformSize()
{
	return m_vBindPoses.size();
}
AnimationTime& AnimationResource::GetAnimationTime()
{
	return m_AnimationTime;
}

vector<AnimInfo>& AnimationResource::GetAnimInfo(UINT nAimation, UINT nCluster)
{
	return m_v3Animation[nAimation][nCluster];
}
void AnimationResource::AddBindPoseTransform(const XMFLOAT4X4& mBindPose)
{
	m_vBindPoses.push_back(mBindPose);
}
void AnimationResource::AddAnimationStack(FbxAMatrix& fbxAnimationMatrix, const DWORD& nTime, UINT nAnimation, UINT nCluster)
{
	SQT sqt;
	AnimInfo ai;

	sqt.Q = XMFLOAT4(static_cast<float>(fbxAnimationMatrix.GetQ().mData[0])
		, static_cast<float>(fbxAnimationMatrix.GetQ().mData[1])
		, static_cast<float>(fbxAnimationMatrix.GetQ().mData[2])
		, static_cast<float>(fbxAnimationMatrix.GetQ().mData[3]));
	sqt.T = XMFLOAT3(static_cast<float>(fbxAnimationMatrix.GetT().mData[0])
		, static_cast<float>(fbxAnimationMatrix.GetT().mData[1])
		, static_cast<float>(fbxAnimationMatrix.GetT().mData[2]));
	sqt.S = XMFLOAT3(static_cast<float>(fbxAnimationMatrix.GetS().mData[0])
		, static_cast<float>(fbxAnimationMatrix.GetS().mData[1])
		, static_cast<float>(fbxAnimationMatrix.GetS().mData[2]));

	ai.srt = sqt;
	ai.nTime = nTime;
	m_v3Animation[nAnimation][nCluster].push_back(ai);
}
void AnimationResource::ReserveResource(UINT nAnimation, UINT nCluster)
{
	vector<vector<AnimInfo>> v2AnimInfo;
	v2AnimInfo.resize(nCluster);

	for (int i = 0; i < nAnimation; i++)
	{
		m_v3Animation.push_back(v2AnimInfo);
	}

	m_AnimationTime.ReserveTimeResource(nAnimation, nCluster);
}
void AnimationResource::SetStartAndEndTime(const DWORD & fbxStartTime, const DWORD & fbxEndTime)
{
	m_AnimationTime.AddTotalTime(fbxStartTime, fbxEndTime);
}