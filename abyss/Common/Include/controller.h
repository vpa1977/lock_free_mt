#ifndef CONTROLLER_HPP_INCLUDED
#define CONTROLLER_HPP_INCLUDED

#include "string_id.h"

#include <set>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
using namespace ::boost;
using namespace ::boost::multi_index;

template< class T >
class CController
{
public:
	virtual ~CController() {}

	virtual void Added( T& in_rHost ) {}
	virtual void Removed( T& in_rHost ) {}

	virtual bool Process( T& in_rHost ) = 0; // when this method returns false, controller is removed and destroyed

	virtual const std::string& GetName() const = 0;
};

template< class T >
class CControllable
{
public:
	virtual ~CControllable() {
		CControllersStorage_by_Pointer& rIndexPointers = m_Controllers.get<Tag_Pointer>();
		for( CControllersStorage_by_Pointer::iterator It = rIndexPointers.begin(); It != rIndexPointers.end(); ++It ) {
			delete (*It).m_Pointer;
		}
	}

	// When it returns true, controller is added to internal storage,
	// so calling code must forget this pointer.
	// When it returns false, calling code is still responsible for controller.
	bool AddController( CController<T>* in_pController ) {
		CControllersStorage_by_Pointer& rIndexPointers = m_Controllers.get<Tag_Pointer>();
		std::pair< CControllersStorage_by_Pointer::iterator, bool > Result = rIndexPointers.insert( CControllerData(in_pController) );
		if( Result.second ) {
			in_pController->Added( *(static_cast<T*>(this)) );
			return true;
		}
		return false;
	}

	void ProcessControllers() {
		// TODO
		// BIG FUCKING TODO
		// Process children also !!
		CControllersStorage_by_Pointer& rIndexPointers = m_Controllers.get<Tag_Pointer>();
		if (!rIndexPointers.size()) 
		{
			return;
		}
		std::set< CController<T>* > Suicides;
		for( CControllersStorage_by_Pointer::iterator It = rIndexPointers.begin(); It != rIndexPointers.end(); ++It ) {
			if( false == (*It).m_Pointer->Process( *(static_cast<T*>(this)) ) ) {
				Suicides.insert( (*It).m_Pointer );
			}
		}
		for( std::set< CController<T>* >::iterator It = Suicides.begin(); It != Suicides.end(); ++It ) {
			rIndexPointers.erase( *It );
		}
	}

	std::set< CController<T>* > FindControllers( const std::string& in_rName ) {
		CControllersStorage_by_Name& rIndexNames = m_Controllers.get<Tag_Name>();
		std::pair<CControllersStorage_by_Name::iterator,CControllersStorage_by_Name::iterator> Range = rIndexNames.equal_range( CStringID(in_rName) );
		std::set< CController<T>* > Result;
		for( CControllersStorage_by_Name::iterator It = Range.first; It != Range.second; ++It ) {
			Result.insert( (*It).m_Pointer );
		}
		return Result;
	}
	unsigned long RemoveControllers( const std::string& in_rName ) {
		unsigned long N = 0;
		CControllersStorage_by_Name& rIndexNames = m_Controllers.get<Tag_Name>();
		for(;;) {
			CControllersStorage_by_Name::iterator It = rIndexNames.find( CStringID(in_rName) );
			if( It != rIndexNames.end() ) {
				CController<T>* pC = (*It).m_Pointer;
				rIndexNames.erase( It );
				pC->Removed( *(static_cast<T*>(this)) );
				delete pC;
				++N;
			}
			else
				break;
		}
		return N;
	}

	// true - removed successfully
	// false - not found
	bool RemoveController( CController<T>* in_pController ) {
		CControllersStorage_by_Pointer& rIndexPointers = m_Controllers.get<Tag_Pointer>();
		CControllersStorage_by_Pointer::iterator It = rIndexPointers.find( in_pController );
		if( It != rIndexPointers.end() ) {
			CController<T>* pC = (*It).m_Pointer;
			rIndexPointers.erase( It );
			pC->Removed( *(static_cast<T*>(this)) );
			delete pC;
			return true;
		}
		return false;
	}

private:

	struct CControllerData {
		CControllerData( CController<T>* in_pController ) :
			m_Pointer(in_pController)
			,m_Name(in_pController->GetName())
		{}
		CController<T>* m_Pointer;
		CStringID m_Name;
	};

	class Tag_Pointer {};
	typedef ordered_unique
	<
		tag< Tag_Pointer >,
		member< CControllerData, CController<T>*, &CControllerData::m_Pointer >
	> CIndexPointer;

	class Tag_Name {};
	typedef ordered_non_unique
	<
		tag< Tag_Name >,
		member< CControllerData, CStringID, &CControllerData::m_Name >
	> CIndexName;

	typedef multi_index_container
	<
		CControllerData,
		indexed_by
		<
			CIndexPointer
			,CIndexName
		>
	> CControllersStorage;

	typedef typename CControllersStorage::index< Tag_Pointer >::type CControllersStorage_by_Pointer;
	typedef typename CControllersStorage::index< Tag_Name >::type CControllersStorage_by_Name;
	CControllersStorage m_Controllers;
};

#endif
































