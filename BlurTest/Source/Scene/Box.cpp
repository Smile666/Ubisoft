#include "BlurTestStd.h"
#include "Box.h"

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

Box::Box() : Mesh()
{
	m_iNumVertices = 36;
}

bool Box::VInitialize(ID3D11Device* pDevice)
{
	Mesh::VInitialize(pDevice);

	HRESULT hr;

	/////////////////////////////////////////////
	//Vertex Buffer Description 
	/////////////////////////////////////////////
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	//////////////////////////////////////////////
	//Subresource data
	//////////////////////////////////////////////
	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));

	//*****************************************
	//
	//	Positions Buffer
	//
	//*****************************************

	XMFLOAT3 pos [] = 
	{
		XMFLOAT3(-1.0f, 1.0f, -1.0f),
		XMFLOAT3(1.0f, 1.0f, -1.0f),
		XMFLOAT3(-1.0f, -1.0f, -1.0f),

		XMFLOAT3(-1.0f, -1.0f, -1.0f),
		XMFLOAT3(1.0f, 1.0f, -1.0f),
		XMFLOAT3(1.0f, -1.0f, -1.0f),
		
		XMFLOAT3(1.0f, 1.0f, -1.0f),
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(1.0f, -1.0f, -1.0f),
		
		XMFLOAT3(1.0f, -1.0f, -1.0f),
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(1.0f, -1.0f, 1.0f),
		
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(-1.0f, 1.0f, 1.0f),
		XMFLOAT3(1.0f, -1.0f, 1.0f),
		
		XMFLOAT3(1.0f, -1.0f, 1.0f),
		XMFLOAT3(-1.0f, 1.0f, 1.0f),
		XMFLOAT3(-1.0f, -1.0f, 1.0f),
		
		XMFLOAT3(-1.0f, 1.0f, 1.0f),
		XMFLOAT3(-1.0f, 1.0f, -1.0f),
		XMFLOAT3(-1.0f, -1.0f, 1.0f),
		
		XMFLOAT3(-1.0f, -1.0f, 1.0f),
		XMFLOAT3(-1.0f, 1.0f, -1.0f),
		XMFLOAT3(-1.0f, -1.0f, -1.0f),
		
		XMFLOAT3(-1.0f, 1.0f, 1.0f),
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(-1.0f, 1.0f, -1.0f),
		
		XMFLOAT3(-1.0f, 1.0f, -1.0f),
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(1.0f, 1.0f, -1.0f),
		
		XMFLOAT3(-1.0f, -1.0f, -1.0f),
		XMFLOAT3(1.0f, -1.0f, -1.0f),
		XMFLOAT3(-1.0f, -1.0f, 1.0f),
		
		XMFLOAT3(-1.0f, -1.0f, 1.0f),
		XMFLOAT3(1.0f, -1.0f, -1.0f),
		XMFLOAT3(1.0f, -1.0f, 1.0f),
	};

	bufferDesc.ByteWidth = 12 * 36;
	bufferData.pSysMem = pos;

	//create positions buffer
	hr = pDevice->CreateBuffer(&bufferDesc, &bufferData, &m_ppBuffers[0]);

	//???!!!delete [] pos;

	//*****************************************
	//
	//	Texture Coords Buffer
	//
	//*****************************************

	bufferDesc.ByteWidth = 8 * 36;

	XMFLOAT2 texCoords[] =
	{
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(0.0f, 1.0f),

		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),
		
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(0.0f, 1.0f),
		
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),
		
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(0.0f, 1.0f),
		
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),
		
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(0.0f, 1.0f),
		
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),
		
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(0.0f, 1.0f),
		
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),
		
		XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(0.0f, 1.0f),
		
		XMFLOAT2(0.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 1.0f),
	};

	bufferData.pSysMem = texCoords;
	hr = pDevice->CreateBuffer(&bufferDesc, &bufferData, &m_ppBuffers[1]);

	//???!!!delete [] texCoords;

	//*****************************************
	//
	//	Normals Buffer
	//
	//*****************************************

	bufferDesc.ByteWidth = 36 * 36;

	NormalData normals[36];

	//generate normals data for each face
	for (int i = 0; i < 12; i++)
	{
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;

		CreateNormalTangentBinormal(&pos[i*3], &texCoords[i*3], normal, tangent, binormal);

		//fill the data for vertex buffer
		normals[i*3+2].m_normal = normal;
		normals[i*3+2].m_tangent = tangent;
		normals[i*3+2].m_binormal = binormal;

		normals[i*3+1].m_normal = normal;
		normals[i*3+1].m_tangent = tangent;
		normals[i*3+1].m_binormal = binormal;

		normals[i*3].m_normal = normal;
		normals[i*3].m_tangent = tangent;
		normals[i*3].m_binormal = binormal;
	}

	bufferData.pSysMem = normals;
	
	hr = pDevice->CreateBuffer(&bufferDesc, &bufferData, &m_ppBuffers[2]);

	//???!!!delete [] normals;

	return true;
}

void Box::VPreRender(App* pApp, const real elapsedTime, const real totalTime)
{
	Mesh::VPreRender(pApp, elapsedTime, totalTime);

	//set primitive topology
	pApp->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//fill and bind constant buffer
	pApp->m_matrixData.World = XMMatrixTranspose(XMMatrixIdentity());
	pApp->m_matrixData.WorldView = XMMatrixTranspose(XMMatrixIdentity() * pApp->m_matrixView);
	pApp->m_matrixData.WorldViewProjection = XMMatrixTranspose(XMMatrixIdentity() * pApp->m_matrixView * pApp->m_matrixProj);
	pApp->GetImmediateContext()->UpdateSubresource(pApp->m_pcbMatrix, 0, nullptr, &pApp->m_matrixData, 0, 0);
	pApp->GetImmediateContext()->VSSetConstantBuffers(0, 1, &pApp->m_pcbMatrix);
}

void Box::VRender(ID3D11DeviceContext* pd3d11DevCon, const real elapsedTime, const real totalTime)
{
	Mesh::VRender(pd3d11DevCon, elapsedTime, totalTime);

	pd3d11DevCon->Draw(36, 0);
}

void Box::VPostRender(App* pApp, const real elapsedTime, const real totalTime)
{
	Mesh::VPostRender(pApp, elapsedTime, totalTime);
}