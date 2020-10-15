// File Generated by Assets/BuildShader.py - source: [SGDirectionalLightFS.d.glsl]
precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;
uniform sampler2D uTexAlbedo;
uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;
uniform sampler2D uTexLight;
uniform sampler2D uTexIndirect;
uniform sampler2D uTexLastFrame;
uniform sampler2DArray uShadowMap;
uniform vec4 uCameraPosition;
uniform vec4 uLightDirection;
uniform vec4 uLightColor;
uniform vec3 uLightMultiplier;
uniform vec3 uShadowDistance;
uniform mat4 uShadowMatrix[3];
uniform mat4 uViewProjection;
uniform mat4 uView;
in vec2 varTexCoord0;
out vec4 FragColor;
float texture2DCompare(vec3 uv, float compare) {
	float depth = texture(uShadowMap, uv).r;
	return step(compare, depth);
}
float shadow(const vec4 shadowCoord[3], const float shadowDistance[3], const float farDistance)
{
	int id = 0;
	float visible = 1.0;
	float bias = 0.0001;
	float depth = 0.0;
	float result = 0.0;
	float size = 2048.0;
	if (farDistance < shadowDistance[0])
		id = 0;
	else if (farDistance < shadowDistance[1])
		id = 1;
	else if (farDistance < shadowDistance[2])
		id = 2;
	else
		return 1.0;
	vec3 shadowUV = shadowCoord[id].xyz / shadowCoord[id].w;
	depth = shadowUV.z;
	vec2 uv = shadowUV.xy;
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			vec2 off = vec2(x, y) / size;
			result += texture2DCompare(vec3(uv + off, id), depth - bias);
		}
	}
	return result / 9.0;
}
const float PI = 3.1415926;
const float MinReflectance = 0.04;
float getPerceivedBrightness(vec3 color)
{
	return sqrt(0.299 * color.r * color.r + 0.587 * color.g * color.g + 0.114 * color.b * color.b);
}
float solveMetallic(vec3 diffuse, vec3 specular, float oneMinusSpecularStrength)
{
	float specularBrightness = getPerceivedBrightness(specular);
	float diffuseBrightness = getPerceivedBrightness(diffuse);
	float a = MinReflectance;
	float b = diffuseBrightness * oneMinusSpecularStrength / (1.0 - MinReflectance) + specularBrightness - 2.0 * MinReflectance;
	float c = MinReflectance - specularBrightness;
	float D = b * b - 4.0 * a * c;
	return clamp((-b + sqrt(D)) / (2.0 * a), 0.0, 1.0);
}
vec3 SSR(const vec3 baseColor, const vec3 position, const vec3 reflection, const float roughness)
{
	vec4 projectedCoord;
	vec3 beginPosition;
	vec3 endPosition;
	vec3 rayPosition = position;
	vec4 viewPosition;
	vec3 dir = reflection * 8.0;
	float mipLevel = roughness * 5.0;
	for (int i = 8; i >= 0; --i)
	{
		beginPosition = rayPosition;
		endPosition = rayPosition + dir;
		rayPosition += dir * 0.5;
		projectedCoord = uViewProjection * vec4(rayPosition.xyz, 1.0);
		projectedCoord.xy = projectedCoord.xy / projectedCoord.w;
		projectedCoord.xy = 0.5 * projectedCoord.xy + vec2(0.5, 0.5);
		vec3 testPosition = texture(uTexPosition, projectedCoord.xy).xyz;
		vec3 d1 = testPosition - beginPosition;
		float lengthSQ1 = d1.x*d1.x + d1.y*d1.y + d1.z*d1.z;
		vec3 d2 = testPosition - endPosition;
		float lengthSQ2 = d2.x*d2.x + d2.y*d2.y + d2.z*d2.z;
		if (lengthSQ1 < lengthSQ2)
		{
			rayPosition = beginPosition;
		}
		dir *= 0.5;
	}
	float z = (uView * vec4(reflection, 0.0)).z;
	z = clamp(z, 0.0, 1.0);
	vec3 color = textureLod(uTexLastFrame, projectedCoord.xy, mipLevel).rgb;
	vec2 dCoords = smoothstep(vec2(0.0, 0.0), vec2(0.5, 0.5), abs(vec2(0.5, 0.5) - projectedCoord.xy));
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	return mix(baseColor * 0.8, color, screenEdgefactor * z);
}
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
vec3 SG(
	const vec3 baseColor,
	const float spec,
	const float gloss,
	const vec3 position,
	const vec3 worldViewDir,
	const vec3 worldLightDir,
	const vec3 worldNormal,
	const vec3 lightColor,
	const float visibility,
	const vec4 light,
	const vec3 indirect,
	const float directMultiplier,
	const float indirectMultiplier,
	const float lightMultiplier)
{
	float roughness = 1.0 - gloss;
	vec3 f0 = vec3(spec, spec, spec);
	vec3 specularColor = f0;
	float oneMinusSpecularStrength = 1.0 - spec;
	float metallic = solveMetallic(baseColor.rgb, specularColor, oneMinusSpecularStrength);
	f0 = vec3(0.04, 0.04, 0.04);
	vec3 diffuseColor = baseColor.rgb;
	specularColor = mix(f0, baseColor.rgb, metallic);
	specularColor = sRGB(specularColor);
	diffuseColor = sRGB(diffuseColor);
	vec3 directionLightColor = sRGB(lightColor);
	vec3 pointLightColor = sRGB(light.rgb);
	vec3 indirectColor = sRGB(indirect.rgb);
	float NdotL = max(dot(worldNormal, worldLightDir), 0.0);
	NdotL = min(NdotL, 1.0);
	vec3 H = normalize(worldLightDir + worldViewDir);
	float NdotE = max(0.0, dot(worldNormal, H));
	float specular = pow(NdotE, 100.0f * gloss) * spec;
	vec3 directionalLight = NdotL * directionLightColor * visibility;
	vec3 color = (directionalLight * directMultiplier + pointLightColor * lightMultiplier) * diffuseColor + specular * specularColor * visibility + light.a * specularColor;
	color += indirectColor * diffuseColor * indirectMultiplier / PI;
	vec3 reflection = -normalize(reflect(worldViewDir, worldNormal));
	color += sRGB(SSR(linearRGB(color), position, reflection, roughness)) * metallic * specularColor;
	return color;
}
void main(void)
{
	vec3 albedo = texture(uTexAlbedo, varTexCoord0.xy).rgb;
	vec3 position = texture(uTexPosition, varTexCoord0.xy).xyz;
	vec3 normal = texture(uTexNormal, varTexCoord0.xy).xyz;
	vec3 data = texture(uTexData, varTexCoord0.xy).rgb;
	vec4 light = texture(uTexLight, varTexCoord0.xy);
	vec3 indirect = texture(uTexIndirect, varTexCoord0.xy).rgb;
	vec3 v = uCameraPosition.xyz - position;
	vec3 viewDir = normalize(v);
	float depth = length(v);
	vec4 shadowCoord[3];
	shadowCoord[0] = uShadowMatrix[0] * vec4(position, 1.0);
	shadowCoord[1] = uShadowMatrix[1] * vec4(position, 1.0);
	shadowCoord[2] = uShadowMatrix[2] * vec4(position, 1.0);
	float shadowDistance[3];
	shadowDistance[0] = uShadowDistance.x;
	shadowDistance[1] = uShadowDistance.y;
	shadowDistance[2] = uShadowDistance.z;
	float visibility = shadow(shadowCoord, shadowDistance, depth);
	vec3 color = SG(
		albedo,
		data.r,
		data.g,
		position,
		viewDir,
		uLightDirection.xyz,
		normal,
		uLightColor.rgb * uLightColor.a,
		visibility,
		light,
		indirect,
		uLightMultiplier.x,
		uLightMultiplier.y,
		uLightMultiplier.z);
	FragColor = vec4(color, 1.0);
}
