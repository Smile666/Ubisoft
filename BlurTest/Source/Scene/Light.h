#pragma once

#include "../Application/App.h"

class Light
{
protected:
	XMVECTOR	m_vPos;
	XMVECTOR	m_vColor;

public:
	virtual void VApply(App* pApp);
};