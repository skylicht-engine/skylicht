// File Generated by Assets/BuildShader.py - source: [SGLitFS.d.hlsl : NO_NORMAL_MAP,NO_SPECGLOSS,SHADOW,OPTIMIZE_SHADOW]
Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);
Texture2DArray uShadowMap : register(t6);
SamplerState uShadowMapSampler : register(s6);
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float4 viewPosition: VIEWPOSITION;
	float3 depth: DEPTH;
	float4 shadowCoord: SHADOWCOORD;
};
cbuffer cbPerFrame
{
	float4 uLightColor;
	float4 uColor;
	float2 uSpecGloss;
	float2 uLightMul;
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
float3 shAmbient(float3 n)
{
	float3 ambientLighting = uSHConst[0].xyz +
		uSHConst[1].xyz * n.y +
		uSHConst[2].xyz * n.z +
		uSHConst[3].xyz * n.x;
	return ambientLighting * 0.75;
}
float shadow(const float4 shadowCoord, const float farDistance)
{
	int id = 0;
	float depth = 0.0;
	const float bias = 0.0002;
	float3 shadowUV = shadowCoord.xyz / shadowCoord.w;
	if (shadowUV.z > 1.0)
		return 1.0;
	depth = shadowUV.z;
	depth -= bias;
	float2 uv = shadowUV.xy;
	return (step(depth, uShadowMap.SampleLevel(uShadowMapSampler, float3(uv, id), 0).r));
}
static const float PI = 3.1415926;
float4 main(PS_INPUT input) : SV_TARGET
{
	float4 diffuseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0) * uColor;
	float3 specMap = float3(uSpecGloss, 0.0);
	float3 n = input.worldNormal;
	float depth = length(input.depth);
	float visibility = shadow(input.shadowCoord, depth);
	float3 ambientLighting = shAmbient(n);
	ambientLighting = sRGB(ambientLighting);
	float3 diffuseColor = sRGB(diffuseMap.rgb);
	float3 lightColor = sRGB(uLightColor.rgb);
	float spec = specMap.r;
	float gloss = specMap.g;
	float NdotL = max(dot(n, input.worldLightDir), 0.0);
	float3 directionalLight = NdotL * lightColor;
	float3 color = directionalLight * diffuseColor * 0.3 * uLightMul.y;
	float3 specularColor = float3(0.5, 0.5, 0.5);
	float3 H = normalize(input.worldLightDir + input.worldViewDir);
	float NdotE = max(0.0,dot(n, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;
	color += specular * specularColor * uLightMul.x;
	color *= visibility;
	color += ambientLighting * diffuseColor / PI;
	return float4(color, diffuseMap.a);
}
