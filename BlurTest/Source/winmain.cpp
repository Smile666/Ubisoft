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

	g_pApp->InitWindow(hInstance, nShowCmd, SCREEN_WIDTH,
						SCREEN_HEIGHT, true);
	g_pApp->InitDirectX11(SCREEN_WIDTH, SCREEN_HEIGHT);
	g_pApp->VInitSimulation();

	int exitcode = g_pApp->Run();

	//clean memory
	delete g_pApp;

	return exitcode;
}