// File Generated by Assets/BuildShader.py - source: [SGLitFS.d.glsl : NO_NORMAL_MAP]
precision mediump float;
precision highp sampler2D;
precision highp sampler2DArray;
uniform sampler2D uTexDiffuse;
uniform sampler2D uTexSpecular;
uniform vec4 uLightColor;
uniform vec4 uColor;
uniform vec4 uSHConst[4];
in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;
in vec3 vWorldLightDir;
in vec4 vViewPosition;
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
void main(void)
{
	vec4 diffuseMap = texture(uTexDiffuse, vTexCoord0.xy) * uColor;
	vec3 specMap = texture(uTexSpecular, vTexCoord0.xy).xyz;
	vec3 n = vWorldNormal;
	vec3 ambientLighting = shAmbient(n);
	ambientLighting = sRGB(ambientLighting);
	vec3 diffuseColor = sRGB(diffuseMap.rgb);
	vec3 lightColor = sRGB(uLightColor.rgb);
	float NdotL = max(dot(n, vWorldLightDir), 0.0);
	vec3 directionalLight = NdotL * lightColor;
	vec3 color = directionalLight * diffuseColor;
	vec3 H = normalize(vWorldLightDir + vWorldViewDir);
	float NdotE = max(0.0, dot(n, H));
	float specular = pow(NdotE, 10.0 + 100.0 * specMap.g) * specMap.r;
	color += specular;
	color += ambientLighting * diffuseColor / PI;
	FragColor = vec4(color, diffuseMap.a);
}
