////////////////////////////////////////
//Constant buffers
////////////////////////////////////////
cbuffer MatrixBuffer
{
	matrix WorldViewProjection;
};

////////////////////////////////////////
//Structures
////////////////////////////////////////
struct vs_input
{
	float3 pos	:	POSITION;
	float2 texCoord	:	TEXCOORDS;
};

struct ps_input
{
	float4 pos	:	SV_POSITION;
	float2 texCoord	:	TEXCOORDS;
};

///////////////////////////////////////
//vertex shader
ps_input FontVS(vs_input input)
{
	ps_input output;

	output.pos = mul(float4(input.pos, 1.0f), WorldViewProjection);
	output.texCoord = input.texCoord;

	return output;
}