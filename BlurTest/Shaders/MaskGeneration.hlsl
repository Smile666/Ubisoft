//////////////////////////////////////
//Shader resources and sampler states
//////////////////////////////////////
Texture2D		depthMap		:	register( t0 );
SamplerState	samplerState	:	register( s0 );

//////////////////////////////////////
//Constant buffers
//////////////////////////////////////
cbuffer ScreenBuffer : register( c0 )
{
	float	screen_width;
	float3	sbPad;
};

//////////////////////////////////////
//Define structures
//////////////////////////////////////
struct ps_input
{
	float4	pos	:	SV_POSITION;
	float2	texCoord	:	TEXCOORDS;
};

float4 MaskSimplePS( ps_input input ) : SV_TARGET
{
	if (input.pos.x > (screen_width / 2.0f) )
	{
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}

float4 MaskDepthPS( ps_input input ) : SV_TARGET
{
	if (input.pos.x < (screen_width / 2.0f))
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	else
	{
		//get depth data
		float depth = depthMap.Load(int3(input.pos.x, input.pos.y, 0.0f));
		if (depth < 1.0f)
			return float4(1.0f, 1.0f, 1.0f, 1.0f);
		else
			return float4(0.0f, 0.0f, 0.0f, 0.0f);
	} 
}