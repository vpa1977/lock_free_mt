#include "../Include/font_loader.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

// forward declaration
bool LoadFontFromBitmap_Internal(
	const unsigned long* in_pPixels,
	long in_Width, long in_Height,
	long in_StartSymbol,
	CFontDescription& out_rFontDescription
);
 
bool LoadFontFromBitmap(
	const std::vector<unsigned char>& in_rBitmap,
	long in_Width, long in_Height, 
	long in_StartSymbol,
	CFontDescription& out_rFontDescription
)
{
	assert( sizeof(unsigned long)*in_Width*in_Height == in_rBitmap.size() );
	return LoadFontFromBitmap_Internal( (unsigned long*)&in_rBitmap[0],in_Width,in_Height,in_StartSymbol,out_rFontDescription );
}

bool LoadFontFromBitmap(
	const std::vector<unsigned long>& in_rBitmap,
	long in_Width, long in_Height,
	long in_StartSymbol,
	CFontDescription& out_rFontDescription
)
{
	assert( in_Width*in_Height == in_rBitmap.size() );
	return LoadFontFromBitmap_Internal( &in_rBitmap[0],in_Width,in_Height,in_StartSymbol,out_rFontDescription );
}

//======================================================================================

bool LoadFontFromBitmap_Internal(
	const unsigned long* in_pPixels,
	long in_Width, long in_Height,
	long in_StartSymbol,
	CFontDescription& out_rFontDescription
)
{
	//CLog::Print("LoadFontFromBitmap_Internal()\n");
	assert(in_pPixels);
	assert(in_Width>0 && in_Height>0);
	assert(in_StartSymbol>=32);

	out_rFontDescription.m_FontHeight = in_Height;
	out_rFontDescription.m_StartSymbol = in_StartSymbol;

	out_rFontDescription.m_Symbols.clear();

	// determine glyphs widths
	unsigned long Color;
	for(long X=0; X<in_Width; ++X)
	{
		if( (0==X) || ((0xffffff&in_pPixels[X])!=Color) )
		{	// next symbol
			//if( out_rFontDescription.m_Symbols.size() > 0 )
			//{
			//	CLog::Print("    width %ld\n", out_rFontDescription.m_Symbols[ out_rFontDescription.m_Symbols.size()-1 ].m_Width );
			//}
			//CLog::Print("  symbol %lu('%c') at x-offset %ld\n",out_rFontDescription.m_Symbols.size()+in_StartSymbol,(char)(out_rFontDescription.m_Symbols.size()+in_StartSymbol),X);
			out_rFontDescription.m_Symbols.resize( out_rFontDescription.m_Symbols.size()+1 );
			out_rFontDescription.m_Symbols[ out_rFontDescription.m_Symbols.size()-1 ].m_Width = 1;
			Color = 0xffffff&in_pPixels[X];
		}
		else
		{
			out_rFontDescription.m_Symbols[ out_rFontDescription.m_Symbols.size()-1 ].m_Width++;
		}
	}

	// read glyphs images
	long XOffset = 0;
	for( size_t N=0; N<out_rFontDescription.m_Symbols.size(); ++N )
	{
		CFontDescription::CSymbol& rSymbol = out_rFontDescription.m_Symbols[N];
		rSymbol.m_SymbolGlyph.resize( rSymbol.m_Width*in_Height );
		{
			//CLog::Print("  reading %lu ('%c')\n",N+in_StartSymbol,(char)(N+in_StartSymbol));
			for(long Y=0; Y<in_Height; ++Y)
			{
				for(long X=0; X<rSymbol.m_Width; ++X)
				{
					rSymbol.m_SymbolGlyph[Y*rSymbol.m_Width+X] = in_pPixels[Y*in_Width+X+XOffset];
				}
			}
		}
		XOffset+=rSymbol.m_Width;
	}

	return true;
}












