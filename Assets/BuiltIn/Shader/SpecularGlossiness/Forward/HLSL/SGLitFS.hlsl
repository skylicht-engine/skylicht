// File Generated by Assets/BuildShader.py - source: [SGLitFS.d.hlsl : _]
Texture2D uTexDiffuse : register(t0);
SamplerState uTexDiffuseSampler : register(s0);
Texture2D uTexNormalMap : register(t1);
SamplerState uTexNormalMapSampler : register(s1);
Texture2D uTexSpecularMap : register(t2);
SamplerState uTexSpecularMapSampler : register(s2);
TextureCube uTexReflect : register(t3);
SamplerState uTexReflectSampler : register(s3);
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
	float4 viewPosition: VIEWPOSITION;
};
cbuffer cbPerFrame
{
	float4 uLightColor;
	float4 uColor;
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
	return ambientLighting * 0.9;
}
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
float4 main(PS_INPUT input) : SV_TARGET
{
	float4 diffuseMap = uTexDiffuse.Sample(uTexDiffuseSampler, input.tex0) * uColor;
	float3 specMap = uTexSpecularMap.Sample(uTexSpecularMapSampler, input.tex0).xyz;
	float3 normalMap = uTexNormalMap.Sample(uTexNormalMapSampler, input.tex0).xyz;
	float3x3 rotation = float3x3(input.worldTangent, input.worldBinormal, input.worldNormal);
	float3 localCoords = normalMap * 2.0 - float3(1.0, 1.0, 1.0);
	localCoords.y *= input.tangentw;
	float3 n = normalize(mul(localCoords, rotation));
	n = normalize(n);
	float spec = specMap.r;
	float gloss = specMap.g;
	float roughness = 1.0 - gloss;
	float3 diffuseColor = diffuseMap.rgb;
	float3 f0 = spec;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(diffuseColor, f0, oneMinusSpecularStrength);
	f0 = float3(0.1, 0.1, 0.1);
	float3 specularColor = lerp(f0, diffuseColor, metallic);
	float3 ambientLighting = shAmbient(n);
	ambientLighting = sRGB(ambientLighting);
	diffuseColor = sRGB(diffuseColor);
	specularColor = sRGB(specularColor);
	float3 lightColor = sRGB(uLightColor.rgb);
	float c = (1.0 - spec * gloss);
	float NdotL = max(dot(n, input.worldLightDir), 0.0);
	float3 directionalLight = NdotL * lightColor;
	float3 color = (directionalLight * diffuseColor) * (0.1 + roughness * 0.3) * c * uLightMul.y;
	float3 H = normalize(input.worldLightDir + input.worldViewDir);
	float NdotE = max(0.0,dot(n, H));
	float specular = pow(NdotE, 10.0 + 100.0 * gloss) * spec;
	color += specular * specularColor * uLightMul.x;
	color += ambientLighting * diffuseColor * (0.1 + c * 0.9) / PI;
	float3 reflection = -normalize(reflect(input.worldViewDir, n));
	float brightness = (0.8 + gloss * 1.8);
	color += sRGB(uTexReflect.SampleLevel(uTexReflectSampler, reflection, roughness * 7).xyz) * brightness * specularColor;
	return float4(color, diffuseMap.a);
}
