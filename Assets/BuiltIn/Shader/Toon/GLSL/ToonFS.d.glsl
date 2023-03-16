precision mediump float;
precision highp sampler2D;
precision highp sampler2DArray;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexRamp;
#if defined(SHADOW)
uniform sampler2DArray uShadowMap;
#endif

uniform vec4 uLightDirection;
uniform vec4 uLightColor;
uniform vec4 uColor;
uniform vec4 uShadowColor;
uniform vec2 uWrapFactor;
uniform vec3 uSpecular;
#if defined(SHADOW)
uniform vec3 uShadowDistance;
uniform mat4 uShadowMatrix[3];
#endif

in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;
in vec3 vWorldPosition;
in vec3 vDepth;

out vec4 FragColor;

#include "../../PostProcessing/GLSL/LibToneMapping.glsl"

#if defined(SHADOW)
#include "../../Shadow/GLSL/LibShadow.glsl"
#endif

void main(void)
{
	vec3 diffuseMap = sRGB(texture(uTexDiffuse, vTexCoord0.xy).rgb);
	
	float NdotL = (dot(vWorldNormal, uLightDirection.xyz) + uWrapFactor.x) / (1.0 + uWrapFactor.x);
	NdotL = max(NdotL, 0.0);
	
	vec3 rampMap = texture(uTexRamp, vec2(NdotL, NdotL)).rgb;
	
	vec3 color = sRGB(uColor.rgb);
	vec3 shadowColor = sRGB(uShadowColor.rgb);
	vec3 lightColor = sRGB(uLightColor.rgb);
	
	float visibility = 1.0;
	
#if defined(SHADOW)
	float depth = length(vDepth);

	vec4 shadowCoord[3];
	shadowCoord[0] = uShadowMatrix[0] * vec4(vWorldPosition, 1.0);
	shadowCoord[1] = uShadowMatrix[1] * vec4(vWorldPosition, 1.0);
	shadowCoord[2] = uShadowMatrix[2] * vec4(vWorldPosition, 1.0);

	float shadowDistance[3];
	shadowDistance[0] = uShadowDistance.x;
	shadowDistance[1] = uShadowDistance.y;
	shadowDistance[2] = uShadowDistance.z;

	visibility = shadow(shadowCoord, shadowDistance, depth);
#endif
	
	// Shadows intensity through alpha
	vec3 ramp = mix(color, shadowColor, uColor.a * (1.0 - visibility));
	ramp = mix(ramp, color, rampMap);
	
	// Specular
	vec3 h = normalize(uLightDirection.xyz + vWorldViewDir);
	
	float NdotH = dot(vWorldNormal, h);
	NdotH = max(NdotH, 0.0);
	
	float spec = pow(NdotH, uSpecular.x*128.0) * uSpecular.y;
	spec = smoothstep(0.5-uSpecular.z*0.5, 0.5+uSpecular.z*0.5, spec);
	
	FragColor = vec4(diffuseMap * lightColor * ramp * (0.5 + visibility * 0.5) + lightColor * spec * visibility, 1.0);
}