//-------------------------------------------
//Lighting interface
//-------------------------------------------
interface iLighting
{
	float4 Illuminate(float3 vNormal, float3 vLightDir);
};

class cLighting :	iLighting
{
	float3 m_vLightColor;
	float4 Illuminate(float3 vNormal, float3 vLightDir)
	{
		return float4(m_vLightColor, 1.0f);
	}
};

//-------------------------------------------
//Lighting Implementation
//-------------------------------------------
class cLambertLighting	:	cLighting
{
	float4 Illuminate(float3 vNormal, float3 vLightDir)
	{
		return max( 0.0f, dot(vNormal, vLightDir)) * float4(m_vLightColor, 1.0f);
	}
};

class cLambertWrapAroundLighting	:	cLighting
{
	float m_fFactor;
	float4 Illuminate(float3 vNormal, float3 vLightDir)
	{
		return float4(m_vLightColor, 1.0f) * max( 0.0f, dot(vNormal, vLightDir) + m_fFactor) / (1.0f + m_fFactor);
		//return max( 0.0f, dot(vNormal, vLightDir)) * float4(m_vLightColor, 1.0f);
		//return float4(m_vLightColor, 1.0f);
	}
};

//class cPhongLighting	:	cLighting
//{
//	float specPower;

//	float3 Illuminate(float3 vNormal, float3 vLightDir);
//};