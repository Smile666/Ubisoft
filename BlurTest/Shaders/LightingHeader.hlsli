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

class cPhongLighting	:	cLighting
{
	float m_fSpecPower;

	float4 IlluminateSpecular(float3 vNormal, float3 vLightDir, float3 vViewDir)
	{
		float3 r = reflect(-vViewDir, vNormal);
		return float4(m_vLightColor, 1.0f) * pow(max(dot(vLightDir, r), 0.0f), m_fSpecPower);
	}
};

class cBlinnLighting	:	cLighting
{
	float m_fSpecPower;

	float4 IlluminateSpecular(float3 vNormal, float3 vLightDir, float3 vViewDir)
	{
		float3 h = (vLightDir + vViewDir) / length(vLightDir + vViewDir);
		return float4(m_vLightColor, 1.0f) * pow(max(dot(vNormal, h), 0.0f), m_fSpecPower);
	}
};

class cToonLighting	:	cLighting
{
	float4 IlluminateDiffuse(float3 vNormal, float3 vLightDir)
	{
		float4 diffuse;
		float c = 0.2f + max(dot(vNormal, vLightDir), 0.0f);

		if (c < 0.4f)
			diffuse = float4(0.3f, 0.3f, 0.3f, 1.0f);
		else
			if (c < 0.7f)
				diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
			else
				diffuse = float4(1.3f, 1.3f, 1.3f, 1.0f);

		return diffuse * float4(m_vLightColor, 1.0f);
	}
};

class cIsotropicWard	:	cLighting
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