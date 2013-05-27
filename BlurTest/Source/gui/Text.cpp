//========================================================================
// Text.cpp
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#include "BlurTestStd.h"
#include "Text.h"

ID3D11ShaderResourceView* pNullTextSRV;

Text::~Text()
{
	SAFE_RELEASE(m_pVertexBuffer);
}

void Text::Initialize(ID3D11Device* pDevice, const char * const text, const float posx, const float posy, Font * pFont)
{
	m_pFont = pFont;
	if (!m_pFont->BuildText(pDevice, &m_pVertexBuffer, m_iNumVertices, text, posx, posy))
	{
		assert(0 && "Error building text vertex data!");
	}
}

void Text::VPreRender(App* pApp, const float elapsedTime, const float totalTime)
{
	//bind vertex and textures data to the pipeline
	UINT stride = sizeof(XMFLOAT3) + sizeof(XMFLOAT2);
	UINT offset = 0;
	ID3D11ShaderResourceView* pShaderResource = m_pFont->GetShaderResource();
	pApp->GetImmediateContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pApp->GetImmediateContext()->PSSetShaderResources(0, 1, &pShaderResource);

	//fill constant buffer
	App::MatrixTexBuffer buffer;
	buffer.WorldViewProjection = XMMatrixTranspose(XMMatrixIdentity() * pApp->GetView() * pApp->GetOrtho());
	pApp->GetImmediateContext()->UpdateSubresource(pApp->GetMatrixTexConstantBuffer(), 0, nullptr, &buffer, 0, 0);
	
	//bind constant buffer
	ID3D11Buffer* pBuffer = pApp->GetMatrixTexConstantBuffer();
	pApp->GetImmediateContext()->VSSetConstantBuffers(0, 1, &pBuffer);
}
void Text::VRender(App* pApp, const float elapsedTime, const float totalTime)
{
	pApp->GetImmediateContext()->Draw(m_iNumVertices, 0);
}

void Text::VPostRender(App* pApp, const float elapsedTime, const float totalTime)
{
	pApp->GetImmediateContext()->PSSetShaderResources(0, 1, &pNullTextSRV);
}

void Text::SetEnable(const bool isEnable)
{ 
	m_bEnable = isEnable; 
}

bool Text::IsEnable() const 
{
	return m_bEnable;
}