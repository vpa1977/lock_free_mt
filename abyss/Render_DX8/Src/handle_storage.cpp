#include "../Include/handle_storage.h"
#include <assert.h>

CHandleStorage::CHandleStorage() :
	m_BeginOfFreeRange(1)
{
}

CHandleStorage::~CHandleStorage()
{
	assert( m_FreeHandles.empty() );
	assert( m_BeginOfFreeRange == 1);
}

unsigned long
CHandleStorage::GetNewHandle()
{
	if( m_FreeHandles.empty() )
	{
		return m_BeginOfFreeRange++;
	}
	else
	{
		assert( m_BeginOfFreeRange > 2 ); // smallest possible case: m_BeginOfFreeRange==3 and m_FreeHandles contains only "1".
		unsigned long SmallestFree = *(m_FreeHandles.begin());
		assert( SmallestFree >= 1 );
		size_t qq = m_FreeHandles.erase( SmallestFree );
		assert( qq == 1 );
		return SmallestFree;
	}
}

void
CHandleStorage::ReleaseHandle( unsigned long in_Handle )
{
	if(in_Handle>0)
	{
		assert( in_Handle < m_BeginOfFreeRange );
		std::set<unsigned long>::iterator It = m_FreeHandles.find(in_Handle);
		assert( It == m_FreeHandles.end() );
		if( in_Handle == m_BeginOfFreeRange-1 )
		{
			if( in_Handle == 1 )
			{
				assert( m_FreeHandles.empty() );
				m_BeginOfFreeRange = 1;
			}
			else if( m_FreeHandles.empty() )
			{
				m_BeginOfFreeRange--;
			}
			else
			{
				m_BeginOfFreeRange--;
				std::set<unsigned long>::iterator ToBeErased = m_FreeHandles.find(m_BeginOfFreeRange-1);
				while( ToBeErased != m_FreeHandles.end() )
				{
					m_FreeHandles.erase( ToBeErased );
					m_BeginOfFreeRange--;
					ToBeErased = m_FreeHandles.find(m_BeginOfFreeRange-1);
				}
			}
		}
		else
		{
			std::pair< std::set<unsigned long>::iterator, bool > InsertResult = m_FreeHandles.insert( in_Handle );
			assert( InsertResult.second == true );
		}
	}
}

void
CHandleStorage::Clear()
{
	m_FreeHandles.clear();
	m_BeginOfFreeRange = 1;
}


















