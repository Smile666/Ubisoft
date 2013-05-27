#include "BlurTestStd.h"
#include "Box.h"

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

void Box::VPreRender(App* pApp, const float elapsedTime, const float totalTime)
{
	Mesh::VPreRender(pApp, elapsedTime, totalTime);

	//set primitive topology
	pApp->GetImmediateContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Box::VRender(App* pApp, const float elapsedTime, const float totalTime)
{
	Mesh::VRender(pApp, elapsedTime, totalTime);

	pApp->GetImmediateContext()->Draw(36, 0);
}

void Box::VPostRender(App* pApp, const float elapsedTime, const float totalTime)
{
	Mesh::VPostRender(pApp, elapsedTime, totalTime);
}