//========================================================================
// BlurTestStd.h
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#pragma once 

// *******************************
//	Includes
// *******************************
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3d10.h>
#include <D3D11Shader.h>
#include <D3Dcompiler.h>

#include <vector>
#include <list>
#include <map>

#include <assert.h>
#include <fstream>

// *******************************
//	Defines
// *******************************
//#define float float
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

//switch off SIMD math
#define _XM_NO_INTRINSICS_

#include <xnamath.h>

#define Key	char

// *******************************
//	Memory
// *******************************
#if !defined(SAFE_DELETE)
	#define SAFE_DELETE(x) if(x) delete x; x=NULL;
#endif

#if !defined(SAFE_DELETE_ARRAY)
	#define SAFE_DELETE_ARRAY(x) if (x) delete [] x; x=NULL; 
#endif

#if !defined(SAFE_RELEASE)
	#define SAFE_RELEASE(x) if(x) x->Release(); x=NULL;
#endif

// *******************************
//	Debugging
// *******************************
#define DebugBrake() __asm { int 3 }

//output notes copied directly from msdn
#ifdef _DEBUG
	#define CheckHR(x) \
		switch (x) \
		{ \
			case S_OK: \
				break; \
			\
			case S_FALSE: \
				OutputDebugStringA("Completed without error, but only partial results were obtained. \
							If a buffer is not large enough to hold the information that is returned \
							to it, the returned information is often truncated to fit into the buffer \
							and S_FALSE is returned from the method."); \
				break; \
			\
			case E_FAIL: \
				OutputDebugStringA("Operation couldn't be performed.");\
				break; \
			\
			case E_INVALIDARG: \
				OutputDebugStringA("One of the arguments passed in was invalid.");\
				break; \
			\
			case E_NOINTERFACE: \
				OutputDebugStringA("The object searched for was not found.");\
				break; \
			\
			case E_OUTOFMEMORY: \
				OutputDebugStringA("A memory allocation attempt failed.");\
				break; \
			\
			case E_UNEXPECTED: \
				OutputDebugStringA("The target was not accessible, ot the engine was not in a state \
							where the function or method could be processed.");\
				break; \
			\
			case E_NOTIMPL: \
				OutputDebugStringA("Not implemented."); \
				break; \
			\
			default: \
				OutputDebugStringA("File not found."); \
				break; \
		};\
		DebugBrake(); 
#else

	#define CheckHR(x) {}

#endif

#define VALID(hr) \
	if (hr == S_OK) \
	{ \
	} \
	else \
	{ \
		CheckHR(hr); \
		return false; \
	};