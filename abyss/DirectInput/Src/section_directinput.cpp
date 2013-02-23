#include "../Include/section_directinput.h"



void Touch_DirectInputSection() {};


//..static CTSectionRegistrator<CDirectInputSection_CLSID,CDirectInputSection> g_CDirectInputRegistrator(MULTIPLE_INSTANCES_ALLOWED);




CDirectInputSection::CDirectInputSection(long sectionID ) : CSection(sectionID)
{
	
}
CDirectInputSection::~CDirectInputSection()
{

}
