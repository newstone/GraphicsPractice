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

void AnimationTime::SetTotalTime(UINT nAnimationCount, const DWORD& nEndTime)
{
	m_vTimes[nAnimationCount].nEndTime = nEndTime;
}
TotalTime& AnimationTime::GetTotalTime(UINT nAnimationCount)
{
	return m_vTimes[nAnimationCount];
}

DWORD& AnimationTime::GetStartTime()
{
	return m_dwdStartTime;
}
void AnimationTime::SetStartTime(const DWORD& dwdTime)
{
	m_dwdStartTime = dwdTime;
}
DWORD& AnimationTime::GetCurrTime()
{
	return m_dwdCurrTime;
}
void AnimationTime::SetCurrTime(const DWORD& dwdTime)
{
	m_dwdCurrTime = dwdTime;
}