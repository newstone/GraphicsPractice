#pragma once

struct AnimInfo
{
	UINT nTime;
	SRT srt;
};
struct SRT
{
	XMFLOAT3 S;
	XMFLOAT4 R;
	XMFLOAT3 T;
};

class AnimationResource
{
	vector<AnimInfo> m_vAnimation;
public:
	AnimationResource();
	~AnimationResource();
};

