precision mediump float;
uniform sampler2D uTexDiffuse;
in vec2 varTexCoord0;
in vec4 varColor;
out vec4 FragColor;
const float gamma = 2.2;
const float invGamma = 1.0/2.2;
vec3 sRGB(vec3 color)
{
	return pow(color, vec3(gamma));
}
vec3 linearRGB(vec3 color)
{
	return pow(color, vec3(invGamma));
}
void main(void)
{
	vec4 result = texture(uTexDiffuse, varTexCoord0.xy) * varColor;
	FragColor = vec4(linearRGB(sRGB(result.rgb)), result.a);
}
