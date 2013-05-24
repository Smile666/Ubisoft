#include "BlurTestStd.h"
#include "Mesh.h"

void CreateNormalTangentBinormal(XMFLOAT3 * positions, XMFLOAT2 * texCoords,
		XMFLOAT3 & normal, XMFLOAT3 & tangent, XMFLOAT3 & binormal)
{
	XMFLOAT3 posVector1, posVector2;
	XMFLOAT2 tuVector, tvVector;
	float r32Den;
	float r32Length;

	//calculate vectors from point to point
	posVector1.x = positions[1].x - positions[0].x;
	posVector1.y = positions[1].y - positions[0].y;
	posVector1.z = positions[1].z - positions[0].z;

	posVector2.x = positions[2].x - positions[0].x;
	posVector2.y = positions[2].y - positions[0].y;
	posVector2.z = positions[2].z - positions[0].z;

	//calculate tex coords vectors
	tuVector.x = texCoords[1].x - texCoords[0].x;
	tuVector.y = texCoords[2].x - texCoords[0].x;

	tvVector.x = texCoords[1].y - texCoords[0].y;
	tvVector.y = texCoords[2].y - texCoords[0].y;

	//calculate inverse determinant
	r32Den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);

	//calculate tangent vector
	tangent.x = (tvVector.y * posVector1.x - tvVector.x * posVector2.x) * r32Den;
	tangent.y = (tvVector.y * posVector1.y - tvVector.x * posVector2.y) * r32Den;
	tangent.z = (tvVector.y * posVector1.z - tvVector.x * posVector2.z) * r32Den;

	r32Length = sqrtf((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	tangent.x = tangent.x / r32Length;
	tangent.y = tangent.y / r32Length;
	tangent.z = tangent.z / r32Length;

	//calculate binormal vector
	binormal.x = (tuVector.x * posVector2.x - tuVector.y * posVector1.x) * r32Den;
	binormal.y = (tuVector.x * posVector2.y - tuVector.y * posVector1.y) * r32Den;
	binormal.z = (tuVector.x * posVector2.z - tuVector.y * posVector1.z) * r32Den;

	r32Length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	binormal.x = binormal.x / r32Length;
	binormal.y = binormal.y / r32Length;
	binormal.z = binormal.z / r32Length;

	//calculate normal vector
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	r32Length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	normal.x = normal.x / r32Length;
	normal.y = normal.y / r32Length;
	normal.z = normal.z / r32Length;
}

Mesh::Mesh() : m_fPitch(0), m_fYaw(0), m_fRoll(0)
{
	//set initial position
	m_vPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

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

	SAFE_RELEASE(m_ppBuffers[2]);
	SAFE_RELEASE(m_ppBuffers[1]);
	SAFE_RELEASE(m_ppBuffers[0]);
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

void Mesh::VUpdate(App* pApp, const real elapsedTime, const real totalTime)
{
	m_fPitch += 0.001f;
	m_fYaw += 0.001f;
	m_fRoll += 0.001f;
}

void Mesh::VPreRender(App* pApp, const real elapsedTime, const real totalTime)
{
	pApp->GetImmediateContext()->IASetVertexBuffers(0, 3, m_ppBuffers, m_pStride, m_pOffset);
	pApp->GetImmediateContext()->PSSetShaderResources(0, 1, &m_pDiffuseSRV);

	//fill and bind constant buffer
	pApp->m_matrixData.World = XMMatrixTranspose(XMMatrixTranslationFromVector(m_vPos) * XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, m_fRoll));
	pApp->m_matrixData.WorldView = XMMatrixTranspose(XMMatrixTranslationFromVector(m_vPos) * XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, m_fRoll) * pApp->m_matrixView);
	pApp->m_matrixData.WorldViewProjection = XMMatrixTranspose(XMMatrixTranslationFromVector(m_vPos) * XMMatrixRotationRollPitchYaw(m_fPitch, m_fYaw, m_fRoll) * pApp->m_matrixView * pApp->m_matrixProj);
	pApp->GetImmediateContext()->UpdateSubresource(pApp->m_pcbMatrix, 0, nullptr, &pApp->m_matrixData, 0, 0);
	pApp->GetImmediateContext()->VSSetConstantBuffers(0, 1, &pApp->m_pcbMatrix);
}

void Mesh::VRender(ID3D11DeviceContext* pd3d11DevCon, const real elapsedTime, const real totalTime)
{
}

void Mesh::VPostRender(App* pApp, const real elapsedTime, const real totalTime)
{
}