cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView;
	matrix		gmtxProjection;
	float3		    gvCameraPosition;
};

Texture2D gtxtBaseTexture : register(t0);

SamplerState gWrapSamplerState : register(s0);
SamplerState gClampSamplerState : register(s1);

struct VS_INPUT
{
	float3 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW;
};


VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld);
	output.position = mul(mul(float4(output.positionW.xyz, 1.0f), gmtxView), gmtxProjection);

	return(output);
}

float4 PS(VS_OUTPUT input)
{
	float4 Color = { 0.3f, 0.6f, 0.3f, 1.0f };

	return Color;
}