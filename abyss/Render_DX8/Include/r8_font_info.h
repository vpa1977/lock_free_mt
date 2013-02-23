#ifndef R8_FONT_INFO_HPP_INCLUDED
#define R8_FONT_INFO_HPP_INCLUDED

#include <vector>

struct CFontDescription
{
	CFontDescription() : m_FontHeight(0) {}
	long m_FontHeight;
	long m_StartSymbol;
	struct CSymbol
	{
		long m_Width;
		// glyph image
		// must be exactly m_FontHeight*m_Width dwords
		// format - A8R8G8B8
		std::vector< unsigned long > m_SymbolGlyph;
	};
	std::vector< CSymbol > m_Symbols;
};

#endif
