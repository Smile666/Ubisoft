#pragma once

#include "App.h"

class IDrawable
{
	virtual void VPreRender(App* pApp, const float elapsedTime, const float totalTime);
	virtual void VRender(ID3D11DeviceContext* pd3d11DevCon, const float elapsedTime, const float totalTime);
	virtual void VPostRender(App* pApp, const float elapsedTime, const float totalTime);
};