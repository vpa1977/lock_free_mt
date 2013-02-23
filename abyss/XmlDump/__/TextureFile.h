#pragma once

#include "AbstractHandler.h"
/** 
	Responsible for exporting texture information
*/
class CTextureFile : public AbstractHandler
{
public:
	CTextureFile(void);
	~CTextureFile(void);
public:
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
