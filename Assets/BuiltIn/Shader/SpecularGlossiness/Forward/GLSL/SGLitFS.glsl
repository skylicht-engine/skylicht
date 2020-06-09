precision mediump float;

uniform sampler2D uTexDiffuse;
uniform sampler2D uTexNormal;
uniform sampler2D uTexSpecular;
uniform samplerCube uTexReflect;

uniform vec4 uLightColor;
uniform vec4 uSHConst[4];

in vec2 vTexCoord0;
in vec3 vWorldNormal;
in vec3 vWorldViewDir;
in vec3 vWorldLightDir;
in vec3 vWorldTangent;
in vec3 vWorldBinormal;
in float vTangentW;
in vec4 vViewPosition;
in vec3 vWorldPosition;

out vec4 FragColor;

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

void main(void)
{
	vec4 diffuseMap = texture(uTexDiffuse, vTexCoord0.xy);
	vec3 normalMap = texture(uTexNormal, vTexCoord0.xy).xyz;
	vec3 specMap = texture(uTexSpecular, vTexCoord0.xy).xyz;
	
	mat3 rotation = mat3(vWorldTangent, vWorldBinormal, vWorldNormal);
	vec3 localCoords = normalMap * 2.0 - vec3(1.0, 1.0, 1.0);
	localCoords.y *= vTangentW;
	vec3 n = normalize(rotation * localCoords);
	n = normalize(n);
	
	// Solver metallic
	float roughness = 1.0 - specMap.g;

	vec3 f0 = vec3(specMap.r, specMap.r, specMap.r);
	vec3 specularColor = f0;
    float oneMinusSpecularStrength = 1.0 - specMap.r;
	float metallic = solveMetallic(diffuseMap.rgb, specularColor, oneMinusSpecularStrength);
	
	f0 = vec3(0.04, 0.04, 0.04);
	vec3 diffuseColor = diffuseMap.rgb * (vec3(1.0, 1.0, 1.0) - f0) * (1.0 - metallic);
    specularColor = mix(f0, diffuseMap.rgb, metallic);
	
	// SH Ambient
	vec3 ambientLighting = uSHConst[0].xyz +
		uSHConst[1].xyz * n.y +
		uSHConst[2].xyz * n.z +
		uSHConst[3].xyz * n.x;
	
	// Lighting
	float NdotL = max(dot(vWorldNormal, vWorldLightDir), 0.0);
	vec3 directionalLight = NdotL * uLightColor.rgb;
	vec3 color = directionalLight * diffuseColor;

	// Specular
	vec3 H = normalize(vWorldLightDir + vWorldViewDir);	
	float NdotE = max(0.0,dot(vWorldNormal, H));
	float specular = pow(NdotE, 100.0f * specMap.g) * specMap.r;
	color += specular * uLightColor.rgb;
	
	// IBL lighting (2 bounce)
	color += ambientLighting * diffuseColor * 2 / PI;
	
	// IBL reflection
	vec3 reflection = -normalize(reflect(vWorldViewDir, vWorldNormal));
	color += textureLod(uTexReflect, reflection, roughness * 8).xyz * specularColor * metallic;	
	
	FragColor = vec4(color, diffuseMap.a);
}