#ifndef COLLADA_ARRAYS_HPP

#define COLLADA_ARRAYS_HPP

#include <vector>
#include <string>


typedef std::basic_string<wchar_t> wstring;

class ColladaNamedElement 
{
public:
	ColladaNamedElement(const wstring& name) : m_Name(name)
	{
	}

private:
	 wstring m_Name;
};


struct ColladaAccessor
{
	ColladaAccessor() : offset(0), stride(1), length(0)
	{
	}
	int offset;
	int stride;
	int length;
	std::vector<wstring> elementNames;
};


template <typename T>
class ColladaArray : public ColladaNamedElement
{
public:
	ColladaArray( const wstring& name) 
		: ColladaNamedElement(name)
	{
		
	}
public:
	
	std::vector<T> operator[](int index) 
	{
		std::vector<T> retVal;
		assert(m_Accessor.length);
		int arrIndex = m_Accessor.offset + index * m_Accessor.stride;
		for (int i = 0 ; i < m_Accessor.elementNames.size() ; i ++ ) 
		{
			retVal[i] = m_Array[arrIndex + i];
		}
		return retVal;
	}

	void SetAccessor(const ColladaAccessor& accessor)
	{
		m_Accessor = accessor;
	}

	const ColladaAccessor& GetAccessor()
	{
		return m_Accessor;
	}
protected:
	std::vector<T> m_Array;
private:
	ColladaAccessor m_Accessor;

};


class CFloatArray : public ColladaArray<float>
{
	public:
		CFloatArray(const wstring& name, wchar_t* source) : ColladaArray<float>(name)
		{
			float res;
			wchar_t* stop;
			wchar_t * next = wcstok(source, L" ");
			while (next) 
			{
				res = (float)wcstod(next, & stop);
				m_Array.push_back(res);
				next = wcstok(NULL, L" ");
			}
		}

};


class CSource 
{
public:

	wstring m_ID;

	int m_Selector;
	ColladaAccessor m_Accessor;
	CFloatArray     m_FloatArray;
	CBoolArray  m_BoolArray;
	CIntArray  m_IntArray;
	CNameArray  m_NameArray;
	CRefArray  m_RefArray;
};

#endif