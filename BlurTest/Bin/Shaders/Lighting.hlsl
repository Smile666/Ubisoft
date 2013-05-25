#include "LightingHeader.hlsli"

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
	float3 lightDir	:	LIGHTDIR;
	float3 viewDir	:	VIEWDIR;
};

ps_input VS( vs_input input )
{
	//float3 lightPos = float3(-3.0f, 1.0f, 0.0f);
	float3 lightPos = float3(0.0f, 0.0f, -3.0f);
	float3 cameraPos = float3(0.0f, 0.0f, -5.0f);

	ps_input output;

	output.pos = mul(float4(input.pos, 1.0f), WorldViewProjection);
	output.texCoord = input.texCoord;
	output.normalWS = mul(input.normal, (float3x3)World);
	output.tangentWS = mul(input.tangent, (float3x3)World);
	output.binormalWS = mul(input.binormal, (float3x3)World);

	float3 posWS = mul(input.pos, (float3x3)World);

	output.lightDir = normalize(lightPos - posWS);
	output.viewDir = normalize(cameraPos - posWS);

	return output;
} 

iLighting	lighting;

cbuffer LightingBuffer	:	register( c0 )
{
	cLambertLighting			g_lambert;
	cLambertWrapAroundLighting	g_lambertWrapAround;
	//cPhongLighting		g_phong;
};

float4 PS( ps_input input ) : SV_TARGET
{
	float4 diffuse = diffuseTexture.Sample(anisotropicSampler, input.texCoord);
	float4 ambient = diffuse * 0.15f;

	float3 lightDir = normalize(input.lightDir);
	float3 viewDir = normalize(input.viewDir);

	float4 finalColor = ambient + diffuse * lighting.Illuminate(input.normalWS, lightDir);

	//return ambient;// * 0.1f;
	return finalColor;
}