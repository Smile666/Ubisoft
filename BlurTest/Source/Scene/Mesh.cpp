#include "BlurTestStd.h"
#include "Mesh.h"

Mesh::Mesh()
{
	/*
	m_pPositionBuffer = NULL;
	m_pTexCoordBuffer = NULL;
	m_pNormalBuffer = NULL;
	*/

	m_ppBuffers = new ID3D11Buffer*[3];
	m_ppBuffers[0] = nullptr;
	m_ppBuffers[1] = nullptr;
	m_ppBuffers[2] = nullptr;

	m_pStride = nullptr;
	m_pOffset = nullptr;
}

Mesh::~Mesh()
{
	SAFE_DELETE_ARRAY(m_pOffset);
	SAFE_DELETE_ARRAY(m_pStride);

	SAFE_RELEASE(m_pNormalBuffer);
	SAFE_RELEASE(m_pTexCoordBuffer);
	SAFE_RELEASE(m_pPositionBuffer);
}

bool Mesh::VInitialize(ID3D11Device* pd3d11Device)
{
	m_pStride = new UINT[3];
	m_pStride[0] = 12;
	m_pStride[1] = 8;
	m_pStride[2] = 36;

	m_pOffset = new UINT[3];
	m_pOffset[0] = 0;
	m_pOffset[1] = 0;
	m_pOffset[2] = 0;

	/******	Initialize shader resource views ******/
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(pd3d11Device, L"Resources\\diffuse.jpg", NULL, NULL, &m_pDiffuseSRV, NULL);

	return true;
}

void Mesh::VPreRender(App* pApp, const real elapsedTime, const real totalTime)
{
	pApp->GetImmediateContext()->IASetVertexBuffers(0, 3, m_ppBuffers, m_pStride, m_pOffset);
	pApp->GetImmediateContext()->PSSetShaderResources(0, 1, &m_pDiffuseSRV);
}

void Mesh::VRender(ID3D11DeviceContext* pd3d11DevCon, const real elapsedTime, const real totalTime)
{
}

void Mesh::VPostRender(App* pApp, const real elapsedTime, const real totalTime)
{
}