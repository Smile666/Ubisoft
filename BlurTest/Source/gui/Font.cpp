#include "BlurTestStd.h"
#include "Font.h"

using namespace std;

Font::~Font()
{
	SAFE_DELETE_ARRAY(m_pSymbolsData);
	SAFE_RELEASE(m_pFontTextureSRV);
}

bool Font::LoadAsciiData(const char * const filename)
{
	const int numChars = 95;
	m_pSymbolsData = new CharData[numChars];

	//try to open file with font data
	ifstream fIn;
	fIn.open(filename);
	if (fIn.fail())
	{
		assert(0 && "Error loading font data!");
		return false;
	}

	/////////////////////////////////
	//Load data for ascii characters
	char temp;
	for (int i = 0; i < numChars; i++)
	{
		fIn.get(temp);
		while (temp != ' ') { fIn.get(temp); }
		fIn.get(temp);
		while (temp != ' ') { fIn.get(temp); }

		fIn >> m_pSymbolsData[i].m_u1;
		fIn >> m_pSymbolsData[i].m_u2;
		fIn >> m_pSymbolsData[i].width;

		fIn.get(temp);
	}

	//now close opened file
	fIn.close();

	return true;
}

bool Font::LoadFontTexture(ID3D11Device* pDevice, const wchar_t * filename)
{
	HRESULT hr;
	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, filename, NULL, NULL, &m_pFontTextureSRV, NULL);
	VALID(hr);
}

void Font::BuildText(ID3D11Device* pDevice, ID3D11Buffer** ppBuffer, int & numVertices, const char * const text, const float posx, const float posy)
{
	FontVertex* pVertices;

	float curPosX = posx;

	//get the number of literals
	int iNumLiterals = static_cast<int>(strlen(text));
	numVertices = iNumLiterals * 6;

	pVertices = new FontVertex[numVertices];

	// =====================================
	// Build vertex data!
	int literal;
	int curVertex = 0;
	for (int i = 0; i < iNumLiterals; i++)
	{
		literal = static_cast<int>(text[i]) - 32;

		//space
		if (literal == ' ')
			curPosX += 3.0f;
		else
		{
			///// first triangle /////

			//top left
			pVertices[curVertex++] = FontVertex(curPosX, posy, 0.0f,
												m_pSymbolsData[literal].m_u1, 0.0f);
			
			//bottom right
			pVertices[curVertex++] = FontVertex(curPosX + m_pSymbolsData[literal].width, posy - 16.0f, 0.0f,
												m_pSymbolsData[literal].m_u2, 1.0f);

			//bottom left
			pVertices[curVertex++] = FontVertex(curPosX, posy - 16.0f, 0.0f,
												m_pSymbolsData[literal].m_u1, 1.0f);

			///// second triangle /////
			//top left
			pVertices[curVertex++] = FontVertex(curPosX, posy, 0.0f,
												m_pSymbolsData[literal].m_u1, 0.0f);

			//top right
			pVertices[curVertex++] = FontVertex(curPosX + m_pSymbolsData[literal].width, posy, 0.0f,
												m_pSymbolsData[literal].m_u2, 0.0f);

			//bottom right
			pVertices[curVertex++] = FontVertex(curPosX + m_pSymbolsData[literal].width, posy - 16.0f, 0.0f,
												m_pSymbolsData[literal].m_u2, 1.0f);

			//offset
			curPosX += m_pSymbolsData[literal].width + 1.0f; 
		}
	}  

	//////////////////////////////////////
	//Create vertex buffer

	HRESULT hr;

	//Vertex Buffer Description 
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = sizeof(FontVertex) * numVertices;

	//////////////////////////////////////////////
	//Subresource data
	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	bufferData.pSysMem = pVertices;

	hr = pDevice->CreateBuffer(&bufferDesc, &bufferData, ppBuffer);
	//VALID(hr);

}