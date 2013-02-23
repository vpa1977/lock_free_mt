#pragma once
#include "abstracthandler.h"
#include "arrays.h"


#define SOURCE
#define FLOAT_ARRAY 1
#define IDLE 0 


class CGeometryHandler :
	public AbstractHandler
{

public:
	CGeometryHandler(void);
	~CGeometryHandler(void);
	virtual HRESULT STDMETHODCALLTYPE startElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName,
            /* [in] */ ISAXAttributes __RPC_FAR *pAttributes);

    virtual HRESULT STDMETHODCALLTYPE endElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName);

    virtual HRESULT STDMETHODCALLTYPE characters( 
            /* [in] */ wchar_t __RPC_FAR *pwchChars,
            /* [in] */ int cchChars) ;
};
