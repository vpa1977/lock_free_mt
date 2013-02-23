#ifndef RENDER_INFO_HPP_INCLUDED
#define RENDER_INFO_HPP_INCLUDED

enum RENDER_DISPLAY_MODE_FORMAT
{
	RDMF_R5G6B5,
	RDMF_X8R8G8B8,
	RDMF_UNKNOWN
};

struct CRenderDisplayMode
{
	unsigned long m_Width;
	unsigned long m_Height;
	unsigned long m_Refresh;
	RENDER_DISPLAY_MODE_FORMAT m_Format;
	CRenderDisplayMode( unsigned long in_Width=0, unsigned long in_Height=0, unsigned long in_Refresh=0, RENDER_DISPLAY_MODE_FORMAT in_Format=RDMF_UNKNOWN ) :
		m_Width(in_Width)
		,m_Height(in_Height)
		,m_Refresh(in_Refresh)
		,m_Format(in_Format)
	{}
};

#endif