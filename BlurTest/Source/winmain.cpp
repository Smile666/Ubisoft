//========================================================================
// winmain.cpp
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#include "BlurTestStd.h"
#include "Application\BlurApp.h"

#define _XM_NO_INTRINSICS_

App*	g_pApp = NULL;

int WINAPI wWinMain(	HINSTANCE	hInstance,
						HINSTANCE	hPrevInstance,
						LPWSTR		lpCmdLine,
						int			nShowCmd )
{
	g_pApp = new BlurApp();

	if (!g_pApp->InitWindow(L"Blur Test", hInstance, nShowCmd, SCREEN_WIDTH,
						SCREEN_HEIGHT, true))
	{
		assert(0 && "Error initializing window occured!");
	}

	if (!g_pApp->InitDirectX11(SCREEN_WIDTH, SCREEN_HEIGHT))
	{
		assert(0 && "Error initializing DirectX occured!");
	}

	if (!g_pApp->VInitSimulation())
	{
		assert(0 && "Error intializing initial simulation state occured!");
	}

	int exitcode = g_pApp->Run();

	//clean memory
	delete g_pApp;

	return exitcode;
}