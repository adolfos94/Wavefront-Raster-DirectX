cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

struct VS_INPUT
{
	float3 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 texCoord	: TEXCOORD;
};

struct PS_INPUT
{
	float4 pos		: SV_POSITION;
	float3 color	: COLOR0;
};

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output;
	float4 pos = float4(input.pos, 1.0f);

	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	float3 normal = float3(input.normal);
	output.color = normal * 0.5 + 0.5;

	return output;
}