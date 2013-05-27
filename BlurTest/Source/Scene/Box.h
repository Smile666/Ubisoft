//========================================================================
// Box.h
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#pragma once

#include "Mesh.h"

class Box : public Mesh
{
public:
	Box();
	virtual bool VInitialize(ID3D11Device* pd3d11Device);
	
	virtual void VPreRender(App* pApp, const float elapsedTime, const float totalTime);
};