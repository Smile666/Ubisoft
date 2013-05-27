#pragma once

#include "../Application/App.h"
#include "../Application/Interfaces.h"
#include "Font.h"

class Text : public IDrawable
{
public:
	Text() {}
	//explicit Text(const char * const text);
	~Text();

	inline void SetEnable(const bool isEnable) { m_bEnable = isEnable; }
	inline bool IsEnable() const { return m_bEnable; }

	bool Initialize(ID3D11Device* pDevice, const char * const text, const float posx, const float posy, Font * pFont);

	virtual void VPreRender	(App* pApp, const float elapsedTime, const float totalTime);
	virtual void VRender	(App* pApp, const float elapsedTime, const float totalTime);
	virtual void VPostRender(App* pApp, const float elapsedTime, const float totalTime);

private:
	ID3D11Buffer*	m_pVertexBuffer;
	int				m_iNumVertices;
	Font*			m_pFont;

	bool			m_bEnable;
};