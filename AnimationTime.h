#pragma once
struct TotalTime
{
	DWORD nStartTime;
	DWORD nEndTime;
	TotalTime(const DWORD& s, const DWORD& e) : nStartTime(s), nEndTime(e){}
};

class AnimationTime
{
	vector<TotalTime> m_vTimes;
public:
	AnimationTime();
	~AnimationTime();

	void AddTotalTime(const DWORD& nStartTime, const DWORD& nEndTime);
	void ReserveTimeResource(UINT nAimation, UINT nCluster);
	TotalTime& GetTotalTime(UINT nAnimationCount);
};

