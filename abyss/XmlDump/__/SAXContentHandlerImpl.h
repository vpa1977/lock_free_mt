// SAXContentHandlerImpl.h:implementation class for ISAXDeclHandler interface

#if !defined(AFX_SAXCONTENTHANDLERIMPL_H__9FF07E95_E58C_424C_B8EA_84E448D5DD79__INCLUDED_)
#define AFX_SAXCONTENTHANDLERIMPL_H__9FF07E95_E58C_424C_B8EA_84E448D5DD79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SAXContentHandlerImpl : public ISAXContentHandler  
{
public:
    SAXContentHandlerImpl();
    virtual ~SAXContentHandlerImpl();

    // This must be correctly implemented, if your handler must be a COM Object 
	// the current implementation is NOT thread-safe
    long __stdcall QueryInterface(const struct _GUID &,void ** );
    unsigned long __stdcall AddRef(void);
    unsigned long __stdcall Release(void);

    virtual HRESULT STDMETHODCALLTYPE putDocumentLocator( 
            /* [in] */ ISAXLocator __RPC_FAR *pLocator);
        
    virtual HRESULT STDMETHODCALLTYPE startDocument( void);
        
    virtual HRESULT STDMETHODCALLTYPE endDocument( void);
        
    virtual HRESULT STDMETHODCALLTYPE startPrefixMapping( 
            /* [in] */ wchar_t __RPC_FAR *pwchPrefix,
            /* [in] */ int cchPrefix,
            /* [in] */ wchar_t __RPC_FAR *pwchUri,
            /* [in] */ int cchUri);
        
    virtual HRESULT STDMETHODCALLTYPE endPrefixMapping( 
            /* [in] */ wchar_t __RPC_FAR *pwchPrefix,
            /* [in] */ int cchPrefix);
        
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
            /* [in] */ int cchChars);
        
    virtual HRESULT STDMETHODCALLTYPE ignorableWhitespace( 
            /* [in] */ wchar_t __RPC_FAR *pwchChars,
            /* [in] */ int cchChars);
        
    virtual HRESULT STDMETHODCALLTYPE processingInstruction( 
            /* [in] */ wchar_t __RPC_FAR *pwchTarget,
            /* [in] */ int cchTarget,
            /* [in] */ wchar_t __RPC_FAR *pwchData,
            /* [in] */ int cchData);
        
    virtual HRESULT STDMETHODCALLTYPE skippedEntity( 
            /* [in] */ wchar_t __RPC_FAR *pwchName,
            /* [in] */ int cchName);
private:
	ULONG m_refCnt;
};

#endif // !defined(AFX_SAXCONTENTHANDLERIMPL_H__9FF07E95_E58C_424C_B8EA_84E448D5DD79__INCLUDED_)
