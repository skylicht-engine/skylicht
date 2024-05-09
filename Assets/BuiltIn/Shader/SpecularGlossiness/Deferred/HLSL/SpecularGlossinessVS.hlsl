struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	float3 tangent: TANGENT;
	float3 binormal: BINORMAL;
	float2 data: DATA;
#ifdef INSTANCING
	float4 uvScale: TEXCOORD1;
	float4 uColor: TEXCOORD2;
	float4 uSpecGloss: TEXCOORD3;
	float4x4 worldMatrix: TEXCOORD4;
#endif
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 worldPosition: WORLDPOSITION;
	float3 worldNormal: WORLDNORMAL;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
#ifdef INSTANCING
	float4 color: COLOR;
	float2 specGloss: SPECGLOSS;
#endif
};

#ifdef INSTANCING
cbuffer cbPerObject
{
	float4x4 uVPMatrix;
	float4x4 uView;
};
#else
cbuffer cbPerObject
{
	float4x4 uMvpMatrix;
	float4x4 uWorldMatrix;
	float4x4 uView;
	float4 uUVScale;
};
#endif

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	
#ifdef INSTANCING
	output.color = input.uColor;
	output.specGloss = input.uSpecGloss.xy;
	float4x4 uWorldMatrix = transpose(input.worldMatrix);
	float4 uUVScale = input.uvScale;
#endif

	output.tex0 = input.tex0 * uUVScale.xy + uUVScale.zw;
	output.tangentw = input.data.x;

	float4 worldPos = mul(input.pos, uWorldMatrix);
	float4 worldNormal = mul(float4(input.norm, 0.0), uWorldMatrix);
	float4 worldTangent = mul(float4(input.tangent, 0.0), uWorldMatrix);

#ifdef INSTANCING
	output.pos = mul(worldPos, uVPMatrix);
#else
	output.pos = mul(input.pos, uMvpMatrix);
#endif

	float4 sampleFragPos = mul(worldPos, uView);

	output.worldPosition = float4(worldPos.xyz, sampleFragPos.z);
	output.worldNormal = normalize(worldNormal.xyz);
	output.worldTangent = normalize(worldTangent.xyz);
	output.worldBinormal = normalize(cross(worldNormal.xyz, worldTangent.xyz));

	return output;
}