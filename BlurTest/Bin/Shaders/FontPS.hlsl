////////////////////////////////////////
//Textures and sampler states
////////////////////////////////////////
Texture2D	fontTexture	:	register( t0 );
SamplerState linearSampler	:	register( s0 );

////////////////////////////////////////
//Structures
////////////////////////////////////////
struct ps_input
{
	float4 pos	:	SV_POSITION;
	float2 texCoord	:	TEXCOORDS;
};

///////////////////////////////////////
//Font Pixel Shader

float4 FontPS(ps_input input) : SV_TARGET
{
	float4 finalColor;

	//get color from font texture
	finalColor = fontTexture.Sample(linearSampler, input.texCoord);

	//if color is not white => pixel is transparent
	if (finalColor.r == 0)
	{
		finalColor.a = 0.0f;
	}
	else
	{
		finalColor.a = 1.0f;
	}

	return finalColor;
}