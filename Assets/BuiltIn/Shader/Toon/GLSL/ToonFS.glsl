// File Generated by Assets/BuildShader.py - source: [ToonFS.d.glsl : _]
precision mediump float;
precision highp sampler2D;
precision highp sampler2DArray;
uniform sampler2D uTexDiffuse;
uniform sampler2D uTexRamp;
uniform vec4 uLightDirection;
uniform vec4 uLightColor;
uniform vec4 uColor;
uniform vec4 uShadowColor;
uniform vec2 uWrapFactor;
uniform vec3 uSpecular;
uniform vec4 uSHConst[4];
in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;
in vec3 vWorldPosition;
in vec3 vDepth;
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
const float PI = 3.1415926;
void main(void)
{
	vec4 diffuse = texture(uTexDiffuse, vTexCoord0.xy);
	vec3 diffuseMap = sRGB(diffuse.rgb);
	float NdotL = (dot(vWorldNormal, uLightDirection.xyz) + uWrapFactor.x) / (1.0 + uWrapFactor.x);
	NdotL = max(NdotL, 0.0);
	vec3 rampMap = texture(uTexRamp, vec2(NdotL, NdotL)).rgb;
	vec3 color = sRGB(uColor.rgb);
	float shadowIntensity = uColor.a;
	vec3 shadowColor = sRGB(uShadowColor.rgb);
	vec3 lightColor = sRGB(uLightColor.rgb);
	float visibility = 1.0;
	vec3 ramp = mix(color, shadowColor, shadowIntensity * (1.0 - visibility));
	ramp = mix(ramp, color, rampMap);
	vec3 h = normalize(uLightDirection.xyz + vWorldViewDir);
	float NdotH = dot(vWorldNormal, h);
	NdotH = max(NdotH, 0.0);
	float spec = pow(NdotH, uSpecular.x*128.0) * uSpecular.y;
	spec = smoothstep(0.5-uSpecular.z*0.5, 0.5+uSpecular.z*0.5, spec);
	FragColor = vec4(diffuseMap * lightColor * ramp * (0.5 + visibility * 0.5) + lightColor * spec * visibility, diffuse.a);
}
