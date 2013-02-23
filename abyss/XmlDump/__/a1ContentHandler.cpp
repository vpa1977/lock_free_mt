 // A1ContentHandler.cpp: derived class from SAXContentHandlerImpl
 
#include "stdafx.h"
#include "A1ContentHandler.h"
#include "TextureFile.h"
#include "GeometryHandler.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


A1ContentHandler::A1ContentHandler()
{
	m_ActiveHandler = NULL;
	std::basic_string<wchar_t> p;
	p = L"library_images";
	m_Handlers[p] = boost::shared_ptr<CTextureFile>(new CTextureFile());
	p = L"library_geometries";
	m_Handlers[p] = boost::shared_ptr<CGeometryHandler>(new CGeometryHandler());
}

A1ContentHandler::~A1ContentHandler()
{
	//object destruction is handled by the Release() impl of parent class
}

//hides the new operator
A1ContentHandler * A1ContentHandler::CreateInstance()
{
	A1ContentHandler * pA1ContentHandler = NULL;
	pA1ContentHandler = new A1ContentHandler();
	assert(pA1ContentHandler!=NULL);
	return pA1ContentHandler;
}

HRESULT STDMETHODCALLTYPE A1ContentHandler::startElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName,
            /* [in] */ ISAXAttributes __RPC_FAR *pAttributes)
{
	std::basic_string<wchar_t> param;
	
	param = pwchLocalName;

	Handlers::iterator it = m_Handlers.find(param);
	if (it != m_Handlers.end() ) 
	{
		m_ActiveHandler = it->second.get();
	}

	if (m_ActiveHandler != NULL) 
	{
		return m_ActiveHandler->startElement( 
            /* [in] */ pwchNamespaceUri,
            /* [in] */ cchNamespaceUri,
            /* [in] */ pwchLocalName,
            /* [in] */ cchLocalName,
            /* [in] */ pwchRawName,
            /* [in] */ cchRawName,
            /* [in] */ pAttributes);
	}
/*	prt(L"<%s", pwchLocalName, cchLocalName);
	int lAttr;
	pAttributes->getLength(&lAttr);
	for(int i=0; i<lAttr; i++)
	{
		wchar_t * ln, * vl; int lnl, vll;
		pAttributes->getQName(i,&ln,&lnl); 
		prt(L" %s=", ln, lnl);
		pAttributes->getValue(i,&vl,&vll);
		prt(L"\"%s\"", vl, vll);
	}
	printf(">"); 
*/


    return S_OK;
	// return E_FAIL;     //The parse operation should be aborted. 

}

HRESULT STDMETHODCALLTYPE A1ContentHandler::endElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName)
{
	std::basic_string<wchar_t> param;
	
	param = pwchLocalName;

	Handlers::iterator it = m_Handlers.find(param);
	if (it != m_Handlers.end() ) 
	{
		m_ActiveHandler = NULL;
	}

	if (m_ActiveHandler != NULL) 
	{
		return m_ActiveHandler->endElement( 
            /* [in] */ pwchNamespaceUri,
            /* [in] */  cchNamespaceUri,
            /* [in] */ pwchLocalName,
            /* [in] */ cchLocalName,
            /* [in] */pwchRawName,
            /* [in] */ cchRawName);
	}
    return S_OK;
	// return E_FAIL;     //The parse operation should be aborted. 

}

HRESULT STDMETHODCALLTYPE A1ContentHandler::characters( 
            /* [in] */ wchar_t __RPC_FAR *pwchChars,
            /* [in] */ int cchChars)
{
	if (m_ActiveHandler != NULL) 
	{
		std::vector<wchar_t> buffer;
		buffer.resize(cchChars+1);
		memcpy((char*)&buffer[0], pwchChars , sizeof(wchar_t)*cchChars);
		return m_ActiveHandler->characters( 
            /* [in] */ (wchar_t*)&buffer[0],
            /* [in] */ cchChars);
	}
    return S_OK;
	// return E_FAIL;     //The parse operation should be aborted. 

}

void A1ContentHandler::prt(
            /* [in] */ const wchar_t * pwchFmt,
            /* [in] */ const wchar_t __RPC_FAR *pwchVal,
            /* [in] */ int cchVal)
{
    static wchar_t val[1000];
	cchVal = cchVal>999 ? 999 : cchVal;
    wcsncpy( val, pwchVal, cchVal );
	val[cchVal] = 0;
    wprintf(pwchFmt,val);
}






