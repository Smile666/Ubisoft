#pragma once

#include "../Application/App.h"
#include "Font.h"

class Text
{
public:
	Text();
	//explicit Text(const char * const text);
	~Text();

	bool Initialize(ID3D11Device* pDevice, const char * const text, const float posx, const float posy, Font * pFont);

	virtual void VPreRender(App* pApp, const float elapsedTime, const float totalTime);
	virtual void VRender(ID3D11DeviceContext* pd3d11DevCon, const float elapsedTime, const real totalTime);
	virtual void VPostRender(App* pApp, const float elapsedTime, const float totalTime);

private:
	ID3D11Buffer*	m_pVertexBuffer;
	int				m_iNumVertices;
	Font*			m_pFont;
};