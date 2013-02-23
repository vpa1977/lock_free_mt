#include "StdAfx.h"
#include ".\texturefile.h"

CTextureFile::CTextureFile(void)
{
}

CTextureFile::~CTextureFile(void)
{
}
HRESULT STDMETHODCALLTYPE CTextureFile::startElement( 
    /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
    /* [in] */ int cchNamespaceUri,
    /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
    /* [in] */ int cchLocalName,
    /* [in] */ wchar_t __RPC_FAR *pwchRawName,
    /* [in] */ int cchRawName,
    /* [in] */ ISAXAttributes __RPC_FAR *pAttributes)
{
return S_OK;
}

HRESULT STDMETHODCALLTYPE CTextureFile::endElement( 
    /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
    /* [in] */ int cchNamespaceUri,
    /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
    /* [in] */ int cchLocalName,
    /* [in] */ wchar_t __RPC_FAR *pwchRawName,
    /* [in] */ int cchRawName)
{
return S_OK;
}

HRESULT STDMETHODCALLTYPE CTextureFile::characters( 
    /* [in] */ wchar_t __RPC_FAR *pwchChars,
    /* [in] */ int cchChars) 
{
	return S_OK;
}
