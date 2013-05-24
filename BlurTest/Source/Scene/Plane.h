#pragma once

#include "Mesh.h"

class Plane : public Mesh
{
private:
	float	m_fWidth;
	float	m_fHeight;

	Plane(const float fWidth = 1.0f, const float fHeight = 1.0f);
	virtual bool VInitialize(ID3D11Device* pd3d11Device);
	
	virtual void VPreRender(App* pApp, const real elapsedTime, const real totalTime);
	virtual void VPostRender(App* pApp, const real elapsedTime, const real totalTime);
	virtual void VRender(ID3D11DeviceContext* pd3d11DevCon, const real elapsedTime, const real totalTime);
};