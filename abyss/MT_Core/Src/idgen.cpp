#include "../Include/impl/stdafx.h"
#include <map>
#include <string>
#include "../Include/impl/idgen.h"
#include <stdio.h>

long CBaseIDGen::m_NextID = 1;

long
CBaseIDGen::RegisterType
(const char* in_pszTypeName)
{
  //printf("  RegisterType( \"%s\" )\n",in_pszTypeName);
  typedef std::map< std::string, long > CTypeIDMap;
  static CTypeIDMap g_RegisteredTypes;
  CTypeIDMap::const_iterator it = g_RegisteredTypes.find( std::string(in_pszTypeName) );
  if( it != g_RegisteredTypes.end() )
  {
    //printf("    already registered: %ld\n",(*it).second);
    return (*it).second;
  }
  else
  {
    //printf("    not registered before: %ld\n",m_NextID);
    g_RegisteredTypes.insert( CTypeIDMap::value_type(in_pszTypeName,m_NextID) );
    return m_NextID++;
  }
}



