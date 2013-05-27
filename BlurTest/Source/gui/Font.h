#pragma once

//========================================================================
// Font.h
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#pragma once

class Font
{
	friend class Text;

public:
	Font() {}
	~Font();

private:
	//single character data
	struct CharData
	{
		float	m_u1;
		float	m_u2;
		float	width;
	};

	CharData*	m_pSymbolsData;

	//font texture
	ID3D11ShaderResourceView*	m_pFontTextureSRV;

public:
	/******** Methods for loading data ********/
	bool LoadAsciiData(const char * const filename);
	bool LoadFontTexture(ID3D11Device* pDevice, const wchar_t * filename);

	/******** Accessors ********/
	ID3D11ShaderResourceView*	GetShaderResource() const { return m_pFontTextureSRV; }

private:
	//font vertex type
	struct FontVertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 texCoord;

		FontVertex() : pos(0.0f, 0.0f, 0.0f), texCoord(0.0f, 0.0f)
		{
		}

		FontVertex(	const float px, const float py, const float pz,
					const float tu, const float tv) :
		pos(px, py, pz), texCoord(tu, tv)
		{
		}
	};

	/******** Graphics API Text Creation ********/
	bool BuildText(ID3D11Device* pDevice, ID3D11Buffer** ppBuffer, int & numVertices, const char * const text, const float posx, const float posy);
};