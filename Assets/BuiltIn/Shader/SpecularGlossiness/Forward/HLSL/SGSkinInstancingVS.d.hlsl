Texture2D uTransformTexture : register(t4);
SamplerState uTransformTextureSampler : register(s4);

struct VS_INPUT
{
	float4 pos: POSITION;
	float3 norm: NORMAL;
	float4 color: COLOR;
	float2 tex0: TEXCOORD0;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float2 data : DATA;
	float4 blendIndex : BLENDINDICES;
	float4 blendWeight : BLENDWEIGHT;
	
	float4 uBoneLocation: TEXCOORD1;
	float4 uColor: TEXCOORD2;
	float2 uBlendAnimation: TEXCOORD3;
	
	float4x4 worldMatrix: TEXCOORD4;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 worldNormal: WORLDNORMAL;
	float3 worldViewDir: WORLDVIEWDIR;
	float3 worldLightDir: WORLDLIGHTDIR;
	float3 worldTangent: WORLDTANGENT;
	float3 worldBinormal: WORLDBINORMAL;
	float tangentw : TANGENTW;
	float4 viewPosition: VIEWPOSITION;
	float3 worldPosition: WORLDPOSITION;
};

cbuffer cbPerObject
{
	float4x4 uVpMatrix;
	float4 uCameraPosition;
	float4 uLightDirection;
	float4 uAnimation;
	float2 uTransformTextureSize;
};

#include "../../../TransformTexture/HLSL/LibTransformTexture.hlsl"

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	float4x4 uWorldMatrix = transpose(input.worldMatrix);

	float4x4 skinMatrix;
	float4 skinPosition;
	float4 skinNormal;
	float4 skinTangent;

	float2 boneLocation = input.uBoneLocation.xy;
	float boneLocationY = input.uBoneLocation.y;

	// bone 0
	boneLocation.y = boneLocationY + input.blendIndex[0];
	skinMatrix = input.blendWeight[0] * getTransformFromTexture(boneLocation);

	// bone 1
	boneLocation.y = boneLocationY + input.blendIndex[1];
	skinMatrix += input.blendWeight[1] * getTransformFromTexture(boneLocation);

	// bone 2
	boneLocation.y = boneLocationY + input.blendIndex[2];
	skinMatrix += input.blendWeight[2] * getTransformFromTexture(boneLocation);

	// bone 3
	boneLocation.y = boneLocationY + input.blendIndex[3];
	skinMatrix += input.blendWeight[3] * getTransformFromTexture(boneLocation);

	// skin result
	skinPosition = mul(input.pos, skinMatrix);
	skinNormal = mul(float4(input.norm, 0.0), skinMatrix);
	skinTangent = mul(float4(input.tangent, 0.0), skinMatrix);

	output.tex0 = input.tex0;
	output.tangentw = input.data.x;

	float4 worldPos = mul(skinPosition, uWorldMatrix);
	float4 worldViewDir = normalize(uCameraPosition - worldPos);

	float4 worldNormal = mul(float4(skinNormal.xyz, 0.0), uWorldMatrix);
	float4 worldTangent = mul(float4(skinTangent.xyz, 0.0), uWorldMatrix);

	output.worldPosition = worldPos.xyz;

	output.worldNormal = normalize(worldNormal.xyz);
	output.worldTangent = normalize(worldTangent.xyz);
	output.worldBinormal = normalize(cross(worldNormal.xyz, worldTangent.xyz));

	output.worldViewDir = worldViewDir.xyz;
	output.worldLightDir = normalize(uLightDirection.xyz);
	
	output.pos = mul(worldPos, uVpMatrix);
	output.viewPosition = output.pos;

	return output;
}