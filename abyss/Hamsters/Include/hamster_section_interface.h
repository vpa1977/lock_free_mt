#ifndef M4_SECTION_INTERFACE_HPP_INCLUDED
#define M4_SECTION_INTERFACE_HPP_INCLUDED

#define M4Section_CLSID 893051171


// ================================================================================
struct CHamster_InitMainMenuRequest : public CCommand
							   {
							   };

struct CHamster_InitMainMenuResponse : public CCommand
							   {
							   };
struct CHamster_HideMainMenuRequest : public CCommand
{
};
struct CHamster_HideMainMenuResponse : public CCommand
{
};
struct CHamster_NewGame : public CCommand
{
};



// ================================================================================

#endif












