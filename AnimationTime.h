#pragma once
struct TotalTime
{
	DWORD nEndTime;
	TotalTime() : nEndTime(0) {};
	TotalTime(const DWORD& e) : nEndTime(e){}
};

class AnimationTime
{
	DWORD m_dwdStartTime;
	DWORD m_dwdCurrTime;
	vector<TotalTime> m_vTimes;
public:
	AnimationTime();
	~AnimationTime();

	void SetTotalTime(UINT nAnimationCount, const DWORD& nEndTime);
	void ReserveTimeResource(UINT nAimation, UINT nCluster);
	TotalTime& GetTotalTime(UINT nAnimationCount);

	DWORD& GetStartTime();
	DWORD& GetCurrTime();
	void SetStartTime(const DWORD& dwdTime);
	void SetCurrTime(const DWORD& dwdTime);
};

