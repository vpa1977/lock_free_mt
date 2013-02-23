#include "StdAfx.h"
#include ".\geometryhandler.h"
#include <boost/shared_ptr.hpp>

CGeometryHandler::CGeometryHandler(void)
{
	
}

CGeometryHandler::~CGeometryHandler(void)
{
}

HRESULT STDMETHODCALLTYPE CGeometryHandler::startElement( 
    /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
    /* [in] */ int cchNamespaceUri,
    /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
    /* [in] */ int cchLocalName,
    /* [in] */ wchar_t __RPC_FAR *pwchRawName,
    /* [in] */ int cchRawName,
    /* [in] */ ISAXAttributes __RPC_FAR *pAttributes)
{
	convert(pAttributes);
	wstring localName(pwchLocalName);
	if (localName == L"float_array") 
	{
		// create a float array
		push_status(FLOAT_ARRAY);
	}
	//// last case - dont do anything 
	else
	{
		push_status(IDLE);
	}

return S_OK;
}

HRESULT STDMETHODCALLTYPE CGeometryHandler::endElement( 
    /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
    /* [in] */ int cchNamespaceUri,
    /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
    /* [in] */ int cchLocalName,
    /* [in] */ wchar_t __RPC_FAR *pwchRawName,
    /* [in] */ int cchRawName)
{
	pop_status();
return S_OK;
}

HRESULT STDMETHODCALLTYPE CGeometryHandler::characters( 
    /* [in] */ wchar_t __RPC_FAR *pwchChars,
    /* [in] */ int cchChars) 
{
	switch (get_status()) 
	{
	case FLOAT_ARRAY:{
							AttrMap::iterator it = m_CurrentAttributes.find(L"id");
							if (it != m_CurrentAttributes.end() ) 
							{
								boost::shared_ptr<CFloatArray> fArr = 
									boost::shared_ptr<CFloatArray>(new 
											CFloatArray(it->second, 
											pwchChars));
							}
					 }
					 break;


	}
	
	return S_OK;
}
