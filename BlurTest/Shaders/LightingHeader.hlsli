//-------------------------------------------
//Lighting interface
//-------------------------------------------
interface iLighting
{
	float4 IlluminateAmbient();
	float4 IlluminateDiffuse(float3 vNormal, float3 vLightDir);
	float4 IlluminateSpecular(float3 vNormal, float3 vLightDir, float3 vViewDir);
};

class cLighting :	iLighting
{
	float3 m_vLightColor;

	float4 IlluminateAmbient()
	{
		return float4(0.15f, 0.15f, 0.15f, 1.0f);
	}

	float4 IlluminateDiffuse(float3 vNormal, float3 vLightDir)
	{
		return float4(m_vLightColor, 1.0f);
	}

	float4 IlluminateSpecular(float3 vNormal, float3 vLightDir, float3 vViewDir)
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
};

//-------------------------------------------
//Lighting Implementation
//-------------------------------------------
class cLambertLighting	:	cLighting
{
	float4 IlluminateDiffuse(float3 vNormal, float3 vLightDir)
	{
		return max( 0.0f, dot(vNormal, vLightDir)) * float4(m_vLightColor, 1.0f);
	}
};

class cLambertWrapAroundLighting	:	cLighting
{
	float m_fFactor;
	float4 IlluminateDiffuse(float3 vNormal, float3 vLightDir)
	{
		return float4(m_vLightColor, 1.0f) * max( 0.0f, dot(vNormal, vLightDir) + m_fFactor) / (1.0f + m_fFactor);
	}
};

//-------------------------------------------
//Next models inherit Lambert as they just
//add specular term to the equation
//-------------------------------------------
class cPhongLighting	:	cLambertLighting
{
	float m_fSpecPower;

	float4 IlluminateSpecular(float3 vNormal, float3 vLightDir, float3 vViewDir)
	{
		float3 r = reflect(-vViewDir, vNormal);
		return float4(m_vLightColor, 1.0f) * pow(max(dot(vLightDir, r), 0.0f), m_fSpecPower);
	}
};

class cBlinnLighting	:	cLambertLighting
{
	float m_fSpecPower;

	float4 IlluminateSpecular(float3 vNormal, float3 vLightDir, float3 vViewDir)
	{
		float3 h = (vLightDir + vViewDir) / length(vLightDir + vViewDir);
		return float4(m_vLightColor, 1.0f) * pow(max(dot(vNormal, h), 0.0f), m_fSpecPower);
	}
};

class cIsotropicWard	:	cLambertLighting
{
	float m_fRoughness;

	float4 IlluminateSpecular(float3 vNormal, float3 vLightDir, float3 vViewDir)
	{
		float3 h = (vLightDir + vViewDir) / length(vLightDir + vViewDir);
		float hDotn = dot(h, vNormal);
		float hDotn2 = hDotn*hDotn;

		return float4(m_vLightColor, 1.0f) * exp( - m_fRoughness * (1.0f - hDotn2) / hDotn2);
	}
};