in vec4 inPosition;
in vec3 inNormal;
in vec4 inColor;
in vec2 inTexCoord0;
in vec3 inTangent;
in vec3 inBinormal;
in vec2 inTangentW;

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;
uniform vec4 uCameraPosition;
uniform vec4 uLightDirection;
uniform vec4 uUVScale;

out vec2 vTexCoord0;
out vec3 vWorldNormal;
out vec3 vWorldViewDir;
out vec3 vWorldLightDir;
out vec3 vWorldTangent;
out vec3 vWorldBinormal;
out float vTangentW;
out vec4 vViewPosition;
out vec3 vWorldPosition;

void main(void)
{
	vTexCoord0 = inTexCoord0 * uUVScale.xy + uUVScale.zw;
	vTangentW = inTangentW.x;

	vec4 worldPos = uWorldMatrix * inPosition;
	vec4 camPos = uWorldMatrix * uCameraPosition;

	vec4 worldViewDir = normalize(camPos - worldPos);

	vec4 worldNormal = uWorldMatrix * vec4(inNormal.xyz, 0.0);
	vec4 worldTangent = uWorldMatrix * vec4(inTangent.xyz, 0.0);

	vWorldPosition = worldPos.xyz;

	vWorldNormal = normalize(worldNormal.xyz);
	vWorldTangent = normalize(worldTangent.xyz);
	vWorldBinormal = normalize(cross(worldNormal.xyz, worldTangent.xyz));

	vWorldViewDir = worldViewDir.xyz;
	vWorldLightDir = normalize(uLightDirection.xyz);
	vViewPosition = uMvpMatrix * inPosition;

	gl_Position = vViewPosition;
}