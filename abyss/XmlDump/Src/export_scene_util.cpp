#include "../include/exportscene.h"
#include "../include/entity_writer.h"
#include "../include/animation_transformer.h"
#include "../../Render_DX8/include/algebra.h"


#include "../Include/mesh_optimizer.h"
#include <boost/shared_ptr.hpp>
#include <stdio.h>
#include <map>
#include <string>


/////////////////////////////////////////////////////////////////////////


IXMLDOMElement* ToElement(IXMLDOMNode* pNode)
{
	IXMLDOMElement* pElement= NULL;
	pNode->QueryInterface(IID_IXMLDOMElement , (void**)&pElement);
	pNode->Release();
	return pElement;
}

/////////////////////////////////////////////////////////////////////////

char* wcompress(wchar_t* data)
{
	if (data == NULL ) return NULL;
	// compresses data string to 1 byte
	int w_size = wcslen(data);
	int c_size = 2*w_size;
	int offset = 1;
	int index = 1;
	char* p = (char*)data;
	for (int i = 0 ; i < w_size ; i ++ ) 
	{
		p[index] = p[index+offset];
		index++;
		offset++;
	}
	p[index] = 0;
	return (char*) p;
}
wchar_t* nocomma(wchar_t* in)
{
	for (int i = 0 ; i < wcslen(in) ; i ++ ) 
	{
		if (in[i] == L',') in[i] = L'.';
	}
	return in;
}


wchar_t* getAttribute(IXMLDOMNode* pNode, wchar_t* attName)
{
	IXMLDOMElement* pElement;
	VARIANT val;
	BSTR bStr = SysAllocString(attName);
	HRESULT hr  = pNode->QueryInterface(IID_IXMLDOMElement ,(void**) &pElement);
	if (!pElement) 
	{
		return NULL;
	}

	pElement->getAttribute(bStr,&val);
	if (val.bstrVal == NULL ) return NULL;
	SysFreeString(bStr);
	pElement->Release();
	wchar_t * buffer = (wchar_t *)malloc (wcslen(val.bstrVal)*sizeof(wchar_t));
	memset(buffer, 0 , wcslen(val.bstrVal)*sizeof(wchar_t));
	wcscpy(buffer, (wchar_t*)val.bstrVal);

	return buffer;
}

wchar_t* getAttribute(IXMLDOMElement* pElement, wchar_t* attName)
{
	VARIANT val;
	BSTR bStr = SysAllocString(attName);
	pElement->getAttribute(bStr,&val);
	SysFreeString(bStr);
	if (val.bstrVal == NULL ) return NULL;

	wchar_t * buffer = (wchar_t *)malloc (wcslen(val.bstrVal)*sizeof(wchar_t));
	memset(buffer, 0 , wcslen(val.bstrVal)*sizeof(wchar_t));
	wcscpy(buffer, (wchar_t*)val.bstrVal);

	return buffer;
}


void xpathSelect(IXMLDOMDocument* pDoc , IXMLDOMElement** pElement, wchar_t* xpath)
{
	
	
	IXMLDOMNode *pNode=NULL;
	BSTR bStr = NULL;
	HRESULT hr;
	*pElement = NULL;
	bStr = SysAllocString(xpath);	
	hr = pDoc->selectSingleNode(bStr, &pNode);
	SysFreeString(bStr);
	if (FAILED(hr) || !pNode)
	{
		// no visual scene found, nothing to process.
		return;
	}
	hr  = pNode->QueryInterface(IID_IXMLDOMElement ,(void **) pElement);
	pNode->Release();
	if (FAILED(hr) || !pElement)
	{
		// scene not named nothing to do
		return;
	}
}

void xpathSelect(IXMLDOMNode* pINNode , IXMLDOMElement** pElement, wchar_t* xpath)
{
	
	
	IXMLDOMNode *pNode=NULL;
	BSTR bStr = NULL;
	HRESULT hr;
	*pElement = NULL;
	bStr = SysAllocString(xpath);		
	hr = pINNode->selectSingleNode(bStr, &pNode);
	SysFreeString(bStr);
	if (FAILED(hr) || !pNode)
	{
		// no visual scene found, nothing to process.
		return;
	}
	hr  = pNode->QueryInterface(IID_IXMLDOMElement ,(void **) pElement);
	pNode->Release();
	if (FAILED(hr) || !pElement)
	{
		// scene not named nothing to do
		return;
	}
}

void xpathSelect(IXMLDOMElement* pINNode , IXMLDOMElement** pElement, wchar_t* xpath)
{
	
	
	IXMLDOMNode *pNode=NULL;
	BSTR bStr = NULL;
	HRESULT hr;
	*pElement = NULL;
	bStr = SysAllocString(xpath);		
	hr = pINNode->selectSingleNode(bStr, &pNode);
	SysFreeString(bStr);
	if (FAILED(hr) || !pNode)
	{
		// no visual scene found, nothing to process.
		return;
	}
	hr  = pNode->QueryInterface(IID_IXMLDOMElement ,(void **) pElement);
	pNode->Release();
	if (FAILED(hr) || !pElement)
	{
		// scene not named nothing to do
		return;
	}
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// source helper methods


std::map< std::basic_string<wchar_t>  , FloatSource > create_sources(IXMLDOMElement* pMeshNode ) 
{
	std::map< std::basic_string<wchar_t>  , FloatSource > m_res;

	IXMLDOMNodeList* pNodeList;
	BSTR query = SysAllocString(L"source");
	pMeshNode->selectNodes(query,&pNodeList);
	SysFreeString(query);
	long length ;
	pNodeList->get_length(&length);
	for (int i = 0 ; i < length ; i ++ ) 
	{
		IXMLDOMNode* sourceNode;
		IXMLDOMElement* arrayNode;
		IXMLDOMElement* accessorNode;
		
		pNodeList->get_item(i, &sourceNode);

		xpathSelect(sourceNode, &arrayNode, L"float_array");
		xpathSelect(sourceNode, &accessorNode, L"technique_common/accessor");
		if (!accessorNode)
		{
			sourceNode->Release();
			continue;
		}
		if (!arrayNode)
		{
			sourceNode->Release();
			continue;
		}
		FloatSource nextSource;
		nextSource.offset = 0;
		BSTR text;
		arrayNode->get_text(&text);
			
		float res;
		wchar_t* stop;
		wchar_t * next = wcstok(text, L" \n\r");
		while (next) 
		{
			for (int i = 0 ; i < wcslen(next) ; i ++ ) 
			{
				if(next[i]==(wchar_t)L',') next[i]=(wchar_t)L'.';
			}
			res = (float)wcstod(next, & stop);
			nextSource.m_data.push_back(res);
			next = wcstok(NULL, L" \n\r");
		}
		wchar_t* sourceName = getAttribute(sourceNode, L"id");
		//sourceName ++;
		wchar_t* str_stride = getAttribute(accessorNode, L"stride");
		nextSource.stride = _wtoi(str_stride);
		
		
		IXMLDOMNodeList* pParamList;
		BSTR pStr = SysAllocString(L"param");
		accessorNode->selectNodes(pStr, &pParamList);
		long paramCount = 0;
		pParamList->get_length(&paramCount);
		nextSource.param = paramCount;
		pParamList->Release();
		SysFreeString(pStr);

		std::basic_string<wchar_t> index(sourceName);

		m_res[index]  = nextSource;
		sourceNode->Release();
		arrayNode->Release();
		accessorNode->Release();

	}
	pNodeList->Release();
	return m_res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// helper methods to retrieve data from collada objects
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CMatrix rotate(IXMLDOMElement* pElement)
{
	CMatrix m;
	BSTR text;
	float x, y, z;
	float angle;
	pElement->get_text(&text);
	swscanf(nocomma(text),L"%f %f %f %f", &x, &y, &z, &angle);
	angle =(float)( angle*	CONST_PI/180);
	if (x)
	{
		m.ConstructRotationX(x*angle);
	}
	else
	if (y)
	{
		m.ConstructRotationY(y*angle);
	}
	else
	if (z)
	{
		m.ConstructRotationZ(z*angle);
	}
	return m;
}

CMatrix scale(IXMLDOMElement* pElement)
{
	BSTR text;
	float x, y, z;
	CMatrix m;
	pElement->get_text(&text);
	swscanf(nocomma(text),L"%f %f %f", &x, &y, &z);
	return m.ConstructScaling(CVector(x,y,z));
	
}

CMatrix translate(IXMLDOMElement* pElement)
{
	CMatrix m;
	BSTR text;
	float x, y, z;
	pElement->get_text(&text);
	swscanf(nocomma(text),L"%f %f %f", &x, &y, &z);
	return m.ConstructTranslation(CVector(x,y,z));

}
CMatrix constructMatrix(IXMLDOMElement *pElement)
{
	BSTR text;
	float m[16];
	pElement->get_text(&text);
	swscanf(nocomma(text), L"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
		&m[0],&m[1],&m[2],&m[3],&m[4],
		&m[5],&m[6],&m[7],&m[8],&m[9],
		&m[10],&m[11],&m[12],&m[13],
		&m[14],&m[15]);
	CMatrix matr(m);
	
	return matr.Transpose();
}

CMatrix constructMatrixNoTranspose(IXMLDOMElement *pElement)
{
	BSTR text;
	float m[16];
	pElement->get_text(&text);
	swscanf(nocomma(text), L"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
		&m[0],&m[1],&m[2],&m[3],&m[4],
		&m[5],&m[6],&m[7],&m[8],&m[9],
		&m[10],&m[11],&m[12],&m[13],
		&m[14],&m[15]);
	CMatrix matr(m);
	
	return matr;
}

std::vector<float> read_float_data(IXMLDOMElement *pElement) 
{
	BSTR text;
	pElement->get_text(&text);
	wchar_t* next;
	std::vector<float> ret;
	next = wcstok( text, L" \r\n");
	while (next) 
	{
		float val;
		swscanf(nocomma(next), L"%f", &val);
		ret.push_back(val);
		next = wcstok(NULL , L" \r\n" );
	}
	return ret;
		
}

std::vector<int> read_int_data(IXMLDOMElement *pElement) 
{
	BSTR text;
	pElement->get_text(&text);
	wchar_t* next;
	std::vector<int> ret;
	next = wcstok( text, L" \r\n");
	while (next) 
	{
		int val;
		swscanf(next, L"%d", &val);
		ret.push_back(val);
		next = wcstok(NULL , L" \r\n" );
	}
	return ret;
}


int  findBone(std::string& name, std::vector<CBoneSerialized>& all_bones  ) 
{
		for (int i = 0 ; i < all_bones.size() ; i ++ ) 
		{
			if (all_bones[i].m_ID == name ) return i;
		}
		return -1;
}

int findBone(int index, std::vector<CBoneSerialized>& all_bones ) 
{
		for (int i = 0 ; i < all_bones.size() ; i ++ ) 
		{
			if (all_bones[i].m_Index == index ) return i;
		}
		return -1;
}


