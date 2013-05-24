/////////////////////////////////////////////
//Constant Buffers
/////////////////////////////////////////////
cbuffer MatrixBufferTex
{
	matrix WorldViewProjection;
};

/////////////////////////////////////////////
//ShaderResource Views
/////////////////////////////////////////////
Texture2D sceneTexture	:	register( t0 );

/////////////////////////////////////////////
//Structures
/////////////////////////////////////////////
struct vs_input
{
	float3 pos		:	POSITION;
	float2 texCoord	:	TEXCOORDS;
};

struct ps_input
{
	float4 pos		:	SV_POSITION;
	float2 texCoord	:	TEXCOORDS;
};

ps_input VS( vs_input input )
{
	ps_input output;

	output.pos = mul(float4(input.pos, 1.0f), WorldViewProjection);
	output.texCoord = input.texCoord;

	return output;
}

float4 PS( ps_input input ) : SV_TARGET
{
	float4 color = sceneTexture.Load(int3(input.pos.xy, 0));
	return color;
}