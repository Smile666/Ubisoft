#pragma once

#include "App.h"

class IDrawable
{
public:
	virtual void VPreRender	(App* pApp, const float elapsedTime, const float totalTime) = 0;
	virtual void VRender	(App* pApp, const float elapsedTime, const float totalTime) = 0;
	virtual void VPostRender(App* pApp, const float elapsedTime, const float totalTime) = 0;
};