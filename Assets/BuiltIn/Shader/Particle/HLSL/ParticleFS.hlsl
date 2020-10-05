Texture2D uTexture : register(t0);
SamplerState uTextureSampler : register(s0);
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex0 : TEXCOORD0;
};
static const float gamma = 2.2;
static const float invGamma = 1.0/2.2;
float3 sRGB(float3 color)
{
	return pow(color, gamma);
}
float3 linearRGB(float3 color)
{
	return pow(color, invGamma);
}
float4 main(PS_INPUT input) : SV_TARGET
{
	float4 result = input.color * uTexture.Sample(uTextureSampler, input.tex0);
	return float4(sRGB(result.rgb), result.a);
}
