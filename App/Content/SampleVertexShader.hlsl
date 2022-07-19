cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

struct VS_INPUT
{
	float3 pos		: POSITION;
	float3 color	: COLOR0;
	//float2 tex		: TEXCOORD0;
};

struct PS_INPUT
{
	float4 pos		: SV_POSITION;
	float3 color	: COLOR0;
	//float2 tex		: TEXCOORD0;
};

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output;
	float4 pos = float4(input.pos, 1.0f);

	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;
	//output.color = float3(255, 0, 0);
	//output.color = input.color;
	//output.tex = input.tex;

	return output;
}