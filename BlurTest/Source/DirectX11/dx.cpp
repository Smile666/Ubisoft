//========================================================================
// dx.cpp
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#include "BlurTestStd.h"
#include "dx.h"

bool CompileShader(const std::wstring & fileName, const std::string & entrypoint,
									ShaderType type, ID3D10Blob** pCompiledShader)
{
	//Check what type of shader we are compiling
	std::string target;
	switch (type)
	{
		case vertex_shader:
			target = "vs_5_0";
			break;
		case hull_shader:
			target = "hs_5_0";
			break;
		case domain_shader:
			target = "ds_5_0";
			break;
		case geometry_shader:
			target = "gs_5_0";
			break;
		case pixel_shader:
			target = "ps_5_0";
			break;
		case compute_shader:
			target = "cs_5_0";
			break;
	};

	//Compilation
	ID3D10Blob* pErrors;
	HRESULT hr = D3DX11CompileFromFile(fileName.c_str(), 0, 0, entrypoint.c_str(), target.c_str(), 0, 0, 0, 
		pCompiledShader, &pErrors, 0);

	//If any errors, draw them to debug output
	if (hr != S_OK)
	{
		OutputDebugStringA((char*)pErrors->GetBufferPointer());
		CheckHR(hr);
		return false;
	}

	SAFE_RELEASE(pErrors);

	return true;
}