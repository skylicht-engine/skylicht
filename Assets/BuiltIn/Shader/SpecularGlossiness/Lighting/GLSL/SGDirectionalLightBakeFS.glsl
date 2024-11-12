// File Generated by Assets/BuildShader.py - source: [SGDirectionalLightFS.d.glsl : LIGHTMAP_BAKE]
precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;
uniform sampler2D uTexAlbedo;
uniform sampler2D uTexPosition;
uniform sampler2D uTexNormal;
uniform sampler2D uTexData;
uniform sampler2D uTexLight;
uniform sampler2D uTexIndirect;
uniform sampler2DArray uShadowMap;
uniform vec4 uCameraPosition;
uniform vec4 uLightDirection;
uniform vec4 uLightColor;
uniform vec3 uLightMultiplier;
uniform vec3 uShadowDistance;
uniform mat4 uShadowMatrix[3];
in vec2 varTexCoord0;
out vec4 FragColor;
float shadow(const vec4 shadowCoord[3], const float shadowDistance[3], const float farDistance)
{
	int id = 0;
	float depth = 0.0;
	float result = 0.0;
	const float bias[3] = float[3](0.0001, 0.0002, 0.0006);
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
	depth -= bias[id];
	vec2 uv = shadowUV.xy;
	float size = 1.0/2048.0;
	vec2 off;
	vec2 rand;
	const vec2 kRandom1 = vec2(12.9898,78.233);
	const float kRandom2 = 43758.5453;
	const float kRandom3 = 0.00047;
	{off = vec2(-1, -1) * size;rand = uv + off;rand += (vec2(fract(sin(dot(rand.xy, kRandom1)) * kRandom2), fract(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, textureLod(uShadowMap, vec3(rand, id), 0.0).r));}
	{off = vec2(0, -1) * size;rand = uv + off;rand += (vec2(fract(sin(dot(rand.xy, kRandom1)) * kRandom2), fract(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, textureLod(uShadowMap, vec3(rand, id), 0.0).r));}
	{off = vec2(1, -1) * size;rand = uv + off;rand += (vec2(fract(sin(dot(rand.xy, kRandom1)) * kRandom2), fract(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, textureLod(uShadowMap, vec3(rand, id), 0.0).r));}
	{off = vec2(-1, 0) * size;rand = uv + off;rand += (vec2(fract(sin(dot(rand.xy, kRandom1)) * kRandom2), fract(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, textureLod(uShadowMap, vec3(rand, id), 0.0).r));}
	{off = vec2(0, 0) * size;rand = uv + off;rand += (vec2(fract(sin(dot(rand.xy, kRandom1)) * kRandom2), fract(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, textureLod(uShadowMap, vec3(rand, id), 0.0).r));}
	{off = vec2(1, 0) * size;rand = uv + off;rand += (vec2(fract(sin(dot(rand.xy, kRandom1)) * kRandom2), fract(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, textureLod(uShadowMap, vec3(rand, id), 0.0).r));}
	{off = vec2(-1, 1) * size;rand = uv + off;rand += (vec2(fract(sin(dot(rand.xy, kRandom1)) * kRandom2), fract(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, textureLod(uShadowMap, vec3(rand, id), 0.0).r));}
	{off = vec2(0, 1) * size;rand = uv + off;rand += (vec2(fract(sin(dot(rand.xy, kRandom1)) * kRandom2), fract(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, textureLod(uShadowMap, vec3(rand, id), 0.0).r));}
	{off = vec2(1, 1) * size;rand = uv + off;rand += (vec2(fract(sin(dot(rand.xy, kRandom1)) * kRandom2), fract(sin(dot(rand.yx, kRandom1)) * kRandom2)) * kRandom3);result += (step(depth, textureLod(uShadowMap, vec3(rand, id), 0.0).r));}
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
	const vec4 position,
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
	float glossiness = max(gloss, 0.01);
	float roughness = 1.0 - glossiness;
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
	float specular = pow(NdotE, 100.0f * glossiness) * spec;
	vec3 envSpecColor = mix(indirectColor, vec3(1.0, 1.0, 1.0), visibility);
	vec3 directionalLight = NdotL * directionLightColor * visibility;
	vec3 color = (directionalLight * directMultiplier + pointLightColor * lightMultiplier) * diffuseColor;
	color += specular * specularColor * envSpecColor;
	color += light.a * specularColor;
	color += indirectColor * diffuseColor * indirectMultiplier / PI;
	return color;
}
void main(void)
{
	vec3 albedo = texture(uTexAlbedo, varTexCoord0.xy).rgb;
	vec4 posdepth = texture(uTexPosition, varTexCoord0.xy);
	vec3 position = posdepth.xyz;
	vec3 normal = texture(uTexNormal, varTexCoord0.xy).xyz;
	vec3 data = texture(uTexData, varTexCoord0.xy).rgb;
	vec4 light = texture(uTexLight, varTexCoord0.xy);
	vec3 indirect = texture(uTexIndirect, varTexCoord0.xy).rgb;
	vec3 v = uCameraPosition.xyz - position;
	vec3 viewDir = normalize(v);
	float directMul = uLightMultiplier.x;
	float indirectMul = uLightMultiplier.y;
	float lightMul = uLightMultiplier.z;
	if (dot(viewDir, normal) < 0.0)
	{
		normal = normal * -1.0;
		directMul = 0.0;
		indirectMul = 0.0;
		lightMul = 0.0;
	}
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
	FragColor = vec4(color, 1.0);
}
