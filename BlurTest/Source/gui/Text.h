//========================================================================
// Text.h
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#pragma once

#include "../Application/App.h"
#include "../Application/Interfaces.h"
#include "Font.h"

class Text : public IDrawable
{
public:
	Text() : m_bEnable(true) {}
	//explicit Text(const char * const text);
	~Text();

	void SetEnable(const bool isEnable);
	bool IsEnable() const;

	//initialize text
	void Initialize(ID3D11Device* pDevice, const char * const text, const float posx, const float posy, Font * pFont);

	/***** Rendering *****/
	virtual void VPreRender	(App* pApp, const float elapsedTime, const float totalTime);
	virtual void VRender	(App* pApp, const float elapsedTime, const float totalTime);
	virtual void VPostRender(App* pApp, const float elapsedTime, const float totalTime);

private:
	//
	// Vertex data
	//
	ID3D11Buffer*	m_pVertexBuffer;
	int				m_iNumVertices;

	//font
	Font*			m_pFont;

	//visibility flag
	bool			m_bEnable;
};