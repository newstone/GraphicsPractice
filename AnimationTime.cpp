#include "stdafx.h"
#include "AnimationTime.h"


AnimationTime::AnimationTime()
{
}

AnimationTime::~AnimationTime()
{
}

void AnimationTime::ReserveTimeResource(UINT nAimation, UINT nCluster)
{
	m_vTimes.resize(nAimation);
}

void AnimationTime::AddTotalTime(const DWORD& nStartTime, const DWORD& nEndTime)
{
	m_vTimes.push_back(TotalTime(nStartTime, nEndTime));
}
TotalTime& AnimationTime::GetTotalTime(UINT nAnimationCount)
{
	return m_vTimes[nAnimationCount];
}