#ifndef IDGEN_HPP_INCLUDED
#define IDGEN_HPP_INCLUDED

#include <typeinfo>
#include <assert.h>

class CBaseIDGen
{
protected:
  static long m_NextID;
  static long RegisterType( const char* in_pszTypeName );
};

template <typename T>
class CIDGen : public CBaseIDGen
{
  long m_ID;
public:
  CIDGen()
  {
    m_ID = RegisterType( typeid(T).name() );
  }
  operator long() const
  {
    return m_ID;
  }
};

template <typename T>
struct CIDClass
{
  static CIDGen<T> m_IDGen;
};

template <typename T>
CIDGen<T> CIDClass<T>::m_IDGen;

template <typename T>
long GetID(const T*)
{
  static CIDClass<T> id;
  return (long)id.m_IDGen;
}

#endif



