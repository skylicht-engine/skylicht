// File Generated by Assets/BuildShader.py - source: [ToonFS.d.hlsl : _]
Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);
Texture2D uTexRamp : register(t1);
SamplerState uTexRampSampler : register(s1);
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldPos: WORLDPOSITION;
	float3 depth: DEPTH;
};
cbuffer cbPerFrame
{
	float4 uLightDirection;
	float4 uLightColor;
	float4 uColor;
	float4 uShadowColor;
	float2 uWrapFactor;
	float3 uSpecular;
	float4 uSHConst[4];
};
static const float gamma = 2.2;
static const float invGamma = 1.0 / 2.2;
float3 sRGB(float3 color)
{
	return pow(color, gamma);
}
float3 linearRGB(float3 color)
{
	return pow(color, invGamma);
}
static const float PI = 3.1415926;
float4 main(PS_INPUT input) : SV_TARGET
{
	float4 diffuse = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0);
	float3 diffuseMap = sRGB(diffuse.rgb);
	float3 color = sRGB(uColor.rgb);
	float shadowIntensity = uColor.a;
	float3 shadowColor = sRGB(uShadowColor.rgb);
	float3 lightColor = sRGB(uLightColor.rgb);
	float visibility = 1.0;
	float NdotL = max((dot(input.worldNormal, uLightDirection.xyz) + uWrapFactor.x) / (1.0 + uWrapFactor.x), 0.0);
	float3 rampMap = uTexRamp.Sample(uTexRampSampler, float2(NdotL, NdotL)).rgb;
	float3 ramp = lerp(color, shadowColor, shadowIntensity * (1.0 - visibility));
	ramp = lerp(ramp, color, rampMap);
	float3 h = normalize(uLightDirection.xyz + input.worldViewDir);
	float NdotH = max(0, dot(input.worldNormal, h));
	float spec = pow(NdotH, uSpecular.x*128.0) * uSpecular.y;
	spec = smoothstep(0.5-uSpecular.z*0.5, 0.5+uSpecular.z*0.5, spec);
	return float4(diffuseMap * lightColor * ramp * (0.5 + visibility * 0.5) + lightColor * spec * visibility, diffuse.a);
}
