// prototype1.cpp : Defines the entry point for the console application.
//

#include "../Include/impl/stdafx.h"

#include "../Include/impl/publishers.h"
#include "../Include/impl/mp_manager.h"
#include "../Include/impl/system_section.h"
#include "../Include/impl/command.h"

#include <vector>
#include <boost/shared_ptr.hpp>

struct TestCommandA :public abstract_command
{
	std::vector<int> hi;
	boost::shared_ptr< std::vector<int> > ptrHI;
};
struct TestCommandB:public abstract_command
{
};

#pragma pack(8)
class CMatrix
{
public:
	float m[16]; // First element is a upper left, last one is a lower right.
};
struct ChainCommand : public abstract_command 
{
		long m_X;
	long m_Y;
	std::string m_String;
	unsigned long m_FontHandle;
	unsigned long m_Color;

	CMatrix m_M;
	unsigned long m_VBHandle;
	unsigned long m_PixelShaderHandle;
	unsigned long m_FirstVertex;
	unsigned long m_PrimitiveType;
	unsigned long m_PrimitiveCount;
};

std::vector<int> global_vector;

class test_section_a : public section, 
	public CTCommandSender<TestCommandA>,
	public CTCommandSender<TestCommandB>,
	public CTCommandSender<CCommand_LaunchSection>,
	public CTCommandSender<ChainCommand>

{
public:
	test_section_a(long id) : section(id)
	{
		RegisterReaction( GetID( (CCommand_SectionStartup*)NULL), CTConform<test_section_a, CCommand_SectionStartup>::Conform);
		RegisterReaction( GetID( (TestCommandA*) NULL ) , CTConform<test_section_a, TestCommandA>::Conform);
		REGISTER_REACTION(test_section_a, CCommand_LaunchSectionResult);
	}

	void Reaction( long srcSectionID , const CCommand_SectionStartup& in_rStartup) 
	{
		CTCommandSender<CCommand_LaunchSection>::SendCommand(SYSTEM_SECTION_ID , 
			CCommand_LaunchSection(222, 0));
	
	}



	void Reaction(long srcSectionID , const CCommand_LaunchSectionResult& in ) 
	{
		secB = in.m_NewSectionID;
		TestCommandA a;
		//a.hi = global_vector;
		//a.ptrHI = boost::shared_ptr< std::vector<int> >( new std::vector<int>(global_vector.begin() , global_vector.end() ));
		CTCommandSender<TestCommandA>::SendCommand( GetThisID() , a);
	}
	long secB;

	void Reaction( long srcSectionID , const TestCommandA& in_rStartup) 
	{
		//printf("Test Command A recieved\n");
		for (int i  = 0 ; i < 1 ; i ++ ) 
		{
			ChainCommand c;
			c.m_String = std::string("ZAEBIS");
			CTCommandSender<ChainCommand>::SendCommand( secB , c);
		}
		CTCommandSender<TestCommandB>::SendCommand( secB , TestCommandB());
		//CTCommandSender<TestCommandB>::SendCommand(2, TestCommandB());
	}

};

class test_section_b : public section, 
	public CTCommandSender<TestCommandA>
{
	public:
	float m_clock;
	long m_count;
	test_section_b(long id) : section(id)
	{
		RegisterReaction( GetID( (CCommand_SectionStartup*)NULL), CTConform<test_section_b, CCommand_SectionStartup>::Conform);
		RegisterReaction( GetID( (TestCommandB*) NULL ) , CTConform<test_section_b, TestCommandB>::Conform);
		RegisterReaction( GetID( (ChainCommand*) NULL )  , CTConform<test_section_b, ChainCommand>::Conform);
	}

	void Reaction(long in , const ChainCommand& c ) 
	{

	}
	void Reaction( long srcSectionID , const CCommand_SectionStartup& in_rStartup) 
	{
		m_clock = clock();
	}

	void Reaction( long srcSectionID , const TestCommandB& in_rStartup) 
	{
		TestCommandA a;
		//a.hi = global_vector;
		//a.ptrHI = boost::shared_ptr< std::vector<int> >( new std::vector<int>(global_vector.begin() , global_vector.end() ));
		CTCommandSender<TestCommandA>::SendCommand( srcSectionID , a);
		//printf("DOING \n");		
//		CTCommandSender<TestCommandA>::SendCommand(srcSectionID, TestCommandA());
	}

};

static CTSectionRegistrator<111, test_section_a> a(false, true);
static CTSectionRegistrator<222, test_section_b> b(false);





int main(int argc, char* argv[])
{
		for (int i = 0 ; i < 1000000 ; i ++ ) 
		{
			global_vector.push_back(i);
		}
	
		mp_manager::get_instance()->run_system();
	return 0;
	

}

