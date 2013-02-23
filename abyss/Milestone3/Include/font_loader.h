#ifndef FONT_LOADER_HPP_INCLUDED
#define FONT_LOADER_HPP_INCLUDED

#include "../../Render_DX8/Include/r8_font_info.h"
#include <vector>

/*

INPUT:
	Image, and it's dimensions.
	Upper row of pixels has special meaning:
		when it's rgb value changes - it means that here begins next symbol.

OUTPUT:
	Filled CFontDescription structure.

*/

bool LoadFontFromBitmap(
	const std::vector<unsigned char>& in_rBitmap,
	long in_Width, long in_Height,
	long in_StartSymbol,
	CFontDescription& out_rFontDescription
);

bool LoadFontFromBitmap(
	const std::vector<unsigned long>& in_rBitmap,
	long in_Width, long in_Height,
	long in_StartSymbol,
	CFontDescription& out_rFontDescription
);

#endif




















