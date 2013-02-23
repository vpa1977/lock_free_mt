#ifndef LOAD_PROXY_HPP_INCLUDED
#define LOAD_PROXY_HPP_INCLUDED

#include "../../Preloader/Include/section_preloader_interface.h"

#include "proxy_action.h"

#include <string>

class CLoadUser
{
public:
	virtual void ReadFileResult( bool in_bSuccess, const std::string& in_rFileName, const std::vector<unsigned char>& in_rData ) = 0;
	virtual void DecodeFileResult( bool in_bSuccess, const std::string& in_rFileName, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData ) = 0;
	virtual void DecodeRawResult( bool in_bSuccess, long in_Width, long in_Height, const std::vector<unsigned char>& in_rData ) = 0;
};

/*
struct COpenKey
{
	COpenKey( CLoadUser* in_pUser, std::string in_URL ) : m_pUser(in_pUser), m_URL(in_URL) {}
	CLoadUser* m_pUser;
	std::string m_URL;
};
bool operator == ( const COpenKey& in_rFirst, const COpenKey& in_rSecond )
{
	if(in_rFirst.m_pUser!=in_rSecond.m_pUser) return false;
	return in_rFirst.m_URL==in_rSecond.m_URL;
}
bool operator < ( const COpenKey& in_rFirst, const COpenKey& in_rOther )
{
	if(in_rFirst.m_pUser<in_rSecond.m_pUser) return true;
	if(in_rFirst.m_pUser>in_rSecond.m_pUser) return false;
	return in_rFirst.m_URL<in_rSecond.m_pUser;
}
*/

enum AWAITED_ACTION
{
	// open
	AA_OPEN_FOR_READ		= 100,
	AA_OPEN_FOR_DECODE		= 101,
	// read
	AA_READ					= 200,
	// close
	AA_CLOSE_AFTER_READ		= 300,
	AA_CLOSE_AFTER_DECODE	= 301,
	// decode
	AA_DECODE_RAW			= 400,
	AA_DECODE_FILE			= 401
};

typedef std::pair<CLoadUser*,AWAITED_ACTION> CAwaitedAction;

class CLoadProxy  :
	protected ProxyAction< unsigned long, CAwaitedAction >
	,protected ProxyAction< std::string, CAwaitedAction >
{
public:
	virtual void ReadFile(  const char* in_pszFileName, CLoadUser* in_pUser ) = 0;
	virtual void DecodeImageFromFile( const char* in_pszFileName, CLoadUser* in_pUser ) = 0;
	enum IMAGE_TYPE
	{
		IT_BMP,
		IT_TGA,
		IT_JPG
	};
	virtual void DecodeRawImage( IMAGE_TYPE in_Type, const std::vector<unsigned char> in_rData, CLoadUser* in_pUser ) = 0;

protected:
	void setInstance(CLoadProxy* instance)
	{
		m_pInstance = instance;
	}

public:
	static CLoadProxy& getInstance()
	{
		return (CLoadProxy&) (*m_pInstance);
	}

	static CLoadProxy* m_pInstance;



};

#endif







