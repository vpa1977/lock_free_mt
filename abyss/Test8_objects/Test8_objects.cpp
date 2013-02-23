#include "element.h"
#include "death.h"
#include "link.h"
#include <stdio.h>

//===================================================================================================

class TestTarget : public CLinkTarget
{
public:
	TestTarget() {
		printf("TestTarget(%p)::TestTarget()\n",this);
	}
	~TestTarget() {
		printf("TestTarget(%p)::~TestTarget()\n",this);
	}
protected:
	void SomethingLinkedToYou( CLinkOrigin* in_pOrigin ) {
		printf("TestTarget(%p)::SomethingLinkedToYou( %p )\n",this,in_pOrigin);
	}
	void SomethingUnlinkedFromYou( CLinkOrigin* in_pOrigin ) {
		printf("TestTarget(%p)::SomethingUnlinkedFromYou( %p )\n",this,in_pOrigin);
	}
};

class TestOrigin : public CLinkArray<2>
{
public:
	TestOrigin() {
		printf("TestOrigin(%p)::TestOrigin()\n",this);
	}
	~TestOrigin() {
		printf("TestOrigin(%p)::~TestOrigin()\n",this);
	}
protected:
	void vLinkConnected( CLinkTarget* in_pTarget ) {
		printf("TestOrigin(%p)::vLinkConnected( %p )\n",this,in_pTarget);
	}
	void vLinkDisconnected( CLinkTarget* in_pTarget ) {
		printf("TestOrigin(%p)::vLinkDisconnected( %p )\n",this,in_pTarget);
	}
};

//===================================================================================================

class TestNotifyTarget : public CLinkTarget
{
public:
	TestNotifyTarget() {
		printf("TestNotifyTarget(%p)::TestNotifyTarget()\n",this);
	}
	~TestNotifyTarget() {
		printf("TestNotifyTarget(%p)::~TestNotifyTarget()\n",this);
	}
protected:
	void SomethingLinkedToYou( CLinkOrigin* in_pOrigin ) {
		printf("TestNotifyTarget(%p)::SomethingLinkedToYou( %p )\n",this,in_pOrigin);
	}
	void SomethingUnlinkedFromYou( CLinkOrigin* in_pOrigin ) {
		printf("TestNotifyTarget(%p)::SomethingUnlinkedFromYou( %p )\n",this,in_pOrigin);
	}
	void NotificatorDied( CDeathNotificatorBase* in_pOrigin ) {
		printf("TestNotifyTarget(%p)::NotificatorDied( %p )\n",this,in_pOrigin);
	}
};

//===================================================================================================

int
main(int argc, char* argv[])
{
	/*
	TestOrigin TO;
	{
		TestTarget TT;
		TestTarget TT2;
		{
			TO.ConnectLink( &TT );
			TO.ConnectLink( &TT2 );
			TO.DisconnectLink( &TT );
			TO.DisconnectLink( &TT2 );
		}
	}
	*/

	/*
	{
		TestNotifyTarget TNT;
		{
			CDeathNotificator<2>* pDN = new CDeathNotificator<2>();
			pDN->ConnectLink(&TNT);

			printf("--------------\n");
			delete pDN;
			printf("--------------\n");
		}
	}
	*/


	CElementWrapper<long,2> TL;

	return 0;
}

























