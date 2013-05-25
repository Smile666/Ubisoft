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
	float3 viewDirTS	:	VIEWDIRTS;
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
	output.normalWS = normalize(output.normalWS);
	output.tangentWS = mul(input.tangent, (float3x3)World);
	output.tangentWS = normalize(output.tangentWS);
	output.binormalWS = mul(input.binormal, (float3x3)World);
	output.binormalWS = normalize(output.binormalWS);

	float3 posWS = mul(input.pos, (float3x3)World);

	output.lightDir = normalize(lightPos - posWS);
	output.viewDir = normalize(cameraPos - posWS);

	output.viewDirTS.x = dot(output.viewDir, output.tangentWS);
	output.viewDirTS.y = dot(output.viewDir, output.binormalWS);
	output.viewDirTS.z = dot(output.viewDir, output.normalWS);

	return output;
} 

iLighting	lighting;

cbuffer LightingBuffer	:	register( c0 )
{
	cLambertLighting			g_lambert;
	cLambertWrapAroundLighting	g_lambertWrapAround;
	cPhongLighting				g_phong;
	cBlinnLighting				g_blinn;
	cToonLighting				g_toon;
};

float4 PS( ps_input input ) : SV_TARGET
{
	//simple parallax mapping
	float2 texCoord;
	float height = heightMap.Sample(anisotropicSampler, input.texCoord).x;
	height = (height * 2.0f) - 1.0f;
	texCoord = input.texCoord + (height * input.viewDirTS.xy) * 0.08f;
	//texCoord = input.texCoord;

	float4 diffuse = diffuseTexture.Sample(anisotropicSampler, texCoord);
	float4 ambient = diffuse * 0.15f;
	float4 specular = float4(0.7f, 0.7f, 0.7f, 1.0f);

	//calculate normal
	float3 normalWS = normalize(input.normalWS);
	float3 tangentWS	= normalize(input.tangentWS);
	float3 binormalWS	= normalize(input.binormalWS);
	float3x3 TangentToWorldSpace = float3x3(	(float3)tangentWS,
												(float3)binormalWS,
												(float3)normalWS	);

	float3 normalTS = normalMap.Sample(anisotropicSampler, texCoord).rgb;
	normalWS = normalize(normalTS * 2.0f - 1.0f);
	normalWS = mul(normalWS, TangentToWorldSpace);

	float3 lightDir = normalize(input.lightDir);
	float3 viewDir = normalize(input.viewDir);

	float4 finalAmbient = ambient * lighting.IlluminateAmbient();
	float4 finalDiffuse = diffuse * lighting.IlluminateDiffuse(normalWS, lightDir);
	float4 finalSpecular = specular * lighting.IlluminateSpecular(normalWS, lightDir, viewDir);
	float4 finalColor = saturate(finalAmbient + finalDiffuse + finalSpecular);

	//return ambient;// * 0.1f;
	return finalColor;
}