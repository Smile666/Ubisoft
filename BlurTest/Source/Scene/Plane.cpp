#include "BlurTestStd.h"
#include "Plane.h"

Plane::Plane(const float fWidth, const float fHeight) : Mesh()
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_iNumVertices = 6;
}

bool Plane::VInitialize(ID3D11Device* pDevice)
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

	float right = m_fWidth / 2.0f;
	float left = -right;
	float top = m_fHeight / 2.0f;
	float bottom = -top;

	XMFLOAT3 pos [] = 
	{
		XMFLOAT3(left,	top,	0.0f), //left top
		XMFLOAT3(right,	top,	0.0f), //right top
		XMFLOAT3(left,	bottom, 0.0f), //left bottom
		XMFLOAT3(left,	bottom, 0.0f), //left bottom
		XMFLOAT3(right,	top,	0.0f), //right top
		XMFLOAT3(right,	bottom, 0.0f), //right bottom
	};

	bufferDesc.ByteWidth = 12 * 6;
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
	};

	bufferData.pSysMem = texCoords;
	hr = pDevice->CreateBuffer(&bufferDesc, &bufferData, &m_ppBuffers[1]);

	//???!!!delete [] texCoords;

	//*****************************************
	//
	//	Normals Buffer
	//
	//*****************************************

	bufferDesc.ByteWidth = 6 * 36;

	NormalData normals[6];

	//generate normals data for each face
	for (int i = 0; i < 2; i++)
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

void Plane::VPreRender(App* pApp, const real elapsedTime, const real totalTime)
{
	Mesh::VPreRender(pApp, elapsedTime, totalTime);

	//set primitive topology
	pApp->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Plane::VRender(ID3D11DeviceContext* pd3d11DevCon, const real elapsedTime, const real totalTime)
{
	Mesh::VRender(pd3d11DevCon, elapsedTime, totalTime);

	pd3d11DevCon->Draw(6, 0);
}

void Plane::VPostRender(App* pApp, const real elapsedTime, const real totalTime)
{
	Mesh::VPostRender(pApp, elapsedTime, totalTime);
}