#ifndef EXPORT_SCENE_UTIL_HPP

#define EXPORT_SCENE_UTIL_HPP
#include <windows.h>
#include <stdio.h>
#include <map>
#include "animation.h"
#include "../../MT_Core/Include/Log.h"



struct FloatSource
{
	FloatSource() : offset(0), stride(0), length(0)
	{
	}
	std::vector<float> m_data;
	int offset;
	int stride;
	int length;
	int param;
	std::vector<float> get(int index) 
	{
		std::vector<float> retVal;	
		assert(m_data.size());
		int arrIndex = offset + index * stride;
		for (int i = arrIndex ; i <arrIndex+param ; i ++ ) 
		{
			retVal.push_back(m_data[i]);
		}
		return retVal;
	}
};


int    findBone(int index,  std::vector<CBoneSerialized>& all_bones ) ;
int    findBone(std::string& name,  std::vector<CBoneSerialized>& all_bones ) ;


char* wcompress(wchar_t* data);
wchar_t* nocomma(wchar_t* in);
wchar_t* getAttribute(IXMLDOMNode* pNode, wchar_t* attName);
wchar_t* getAttribute(IXMLDOMElement* pElement, wchar_t* attName);
void xpathSelect(IXMLDOMDocument* pDoc , IXMLDOMElement** pElement, wchar_t* xpath);
void xpathSelect(IXMLDOMNode* pINNode , IXMLDOMElement** pElement, wchar_t* xpath);
void xpathSelect(IXMLDOMElement* pINNode , IXMLDOMElement** pElement, wchar_t* xpath);
std::map< std::basic_string<wchar_t>  , FloatSource > create_sources(IXMLDOMElement* pMeshNode );
IXMLDOMElement* ToElement(IXMLDOMNode* pNode);
CMatrix rotate(IXMLDOMElement* pElement);
CMatrix scale(IXMLDOMElement* pElement);
CMatrix translate(IXMLDOMElement* pElement);
CMatrix constructMatrix(IXMLDOMElement *pElement);
CMatrix constructMatrixNoTranspose(IXMLDOMElement *pElement);
std::vector<float> read_float_data(IXMLDOMElement *pElement) ;
std::vector<int> read_int_data(IXMLDOMElement *pElement) ;


class XMLNodeEnumerator 
{
public:
	XMLNodeEnumerator(IXMLDOMNodeList* pList)
	{
		m_pList = pList;
		m_pNode = NULL;
		m_pList->get_length(&m_size);
	}
	XMLNodeEnumerator(IXMLDOMNode* pNode, wchar_t* selectQuery)
	{
		m_pList = NULL;
		m_pNode = pNode;
		if (!m_pNode ) {
				m_size = 0;
				return;
		}
		m_pNode->selectNodes(selectQuery,&m_pList);
		m_pList->get_length(&m_size);
	}
	~XMLNodeEnumerator()
	{
		if (m_pList)
			m_pList->Release();

	}
	long size()
	{
		
		return m_size;
	}

	IXMLDOMNode* operator[](unsigned int index)
	{
		assert(index < size());
		assert(m_pList);
		IXMLDOMNode* pNode;
		m_pList->get_item(index, &pNode);
		return pNode;
	}


private:
	IXMLDOMNode* m_pNode;
	IXMLDOMNodeList* m_pList;
	long m_size;

};



#endif