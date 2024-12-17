// File Generated by Assets/BuildShader.py - source: [SGLightmapFS.d.hlsl : _]
Texture2D uTexAlbedo : register(t0);
SamplerState uTexAlbedoSampler : register(s0);
Texture2D uTexPosition : register(t1);
SamplerState uTexPositionSampler : register(s1);
Texture2D uTexNormal : register(t2);
SamplerState uTexNormalSampler : register(s2);
Texture2D uTexData : register(t3);
SamplerState uTexDataSampler : register(s3);
Texture2D uTexLight : register(t4);
SamplerState uTexLightSampler : register(s4);
Texture2D uTexIndirect : register(t5);
SamplerState uTexIndirectSampler : register(s5);
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};
cbuffer cbPerFrame
{
	float4 uCameraPosition;
	float4 uLightDirection;
	float4 uLightColor;
	float3 uLightMultiplier;
};
static const float PI = 3.1415926;
static const float MinReflectance = 0.04;
float getPerceivedBrightness(float3 color)
{
	return sqrt(0.299 * color.r * color.r + 0.587 * color.g * color.g + 0.114 * color.b * color.b);
}
float solveMetallic(float3 diffuse, float3 specular, float oneMinusSpecularStrength)
{
	float specularBrightness = getPerceivedBrightness(specular);
	float diffuseBrightness = getPerceivedBrightness(diffuse);
	float a = MinReflectance;
	float b = diffuseBrightness * oneMinusSpecularStrength / (1.0 - MinReflectance) + specularBrightness - 2.0 * MinReflectance;
	float c = MinReflectance - specularBrightness;
	float D = b * b - 4.0 * a * c;
	return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}
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
float3 SGLM(
	const float3 baseColor,
	const float spec,
	const float gloss,
	const float4 position,
	const float3 worldViewDir,
	const float3 worldLightDir,
	const float3 worldNormal,
	const float3 lightColor,
	const float4 light,
	const float3 indirect,
	const float directMultiplier,
	const float indirectMultiplier,
	const float lightMultiplier)
{
	float glossiness = max(gloss, 0.01);
	float roughness = 1.0 - glossiness;
	float3 f0 = spec;
	float3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, specularColor, oneMinusSpecularStrength);
	f0 = float3(0.1, 0.1, 0.1);
	float3 diffuseColor = baseColor.rgb;
	specularColor = lerp(f0, baseColor.rgb, metallic);
	specularColor = sRGB(specularColor);
	diffuseColor = sRGB(diffuseColor);
	float3 directionColor = sRGB(light.rgb);
	float3 indirectColor = sRGB(indirect.rgb);
	float3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0, dot(worldNormal, H));
	float specular = pow(NdotE, 10.0 + 100.0 * glossiness) * spec;
	float3 color = (directionColor * lightMultiplier) * diffuseColor;
	float visibility = light.a;
	float3 envSpecColor = lerp(indirectColor, float3(1.0, 1.0, 1.0), visibility);
	color += specular * specularColor * envSpecColor;
	color += indirectColor * diffuseColor * indirectMultiplier / PI;
	return color;
}
float4 main(PS_INPUT input) : SV_TARGET
{
	float3 albedo = uTexAlbedo.Sample(uTexAlbedoSampler, input.tex0).rgb;
	float4 posdepth = uTexPosition.Sample(uTexPositionSampler, input.tex0);
	float3 position = posdepth.xyz;
	float3 normal = uTexNormal.Sample(uTexNormalSampler, input.tex0).xyz;
	float3 data = uTexData.Sample(uTexDataSampler, input.tex0).xyz;
	float4 light = uTexLight.Sample(uTexLightSampler, input.tex0);
	float3 indirect = uTexIndirect.Sample(uTexIndirectSampler, input.tex0).rgb;
	float3 v = uCameraPosition.xyz - position;
	float3 viewDir = normalize(v);
	float directMul = uLightMultiplier.x;
	float indirectMul = uLightMultiplier.y;
	float lightMul = uLightMultiplier.z;
	float3 color = SGLM(
		albedo,
		data.r,
		data.g,
		posdepth,
		viewDir,
		uLightDirection.xyz,
		normal,
		uLightColor.rgb * uLightColor.a,
		light,
		indirect,
		directMul,
		indirectMul,
		lightMul);
	return float4(color, 1.0);
}
