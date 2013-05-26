#include "BlurTestStd.h"
#include "Text.h"

bool Text::Initialize(ID3D11Device* pDevice, const char * const text, const float posx, const float posy, Font * pFont)
{
	m_pFont = pFont;
	m_pFont->BuildText(pDevice, &m_pVertexBuffer, m_iNumVertices, text, posx, posy);
	return true;
}

void Text::VPreRender(App* pApp, const float elapsedTime, const float totalTime)
{
	UINT stride = sizeof(XMFLOAT3) + sizeof(XMFLOAT2);
	UINT offset = 0;
	ID3D11ShaderResourceView* pShaderResource = m_pFont->GetShaderResource();
	pApp->GetImmediateContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pApp->GetImmediateContext()->PSSetShaderResources(0, 1, &pShaderResource);

	//fill and bind constant buffer
	pApp->m_matrixData.World = XMMatrixTranspose(XMMatrixIdentity());
	pApp->m_matrixData.WorldView = XMMatrixTranspose(XMMatrixIdentity() * pApp->m_matrixView);
	pApp->m_matrixData.WorldViewProjection = XMMatrixTranspose(XMMatrixIdentity() * pApp->m_matrixView * pApp->m_matrixOrtho);
	pApp->GetImmediateContext()->UpdateSubresource(pApp->m_pcbMatrix, 0, nullptr, &pApp->m_matrixData, 0, 0);
	pApp->GetImmediateContext()->VSSetConstantBuffers(0, 1, &pApp->m_pcbMatrix);
}
void Text::VRender(ID3D11DeviceContext* pd3d11DevCon, const float elapsedTime, const real totalTime)
{
	pd3d11DevCon->Draw(m_iNumVertices, 0);
}

void Text::VPostRender(App* pApp, const float elapsedTime, const float totalTime)
{
}