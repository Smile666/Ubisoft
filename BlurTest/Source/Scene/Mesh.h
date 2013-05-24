#pragma once

#include "../Application/App.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	virtual bool VInitialize(ID3D11Device* pd3d11Device);
	virtual void VUpdate(const real elapsedTime, const real totalTime) {}
	
	virtual void VPreRender(App* pApp, const real elapsedTime, const real totalTime);
	virtual void VRender(ID3D11DeviceContext* pd3d11DevCon, const real elapsedTime, const real totalTime);
	virtual void VPostRender(App* pApp, const real elapsedTime, const real totalTime);

protected:
	////////////////////////////////////////////////////
	//Separate all buffer components for effeciency
	//purposes: for example, when we turn off lightning
	//we don't need to send normal data to GPU
	//
	//[0] - positions buffer
	//[1] - texture coordinates buffer
	//[2] - normal data buffer
	//
	////////////////////////////////////////////////////
	ID3D11Buffer* m_pPositionBuffer;
	ID3D11Buffer* m_pTexCoordBuffer;
	ID3D11Buffer* m_pNormalBuffer;
	ID3D11Buffer** m_ppBuffers;

	UINT* m_pStride;
	UINT* m_pOffset;

	////////////////////////////////////////////////////
	//Shader Resource Views
	//
	////////////////////////////////////////////////////
	ID3D11ShaderResourceView*	m_pDiffuseSRV;
	ID3D11ShaderResourceView*	m_pNormalSRV;
	ID3D11ShaderResourceView*	m_pSpecularSRV;
	ID3D11ShaderResourceView*	m_pHeightSRV;

	////////////////////////////////////////////////////
	//Other rendering data
	////////////////////////////////////////////////////
	int	m_iNumVertices;

protected:

	// ============================================
	//		Define structures for vertex data
	// ============================================
	struct PositionData
	{
		XMFLOAT3 m_pos;

		//constructor
		PositionData(float x, float y, float z)
			: m_pos(x, y, z)
		{
		}
	};

	struct TexCoordData
	{
		XMFLOAT2 m_texCoord;

		//constructor
		TexCoordData(float tu, float tv)
			: m_texCoord(tu, tv)
		{
		}
	};

	struct NormalData
	{
		XMFLOAT3 m_normal;
		XMFLOAT3 m_tangent;
		XMFLOAT3 m_binormal;
	};

public:

	/////////////////////////////
	//Getters
	/////////////////////////////
	inline int GetNumVertices() const { return m_iNumVertices; }
};