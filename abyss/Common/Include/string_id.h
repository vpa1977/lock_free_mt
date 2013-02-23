#ifndef STRING_ID_HPP_INCLUDED
#define STRING_ID_HPP_INCLUDED

#include <string>

struct CStringID
{
	CStringID( const std::string& in_rStr ) {
	}

	bool operator < ( const CStringID& in_rOther ) const {
		if( m_ID < in_rOther.m_ID )
			return true;
		else if( m_ID > in_rOther.m_ID )
			return false;
		else
			return m_String < in_rOther.m_String;
	}
private:
	unsigned long m_ID;
	std::string m_String;
};

#endif


































