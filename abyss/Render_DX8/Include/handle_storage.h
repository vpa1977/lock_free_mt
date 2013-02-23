#ifndef HANDLE_STORAGE_HPP_INCLUDED
#define HANDLE_STORAGE_HPP_INCLUDED

#include <set>

class CHandleStorage
{
public:
	CHandleStorage();
	~CHandleStorage();
	unsigned long GetNewHandle();
	void ReleaseHandle( unsigned long in_Handle );
	void Clear();
private:
	std::set<unsigned long> m_FreeHandles;
	unsigned long m_BeginOfFreeRange;
};

#endif