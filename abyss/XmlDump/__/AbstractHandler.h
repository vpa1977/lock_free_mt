#pragma once
#include <map>
#include <stack>
/**
	Abstract Handler which processes a section of collada document
*/
class AbstractHandler
{
public:
virtual HRESULT STDMETHODCALLTYPE startElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName,
            /* [in] */ ISAXAttributes __RPC_FAR *pAttributes) = 0;

    virtual HRESULT STDMETHODCALLTYPE endElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName) = 0;

    virtual HRESULT STDMETHODCALLTYPE characters( 
            /* [in] */ wchar_t __RPC_FAR *pwchChars,
            /* [in] */ int cchChars) = 0;
protected:
	void convert(ISAXAttributes __RPC_FAR *pAttributes)
	{
		m_CurrentAttributes.clear();
		int lAttr;
		pAttributes->getLength(&lAttr);
		for(int i=0; i<lAttr; i++)
		{
			wchar_t * ln, * vl; 
			int lnl, vll;
			pAttributes->getQName(i,&ln,&lnl); 
			wstring key(ln, lnl);

			pAttributes->getValue(i,&vl,&vll);
			
			wstring val(vl, vll);
			m_CurrentAttributes[key] = val;
		}
	}
	void push_status(int status) 
	{
		m_Mode.push(status);
	}
	int pop_status()
	{
		int res = m_Mode.top();
		m_Mode.pop();
		return res;
	}
	int get_status()
	{
		return m_Mode.top();
	}
protected:
	typedef std::basic_string<wchar_t> wstring;
	typedef std::map< wstring,wstring> AttrMap;
	AttrMap m_CurrentAttributes;
private:
	std::stack<int> m_Mode;
};
