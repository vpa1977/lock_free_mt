#ifndef CBMPFILE_HPP

#define CBMPFILE_HPP

#include <vector>

namespace abyss
{

	namespace preloader {
		class CBmpFile 
		{
			public:
				int Read(unsigned char* pIn, int size);
				
				std::vector<unsigned char> m_ImgData;
				unsigned int m_Width,m_Height;
			private:
				int ReadHeader(unsigned char* pIn , int size );
				void FlipImg();
				int LoadBmpPalette(unsigned char* pIn);
				int ReadRaw(unsigned char* pIn , int size );
				int ReadRLE(unsigned char* pIn , int size );

				short int m_Bpp, m_Planes; 
				unsigned int m_EncType;
				int m_ImageOffset,m_InfoSize,m_DataSize;

				
		};
	}
}

#endif