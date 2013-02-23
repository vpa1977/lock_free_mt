/*********************************************
*
*  Testing section registration
*
**********************************************/

#include <typeinfo>
#include <stdio.h>

template< typename T >
class Base
{
public:
	void Foo()
	{
		printf("Base<%s>::Foo()\n", typeid(T).name() );
		(static_cast<T*>(this))->Bar();
	}
};

template< typename C, typename T >
class Medium : public virtual Base<C>
{
public:
	void QQ(T* in_Ptr)
	{
		printf("Medium<%s>::QQ()\n", typeid(T).name() );
	}
};

class BarProvider
{
public:
	void Bar() const
	{
		printf("BarProvider::Bar()\n");
	}
};

struct ZZZ {};

class Final :
	public BarProvider,
	public Medium<Final,int>,
	public Medium<Final,ZZZ>
{
public:
};

int
main(int argc, char* argv[])
{
	//TEST<int,int> tii;
	//tii.Test();
	Final f;
	f.Foo();
	f.QQ<int>( NULL );
	f.QQ<ZZZ>( NULL );
	return 0;
}


/*
#include "../MT_Core/Include/thread.h"
#include "../MT_Core/Include/section_registrator.h"
#include <stdio.h>

int
main(int argc, char* argv[])
{
	printf("main() begin -----------------------\n");
	DumpRegisteredSections();
	CThread::RunPool();
	printf("main() end -------------------------\n");
	return 0;
}
*/








