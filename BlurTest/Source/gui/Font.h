#pragma once

class Font
{
	friend class Text;
public:
	Font();
	~Font();

private:
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
	ID3D11ShaderResourceView*	GetShaderResource() const { return m_pFontTextureSRV; }

private:
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

	/******** Methods for loading data ********/
	bool LoadAsciiData(const char * const filename);
	bool LoadFontTexture(ID3D11Device* pDevice, const wchar_t * filename);

	/******** Graphics API Text Creation ********/
	void BuildText(ID3D11Device* pDevice, ID3D11Buffer** ppBuffer, int & numVertices, const char * const text, const float posx, const float posy);
};