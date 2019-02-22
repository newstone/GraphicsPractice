#pragma once
struct TotalTime
{
	DWORD nStartTime;
	DWORD nEndTime;
	TotalTime() : nStartTime(0), nEndTime(0) {};
	TotalTime(const DWORD& s, const DWORD& e) : nStartTime(s), nEndTime(e){}
};

class AnimationTime
{
	DWORD m_dwdStartTime;
	DWORD m_dwdCurrTime;
	vector<TotalTime> m_vTimes;
public:
	AnimationTime();
	~AnimationTime();

	void AddTotalTime(const DWORD& nStartTime, const DWORD& nEndTime);
	void ReserveTimeResource(UINT nAimation, UINT nCluster);
	TotalTime& GetTotalTime(UINT nAnimationCount);

	DWORD& GetStartTime();
	DWORD& GetCurrTime();
	void SetStartTime(const DWORD& dwdTime);
	void SetCurrTime(const DWORD& dwdTime);
};

