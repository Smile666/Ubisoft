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
	float xClipSpace = input.pos.x;
	float screen_center = screen_width / 2.0f;

	//Smooth border a little bit
	if (xClipSpace > (screen_center + 10.0f) )
	{
		//full blur
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else if (xClipSpace < (screen_center - 10.0f))
	{
		//no blur
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		//interpolate
		float value = 1 - ( (screen_center + 10.0f - xClipSpace) / 20.0f );
		return float4(value, value, value, 1.0f);
	}
}

//only object mask generation
float4 MaskDepthPS( ps_input input ) : SV_TARGET
{
	float xClipSpace = input.pos.x;
	float screen_center = screen_width / 2.0f;

	//sample depth
	float depth = depthMap.Load(int3(xClipSpace, input.pos.y, 0.0f));

	//blur only if it's object's surface
	if (depth < 1.0f)
	{
		if (xClipSpace > (screen_center + 10.0f) )
		{
			return float4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else if (xClipSpace < (screen_center - 10.0f))
		{
			return float4(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			float value = 1 - ( (screen_center + 10.0f - xClipSpace) / 20.0f );
			return float4(value, value, value, 1.0f);
		}
	}
	else
	{
		//no blur
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}