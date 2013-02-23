 // A1ContentHandler.h: derived class from SAXContentHandlerImpl
 
#if !defined(AFX_A1CONTENTHANDLER_H__E0CFBC18_CCC1_42F3_B0A4_B03331AB9693__INCLUDED_)
#define AFX_A1CONTENTHANDLER_H__E0CFBC18_CCC1_42F3_B0A4_B03331AB9693__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SAXContentHandlerImpl.h"
#include "AbstractHandler.h"
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <map>



class A1ContentHandler : public SAXContentHandlerImpl  
{
public:
	static A1ContentHandler * CreateInstance();
    A1ContentHandler();
    virtual ~A1ContentHandler();

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

private:
        void prt(
            /* [in] */ const wchar_t * pwchFmt,
            /* [in] */ const wchar_t __RPC_FAR *pwchVal,
            /* [in] */ int cchVal);
		int idnt;
		typedef std::map< std::basic_string<wchar_t>, boost::shared_ptr<AbstractHandler> > Handlers;		
		Handlers m_Handlers;
		AbstractHandler* m_ActiveHandler;
		
};

#endif // !defined(AFX_A1CONTENTHANDLER_H__E0CFBC18_CCC1_42F3_B0A4_B03331AB9693__INCLUDED_)
