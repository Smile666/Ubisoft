//========================================================================
// dx.h
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#pragma once

enum ShaderType
{
	vertex_shader,
	hull_shader,
	domain_shader,
	geometry_shader,
	pixel_shader,
	compute_shader,
};
bool CompileShader(const std::wstring & fileName, const std::string & entrypoint,
									ShaderType type, ID3D10Blob** ppCompiledShader);