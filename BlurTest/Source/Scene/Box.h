#pragma once

#include "Mesh.h"

class Box : public Mesh
{
public:
	Box();
	virtual bool VInitialize(ID3D11Device* pd3d11Device);
	
	virtual void VPreRender(App* pApp, const float elapsedTime, const float totalTime);
	virtual void VPostRender(App* pApp, const float elapsedTime, const float totalTime);
	virtual void VRender(App* pApp, const float elapsedTime, const float totalTime);
};