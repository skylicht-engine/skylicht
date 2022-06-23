precision highp float;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexNormal;
uniform sampler2D uTexSpec;

uniform vec4 uColor;
uniform vec2 uGloss;

in vec2 vTexCoord0;
in vec4 vWorldPosition;
in vec3 vWorldNormal;
in vec3 vWorldTangent;
in vec3 vWorldBinormal;
in float vTangentW;

layout(location = 0) out vec4 Diffuse;
layout(location = 1) out vec4 Position;
layout(location = 2) out vec4 Normal;
layout(location = 3) out vec4 SG;

void main(void)
{
	vec3 baseMap = texture(uTexDiffuse, vTexCoord0.xy).rgb;
	vec3 normalMap = texture(uTexNormal, vTexCoord0.xy).xyz;
	vec3 specMap = texture(uTexSpec, vTexCoord0.xy).rgb;

	mat3 rotation = mat3(vWorldTangent, vWorldBinormal, vWorldNormal);
	vec3 localCoords = normalMap * 2.0 - vec3(1.0, 1.0, 1.0);
	localCoords.y = localCoords.y * vTangentW;
	vec3 n = rotation * localCoords;
	n = normalize(n);

	Diffuse = vec4(baseMap * uColor.rgb, 1.0);
	Position = vWorldPosition;
	Normal = vec4(n, 1.0);
	SG = vec4(specMap.r, uGloss.y, 1.0, 1.0);
}
