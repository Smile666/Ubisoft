/////////////////////////////////////////////
//Constant Buffers
/////////////////////////////////////////////
cbuffer MatrixBuffer
{
	matrix World;
	matrix WorldView;
	matrix WorldViewProjection;
};

////////////////////////////////////////////
//Textures and Sampler States
////////////////////////////////////////////
Texture2D		diffuseTexture		:	register( t0 );
Texture2D		normalMap			:	register( t1 );
Texture2D		specularMap			:	register( t2 );
Texture2D		heightMap			:	register( t3 );
SamplerState	anisotropicSampler	:	register( s0 );

/////////////////////////////////////////////
//Structures
/////////////////////////////////////////////
struct vs_input
{
	float3 pos		:	POSITION;
	float2 texCoord	:	TEXCOORDS;
	float3 normal	:	NORMAL;
	float3 tangent	:	TANGENT;
	float3 binormal	:	BINORMAL;
};

struct ps_input
{
	float4 pos	:	SV_POSITION;
	float2 texCoord	:	TEXCOORDS;
	float3 normalWS	:	NORMALWS;
	float3 tangentWS	:	TANGENTWS;
	float3 binormalWS	:	BINORMALWS;
};

ps_input VS( vs_input input )
{
	ps_input output;

	output.pos = mul(float4(input.pos, 1.0f), WorldViewProjection);
	output.texCoord = input.texCoord;
	output.normalWS = mul(input.normal, (float3x3)World);
	output.tangentWS = mul(input.tangent, (float3x3)World);
	output.binormalWS = mul(input.binormal, (float3x3)World);

	return output;
}

float4 PS( ps_input input ) : SV_TARGET
{
	float4 diffuse = diffuseTexture.Sample(anisotropicSampler, input.texCoord);

	return diffuse;
}