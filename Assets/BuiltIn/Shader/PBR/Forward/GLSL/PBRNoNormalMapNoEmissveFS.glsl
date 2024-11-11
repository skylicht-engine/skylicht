// File Generated by Assets/BuildShader.py - source: [PBRLitFS.d.glsl : NO_NORMAL_MAP]
precision mediump float;
uniform sampler2D uTexAlbedo;
uniform sampler2D uTexRMA;
uniform samplerCube uTexReflect;
uniform sampler2D uTexBRDF;
uniform vec4 uLightColor;
uniform vec4 uColor;
uniform vec4 uSHConst[4];
in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;
in vec3 vWorldLightDir;
in vec4 vViewPosition;
in vec3 vWorldPosition;
out vec4 FragColor;
const float gamma = 2.2;
const float invGamma = 1.0 / 2.2;
vec3 sRGB(vec3 color)
{
	return pow(color, vec3(gamma));
}
vec3 linearRGB(vec3 color)
{
	return pow(color, vec3(invGamma));
}
vec3 shAmbient(vec3 n)
{
	vec3 ambientLighting = uSHConst[0].xyz +
		uSHConst[1].xyz * n.y +
		uSHConst[2].xyz * n.z +
		uSHConst[3].xyz * n.x;
	return ambientLighting * 0.9;
}
const float PI = 3.1415926;
vec3 fresnelSchlick(vec3 V, vec3 H, vec3 F0)
{
	float VdotH = max(dot(V, H), 0.001);
	return F0 + (1.0 - F0) * pow((1.0 - VdotH), 5.0);
}
float gaSchlickG1(vec3 N, vec3 V, float k)
{
	float NdotV = max(dot(N, V), 0.001);
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return nom / denom;
}
float schlickGGX(vec3 N, vec3 V, vec3 L, float roughness)
{
	float k = 0.001 + (roughness + 1.0) * (roughness + 1.0) / 8.0;
	return gaSchlickG1(N, L, k) * gaSchlickG1(N, V, k);
}
float ndfGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float nom = a2;
	float denom = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
	denom = PI * denom * denom;
	return nom / denom;
}
vec3 computeLightContribution(vec3 N, vec3 L, vec3 V, vec3 F0, vec3 lambert, vec3 lightColor, float VdotN, float roughness, float metalness)
{
	vec3 H = normalize(L + V);
	float D = ndfGGX(N, H, roughness);
	float G = schlickGGX(N, V, L, roughness);
	vec3 F = fresnelSchlick(V, H, F0);
	float LdotN = max(dot(N, L), 0.0);
	vec3 specularBRDF = (D * F * G) / (4.0 * VdotN * LdotN + 0.001);
	vec3 kd = mix(vec3(1.0, 1.0, 1.0) - F, vec3(0.0, 0.0, 0.0), metalness);
	vec3 BRDF = kd * lambert + specularBRDF;
	return BRDF * lightColor * LdotN;
}
void main(void)
{
	vec4 albedoMap = texture(uTexAlbedo, vTexCoord0.xy) * uColor;
	vec3 rmaMap = texture(uTexRMA, vTexCoord0.xy).xyz;
	vec3 n = vWorldNormal;
	float roughness = rmaMap.r;
	float metalness = rmaMap.g;
	float ao = rmaMap.b;
	vec3 ambientLighting = shAmbient(n);
	ambientLighting = sRGB(ambientLighting);
	vec3 albedo = sRGB(albedoMap.rgb);
	vec3 F0 = vec3(0.04, 0.04, 0.04);
	F0 = mix(F0, albedo, metalness);
	float VdotN = max(dot(vWorldViewDir, n), 0.0);
	vec3 lambert = albedo / PI;
	vec3 lightContribution = computeLightContribution(n, vWorldLightDir, vWorldViewDir, F0, lambert, sRGB(uLightColor.rgb), VdotN, roughness, metalness);
	vec3 F = fresnelSchlick(vWorldViewDir, n, F0);
	vec3 kd = vec3(1.0) - F;
	kd *= (1.0 - metalness);
	vec3 indirectDiffuse = ambientLighting * lambert;
	vec3 reflection = -normalize(reflect(vWorldViewDir, n));
	vec3 prefilteredColor = sRGB(textureLod(uTexReflect, reflection, roughness * 7.0).xyz);
	vec2 envBRDF = texture(uTexBRDF, vec2(VdotN, roughness)).rg;
	vec3 indirectSpecular = prefilteredColor * (F0 * envBRDF.x + envBRDF.y);
	float brightness = (0.8 + (1.0 - roughness) * 2.2);
	vec3 indirectLight = (kd * indirectDiffuse + indirectSpecular * brightness);
	FragColor = vec4((lightContribution + indirectLight) * ao, albedoMap.a);
}
