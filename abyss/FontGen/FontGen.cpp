#include <windows.h>
#include <stdio.h>
#include <assert.h>

struct qq  {long qqq; long qqqq;};
qq g_data[] = {
	{1,2},
	{3,4}
};

long g_FontHeight = 0;
struct CGlyphInfo
{
	CGlyphInfo() : m_X(0),m_Y(0),m_Width(0) {}
	long m_X;
	long m_Y;
	long m_Width;
};
CGlyphInfo g_GlyphInfos[256];

#define TEXTURE_SIZE 256
bool g_TextureData[TEXTURE_SIZE][TEXTURE_SIZE];

#define WHITE	0xffffff
#define YELLOW	0xffff00
#define RED		0xff0000

unsigned long ConstructDword(bool*);

int
main(int argc, char* argv[])
{
#if 0
	printf("---------------------------------------------------------------------------\n");
	for(int c=32; c<128; ++c)
	{
		printf("%c",c);
	}
	printf("\n---------------------------------------------------------------------------\n");
#else
	FILE* fin = fopen("font.bmp","rb");
	assert(fin);

	BITMAPFILEHEADER FileHeader;
	size_t nRead = fread( &FileHeader, 1, sizeof(BITMAPFILEHEADER), fin );
	assert( sizeof(BITMAPFILEHEADER) == nRead );

	assert( FileHeader.bfType == 19778 ); // 'BM'
	unsigned long ImageOffset = FileHeader.bfOffBits;
	assert( ImageOffset >= sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) );

	BITMAPINFO BitmapInfo;
	nRead = fread( &BitmapInfo, 1, sizeof(BITMAPINFO), fin );
	assert( sizeof(BITMAPINFO) == nRead );

	assert( BitmapInfo.bmiHeader.biWidth == TEXTURE_SIZE );
	assert( BitmapInfo.bmiHeader.biHeight == TEXTURE_SIZE );
	assert( BitmapInfo.bmiHeader.biPlanes == 1 );
	assert( BitmapInfo.bmiHeader.biBitCount == 24 );
	assert( BitmapInfo.bmiHeader.biCompression == BI_RGB );
	unsigned long RowSizeBytes = BitmapInfo.bmiHeader.biWidth*3;
	assert( (RowSizeBytes%4) == 0 );

	bool bHaveFontHeight = false;
	long NextChar = 32; // chars 0..31 are not in font
	for(long Row=0; Row<BitmapInfo.bmiHeader.biHeight; ++Row)
	{
		long RowReverse = BitmapInfo.bmiHeader.biHeight-1-Row;
		int r = fseek( fin, ImageOffset+RowSizeBytes*RowReverse, SEEK_SET );
		assert(0==r);
		bool bExpectingControlRow =
			(0==Row)
			|| ( bHaveFontHeight && (0==(Row%g_FontHeight)) );
		bExpectingControlRow = bExpectingControlRow && (NextChar<128);
		if(bExpectingControlRow)
		{
			bool bReadingGlyph = false;
			for(long Col=0; Col<BitmapInfo.bmiHeader.biWidth; ++Col)
			{
				g_TextureData[Row][Col] = false;
				unsigned long Pixel=0;
				size_t nRead = fread(&Pixel,1,3,fin);
				assert(3==nRead);
				if(RED==Pixel)
				{
					if( bReadingGlyph )
					{
						g_GlyphInfos[NextChar].m_Width = Col - g_GlyphInfos[NextChar].m_X;
						NextChar++;
						bReadingGlyph = false;
					}
				}
				else if(YELLOW==Pixel)
				{
					if( !bReadingGlyph )
					{
						g_GlyphInfos[NextChar].m_X = Col;
						g_GlyphInfos[NextChar].m_Y = Row;
						bReadingGlyph = true;
					}
				}
				else
				{
					assert(false);
				}
			}
			assert( !bReadingGlyph );
		}
		else
		{
			bool bControlRow = false;
			bool bReadingGlyph = false;
			for(long Col=0; Col<BitmapInfo.bmiHeader.biWidth; ++Col)
			{
				unsigned long Pixel=0;
				size_t nRead = fread(&Pixel,1,3,fin);
				assert(3==nRead);

				if( !bHaveFontHeight && 0==Col && ( RED==Pixel || YELLOW==Pixel ) )
				{
					assert(RED==Pixel);
					if(!bHaveFontHeight)
					{
						g_FontHeight = Row;
						bHaveFontHeight = true;
						bControlRow = true;
					}
				}

				if(bControlRow)
				{
					g_TextureData[Row][Col] = false;
					if(RED==Pixel)
					{
						if( bReadingGlyph )
						{
							g_GlyphInfos[NextChar].m_Width = Col - g_GlyphInfos[NextChar].m_X;
							NextChar++;
							bReadingGlyph = false;
						}
					}
					else if(YELLOW==Pixel)
					{
						if( !bReadingGlyph )
						{
							g_GlyphInfos[NextChar].m_X = Col;
							g_GlyphInfos[NextChar].m_Y = Row;
							bReadingGlyph = true;
						}
					}
					else
					{
						assert(false);
					}
				}
				else
				{
					if(WHITE==Pixel)
						g_TextureData[Row][Col] = true;
					else
						g_TextureData[Row][Col] = false;
				}
			}
			assert( !bReadingGlyph );
		}
	}
	assert(128==NextChar);

	fclose(fin);

	FILE* fout = fopen("font_data.cpp","wt");
	assert(fout);

	//fprintf(fout,"struct CGlyphInfo\n");
	//fprintf(fout,"{\n");
	//fprintf(fout,"\tlong m_X;\n");
	//fprintf(fout,"\tlong m_Y;\n");
	//fprintf(fout,"\tlong m_Width; // m_Width==0 means that this symbol is unused\n");
	//fprintf(fout,"}\n");
	fprintf(fout,"const long g_FontHeight = %ld;\n",g_FontHeight);
	fprintf(fout,"CSymbolInfo g_FontData[] = {\n");
	for(int i=0; i<256; ++i )
	{
		fprintf(fout,"\t{%f,%f,%f}",(float)g_GlyphInfos[i].m_X,(float)g_GlyphInfos[i].m_Y,(float)g_GlyphInfos[i].m_Width);
		if( i>=32 && i<=127 )
			fprintf(fout,",\t// '%c\'\n",(char)i);
		else
		{
			if( i!=255 )
				fprintf(fout,",\n");
			else
				fprintf(fout,"\n");
		}
	}
	fprintf(fout,"};\n\n");

	fprintf(fout,"#define FONT_TEXTURE_SIZE 256\n");
	fprintf(fout,"unsigned long g_FontTexture[] = {\n");
	for(Row=0; Row<TEXTURE_SIZE; ++Row)
	{
		bool* pBools = &(g_TextureData[Row][0]);
		unsigned long Data = ConstructDword(pBools);
		fprintf(fout,"\t0x%08x, ",Data);
		
		pBools = &(g_TextureData[Row][32]);
		Data = ConstructDword(pBools);
		fprintf(fout,"0x%08x, ",Data);
		
		pBools = &(g_TextureData[Row][64]);
		Data = ConstructDword(pBools);
		fprintf(fout,"0x%08x, ",Data);
		
		pBools = &(g_TextureData[Row][96]);
		Data = ConstructDword(pBools);
		fprintf(fout,"0x%08x, ",Data);
		
		pBools = &(g_TextureData[Row][128]);
		Data = ConstructDword(pBools);
		fprintf(fout,"0x%08x, ",Data);
		
		pBools = &(g_TextureData[Row][160]);
		Data = ConstructDword(pBools);
		fprintf(fout,"0x%08x, ",Data);
		
		pBools = &(g_TextureData[Row][192]);
		Data = ConstructDword(pBools);
		fprintf(fout,"0x%08x, ",Data);
		
		pBools = &(g_TextureData[Row][224]);
		Data = ConstructDword(pBools);

		if(Row==TEXTURE_SIZE-1)
			fprintf(fout,"0x%08x\n",Data);
		else
			fprintf(fout,"0x%08x,\n",Data);
	}
	fprintf(fout,"};\n");

	fclose(fout);
#endif
	return 0;
}

unsigned long
ConstructDword(bool* pB)
{
	assert(pB);

	unsigned long Res=0;
	for(unsigned long i=0; i<32; ++i)
	{
		if(pB[i])
			Res = Res | (1<<i);
	}

	return Res;
}
















