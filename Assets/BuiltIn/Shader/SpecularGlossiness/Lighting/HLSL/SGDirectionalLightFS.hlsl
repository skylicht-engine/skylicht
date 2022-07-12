// File Generated by Assets/BuildShader.py - source: [SGDirectionalLightFS.d.hlsl : ENABLE_SSR]
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
Texture2DArray uShadowMap : register(t6);
SamplerState uShadowMapSampler : register(s6);
Texture2D uTexLastFrame : register(t7);
SamplerState uTexLastFrameSampler : register(s7);
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
	float3 uShadowDistance;
	float4x4 uShadowMatrix[3];
	float4x4 uView;
	float4x4 uProjection;
};
float shadow(const float4 shadowCoord[3], const float shadowDistance[3], const float farDistance)
{
	int id = 0;
	float visible = 1.0;
	float bias = 0.0001;
	float depth = 0.0;
	float result = 0.0;
	if (farDistance < shadowDistance[0])
		id = 0;
	else if (farDistance < shadowDistance[1])
		id = 1;
	else if (farDistance < shadowDistance[2])
		id = 2;
	else
		return 1.0;
	float3 shadowUV = shadowCoord[id].xyz / shadowCoord[id].w;
	depth = shadowUV.z;
	depth -= bias;
	float2 uv = shadowUV.xy;
	float size = 1.0/2048;
	float2 off;
	float2 rand;
	const float2 kRandom1 = float2(12.9898,78.233);
	const float kRandom2 = 43758.5453;
	const float kRandom3 = 0.00047;
	{off = float2(-1, -1) * size;rand = uv + off;rand += (float2(frac(sin(dot(rand.xy, kRandom1)) * kRandom2), frac(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, uShadowMap.SampleLevel(uShadowMapSampler, float3(rand, id), 0).r));}
	{off = float2(0, -1) * size;rand = uv + off;rand += (float2(frac(sin(dot(rand.xy, kRandom1)) * kRandom2), frac(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, uShadowMap.SampleLevel(uShadowMapSampler, float3(rand, id), 0).r));}
	{off = float2(1, -1) * size;rand = uv + off;rand += (float2(frac(sin(dot(rand.xy, kRandom1)) * kRandom2), frac(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, uShadowMap.SampleLevel(uShadowMapSampler, float3(rand, id), 0).r));}
	{off = float2(-1, 0) * size;rand = uv + off;rand += (float2(frac(sin(dot(rand.xy, kRandom1)) * kRandom2), frac(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, uShadowMap.SampleLevel(uShadowMapSampler, float3(rand, id), 0).r));}
	{off = float2(0, 0) * size;rand = uv + off;rand += (float2(frac(sin(dot(rand.xy, kRandom1)) * kRandom2), frac(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, uShadowMap.SampleLevel(uShadowMapSampler, float3(rand, id), 0).r));}
	{off = float2(1, 0) * size;rand = uv + off;rand += (float2(frac(sin(dot(rand.xy, kRandom1)) * kRandom2), frac(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, uShadowMap.SampleLevel(uShadowMapSampler, float3(rand, id), 0).r));}
	{off = float2(-1, 1) * size;rand = uv + off;rand += (float2(frac(sin(dot(rand.xy, kRandom1)) * kRandom2), frac(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, uShadowMap.SampleLevel(uShadowMapSampler, float3(rand, id), 0).r));}
	{off = float2(0, 1) * size;rand = uv + off;rand += (float2(frac(sin(dot(rand.xy, kRandom1)) * kRandom2), frac(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, uShadowMap.SampleLevel(uShadowMapSampler, float3(rand, id), 0).r));}
	{off = float2(1, 1) * size;rand = uv + off;rand += (float2(frac(sin(dot(rand.xy, kRandom1)) * kRandom2), frac(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, uShadowMap.SampleLevel(uShadowMapSampler, float3(rand, id), 0).r));}
	return result / 9.0;
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
float2 binarySearch(float3 dir, float3 rayPosition)
{
	float4 projectedCoord;
	float4 testPosition;
	float dDepth;
	const float2 uvOffset = float2(0.5, 0.5);
	const float2 uvScale = float2(0.5, -0.5);
	[unroll]
	for(int i = 4; i > 0; --i)
	{
		{projectedCoord = mul(float4(rayPosition.xyz, 1.0), uProjection);projectedCoord.xy = projectedCoord.xy / projectedCoord.w;projectedCoord.xy = uvScale * projectedCoord.xy + uvOffset;testPosition = uTexPosition.Sample(uTexPositionSampler, projectedCoord.xy);dDepth = rayPosition.z - testPosition.w;dir *= 0.5;if(dDepth > 0.0)	rayPosition -= dir;else	rayPosition += dir;};
		{projectedCoord = mul(float4(rayPosition.xyz, 1.0), uProjection);projectedCoord.xy = projectedCoord.xy / projectedCoord.w;projectedCoord.xy = uvScale * projectedCoord.xy + uvOffset;testPosition = uTexPosition.Sample(uTexPositionSampler, projectedCoord.xy);dDepth = rayPosition.z - testPosition.w;dir *= 0.5;if(dDepth > 0.0)	rayPosition -= dir;else	rayPosition += dir;};
		{projectedCoord = mul(float4(rayPosition.xyz, 1.0), uProjection);projectedCoord.xy = projectedCoord.xy / projectedCoord.w;projectedCoord.xy = uvScale * projectedCoord.xy + uvOffset;testPosition = uTexPosition.Sample(uTexPositionSampler, projectedCoord.xy);dDepth = rayPosition.z - testPosition.w;dir *= 0.5;if(dDepth > 0.0)	rayPosition -= dir;else	rayPosition += dir;};
		{projectedCoord = mul(float4(rayPosition.xyz, 1.0), uProjection);projectedCoord.xy = projectedCoord.xy / projectedCoord.w;projectedCoord.xy = uvScale * projectedCoord.xy + uvOffset;testPosition = uTexPosition.Sample(uTexPositionSampler, projectedCoord.xy);dDepth = rayPosition.z - testPosition.w;dir *= 0.5;if(dDepth > 0.0)	rayPosition -= dir;else	rayPosition += dir;};
	}
	return projectedCoord.xy;
}
float2 ssrRayMarch(const float4 position, const float3 reflection)
{
	float4 projectedCoord;
	float3 rayPosition = mul(float4(position.xyz, 1.0), uView).xyz;
	float3 viewReflection = normalize(mul(float4(reflection, 0.0), uView).xyz);
	float3 dir = viewReflection * 0.5;
	float2 ssrUV;
	float4 testPosition;
	const float2 uvOffset = float2(0.5, 0.5);
	const float2 uvScale = float2(0.5, -0.5);
	[unroll]
	for(int i = 8; i > 0; --i)
	{
		{rayPosition += dir;projectedCoord = mul(float4(rayPosition.xyz, 1.0), uProjection);projectedCoord.xy = projectedCoord.xy / projectedCoord.w;ssrUV = uvScale * projectedCoord.xy + uvOffset;testPosition = uTexPosition.Sample(uTexPositionSampler, ssrUV);if(rayPosition.z - testPosition.w >= 0.0){	return binarySearch(dir, rayPosition);}};
		{rayPosition += dir;projectedCoord = mul(float4(rayPosition.xyz, 1.0), uProjection);projectedCoord.xy = projectedCoord.xy / projectedCoord.w;ssrUV = uvScale * projectedCoord.xy + uvOffset;testPosition = uTexPosition.Sample(uTexPositionSampler, ssrUV);if(rayPosition.z - testPosition.w >= 0.0){	return binarySearch(dir, rayPosition);}};
		{rayPosition += dir;projectedCoord = mul(float4(rayPosition.xyz, 1.0), uProjection);projectedCoord.xy = projectedCoord.xy / projectedCoord.w;ssrUV = uvScale * projectedCoord.xy + uvOffset;testPosition = uTexPosition.Sample(uTexPositionSampler, ssrUV);if(rayPosition.z - testPosition.w >= 0.0){	return binarySearch(dir, rayPosition);}};
		{rayPosition += dir;projectedCoord = mul(float4(rayPosition.xyz, 1.0), uProjection);projectedCoord.xy = projectedCoord.xy / projectedCoord.w;ssrUV = uvScale * projectedCoord.xy + uvOffset;testPosition = uTexPosition.Sample(uTexPositionSampler, ssrUV);if(rayPosition.z - testPosition.w >= 0.0){	return binarySearch(dir, rayPosition);}};
	}
	return ssrUV;
}
float3 SSR(const float3 baseColor, const float4 position, const float3 reflection, const float roughness)
{
	float2 ssrUV = ssrRayMarch(position, reflection);
	float z = mul(float4(reflection, 0.0), uView).z;
	z = clamp(z, 0.0, 1.0);
	float mipLevel = roughness * 5.0;
	float3 color = uTexLastFrame.SampleLevel(uTexLastFrameSampler, ssrUV, mipLevel).rgb;
	float2 dCoords = smoothstep(float2(0.0, 0.0), float2(0.5, 0.5), abs(float2(0.5, 0.5) - ssrUV));
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	return lerp(baseColor * 0.8, color, screenEdgefactor * z);
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
float3 SG(
	const float3 baseColor,
	const float spec,
	const float gloss,
	const float4 position,
	const float3 worldViewDir,
	const float3 worldLightDir,
	const float3 worldNormal,
	const float3 lightColor,
	const float visibility,
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
	f0 = float3(0.04, 0.04, 0.04);
	float3 diffuseColor = baseColor.rgb;
	specularColor = lerp(f0, baseColor.rgb, metallic);
	specularColor = sRGB(specularColor);
	diffuseColor = sRGB(diffuseColor);
	float3 directionLightColor = sRGB(lightColor);
	float3 pointLightColor = sRGB(light.rgb);
	float3 indirectColor = sRGB(indirect.rgb);
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	NdotL = min(NdotL, 1.0);
	float3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0, dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * glossiness) * spec;
	float3 envSpecColor = lerp(indirectColor, float3(1.0, 1.0, 1.0), visibility);
	float3 directionalLight = NdotL * directionLightColor * visibility;
	float3 color = (directionalLight * directMultiplier + pointLightColor * lightMultiplier) * diffuseColor + specular * specularColor * envSpecColor + light.a * specularColor;
	color += indirectColor * diffuseColor * indirectMultiplier / PI;
	float3 reflection = -normalize(reflect(worldViewDir, worldNormal));
	color += sRGB(SSR(linearRGB(color), position, reflection, roughness)) * metallic * specularColor;
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
	float depth = length(v);
	float4 shadowCoord[3];
	shadowCoord[0] = mul(float4(position, 1.0), uShadowMatrix[0]);
	shadowCoord[1] = mul(float4(position, 1.0), uShadowMatrix[1]);
	shadowCoord[2] = mul(float4(position, 1.0), uShadowMatrix[2]);
	float shadowDistance[3];
	shadowDistance[0] = uShadowDistance.x;
	shadowDistance[1] = uShadowDistance.y;
	shadowDistance[2] = uShadowDistance.z;
	float visibility = shadow(shadowCoord, shadowDistance, depth);
	float3 color = SG(
		albedo,
		data.r,
		data.g,
		posdepth,
		viewDir,
		uLightDirection.xyz,
		normal,
		uLightColor.rgb * uLightColor.a,
		visibility,
		light,
		indirect,
		directMul,
		indirectMul,
		lightMul);
	return float4(color, 1.0);
}
