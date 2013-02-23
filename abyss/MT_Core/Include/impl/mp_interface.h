#ifndef MP_INTERFACE_HPP

#define MP_INTERFACE_HPP 
#include "stdafx.h"
#include "section_constants.h"
#include "mp_manager.h"

template <typename S> 
class CTSectionFactory
{
	public:
		section* create(unsigned long id)
		{
			return new S(id);
		}
};

template< unsigned long CLASSID, typename S >
class CTSectionRegistrator
{
public:
	CTSectionRegistrator( bool in_MultipleInstancesAllowed, bool in_StartupSection = REGULAR_SECTION, unsigned long in_Flags = SF_NONE )
	{
		class CSCreator : public section_factory
		{
		public:
			section* create(unsigned long id)
			{
				return new S(id);
			}
			/*const section_descriptor& get_descriptor() const
			{
				return m_descriptor;
			}
			section_descriptor m_descriptor;*/
		};
		CSCreator* Creator = new CSCreator();
		//Creator.m_descriptor ; // init descriptor
		mp_manager::get_instance()->register_section(CLASSID, Creator);
		if (in_StartupSection) 
		{
			mp_manager::get_instance()->set_startup(CLASSID);
		}
	}
}; 


#endif