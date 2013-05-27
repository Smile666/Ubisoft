#include "LightingHeader.hlsli"

/////////////////////////////////////////////
//Constant Buffers
/////////////////////////////////////////////
cbuffer MatrixBuffer	:	register( c0 )
{
	matrix World;
	matrix WorldView;
	matrix WorldViewProjection;
};

cbuffer LightBuffer		:	register( c1 )
{
	float3	lightPos	:	packoffset(c0.x);
	float	pad1		:	packoffset(c0.w);
	float3	cameraPos	:	packoffset(c1.x);
	float	pad2		:	packoffset(c1.w);
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
	float4 pos			:	SV_POSITION;
	float2 texCoord		:	TEXCOORDS;
	float3 normalWS		:	NORMALWS;
	float3 tangentWS	:	TANGENTWS;
	float3 binormalWS	:	BINORMALWS;
	float3 lightDir		:	LIGHTDIR;
	float3 viewDir		:	VIEWDIR;
	float3 viewDirTS	:	VIEWDIRTS;
};

ps_input VS( vs_input input )
{
	ps_input output;

	//transform position to clip space
	output.pos = mul(float4(input.pos, 1.0f), WorldViewProjection);

	//send texture coordinates
	output.texCoord = input.texCoord;

	//calculate normal
	output.normalWS = mul(input.normal, (float3x3)World);
	output.normalWS = normalize(output.normalWS);

	//calculate tangent
	output.tangentWS = mul(input.tangent, (float3x3)World);
	output.tangentWS = normalize(output.tangentWS);

	//calculate birnormal
	output.binormalWS = mul(input.binormal, (float3x3)World);
	output.binormalWS = normalize(output.binormalWS);

	//calculate direction to light source and viewer
	float3 posWS = mul(input.pos, (float3x3)World);
	output.lightDir = normalize(lightPos - posWS);
	output.viewDir = normalize(cameraPos - posWS);

	//transform direction to viewer to tangent space
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
	cIsotropicWard				g_isotropicWard;
};

float4 PS( ps_input input ) : SV_TARGET
{
	//move texture coordinate
	float2 texCoord;
	float height = heightMap.Sample(anisotropicSampler, input.texCoord).x;
	height = (height * 2.0f) - 1.0f;
	texCoord = input.texCoord + (height * input.viewDirTS.xy) * 0.08f;
	//texCoord = input.texCoord;

	//sample diffuse map
	float4 diffuse = diffuseTexture.Sample(anisotropicSampler, texCoord);

	//default ambient
	float4 ambient = diffuse;

	//sample specular
	float4 specular = specularMap.Sample(anisotropicSampler, texCoord);

	/////////////////////////
	//calculate normal
	/////////////////////////

	//normalize as it may not have 1 length after rasterizer stage
	float3 normalWS		= normalize(input.normalWS); 
	float3 tangentWS	= normalize(input.tangentWS);
	float3 binormalWS	= normalize(input.binormalWS);

	//transformation matrix (tangent space -> world space
	float3x3 TangentToWorldSpace = float3x3(	(float3)tangentWS,
												(float3)binormalWS,
												(float3)normalWS	);

	//sample normal from map in tangent space
	float3 normalTS = normalMap.Sample(anisotropicSampler, texCoord).rgb;

	//go to range [-1, 1]
	normalWS = normalize(normalTS * 2.0f - 1.0f);

	//transform to world space
	normalWS = mul(normalWS, TangentToWorldSpace);

	//renormalize
	float3 lightDir = normalize(input.lightDir);
	float3 viewDir = normalize(input.viewDir);

	
	float4 finalAmbient = ambient * lighting.IlluminateAmbient(); //calculate ambient
	float4 finalDiffuse = diffuse * lighting.IlluminateDiffuse(normalWS, lightDir); //diffuse
	float4 finalSpecular = specular * lighting.IlluminateSpecular(normalWS, lightDir, viewDir); //specular

	//get final pixel color
	float4 finalColor = saturate(finalAmbient + finalDiffuse + finalSpecular);
	return finalColor;
}