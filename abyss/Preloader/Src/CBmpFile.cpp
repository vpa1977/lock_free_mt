
#include "../include/CBmpFile.h"

using namespace abyss::preloader;


int CBmpFile::Read(unsigned char *pIn, int size )
{
	int ret;
	

	ret = ReadHeader(pIn , size);
	if (ret < 0 ) 
	{
		return ret;
	}

	if (m_Bpp < 8 ) 
	{
		return -1;
	}

	switch (m_EncType) 
	{
		case 0: // raw data
			ret =  ReadRaw(pIn, size);
			break;
		case 1: // RLE data
			ret = ReadRLE(pIn , size);
			break;
		default:
			return -1;
	}

	//FlipImg();
	if (m_Bpp == 8 ) 
	{
		ret = LoadBmpPalette(pIn);

	}
	FlipImg();
	return ret;
}


int CBmpFile::ReadHeader(unsigned char* pIn , int size ) 
{
	if (pIn[0] != 'B' && pIn[1] != 'M' ) 
	{
		// NOT A BMP
		return -1;
	}

	memcpy(&m_ImageOffset,&pIn[10],sizeof(int));   // Offset to image data
	   
	memcpy(&m_InfoSize,&pIn[14],sizeof(int));
   if(m_InfoSize!=0x28)
   {
    return -1;
   }
 
  memcpy(&m_Width,&pIn[18],4);   // Image width
  memcpy(&m_Height,&pIn[22],4);  // Image height
  memcpy(&m_Planes,&pIn[26],2);  // Colour planes
  memcpy(&m_Bpp,&pIn[28],2);     // BPP
  memcpy(&m_EncType,&pIn[30],4);     // Encoding
 
  m_DataSize=(m_Width*m_Height*(m_Bpp/8)); // Calculate Image Data size

  m_ImgData.resize(m_DataSize);	
}

int CBmpFile::ReadRaw(unsigned char* pIn, int size)
{
	memcpy( (char *) &(m_ImgData[0]) , (char*)(pIn+m_ImageOffset) , m_DataSize);
	return 1;
}

int CBmpFile::ReadRLE(unsigned char* pIn, int size)
{
	unsigned char operation;
	unsigned char val;
    int idCode=1;
	int count;
	int pos;
	int index;
	unsigned char *pSrc;
 
  // Get the start of the RLE data
	pSrc=& (pIn[m_ImageOffset]);
 
  pos=0;
  index=0;
 
   while(idCode)
    {
      // Stay on even bytes
      while(pos%2)
       {
        pos++;
       }
 
     operation=pSrc[pos];
     val=pSrc[pos+1];
     pos+=2;
 
      if(operation>0) // Run mode, repeat val (operation )
       {
         for(int i=0;i<operation ;i++)
          {
		   m_ImgData[index]=val;
           index++;
          }
       }
      else // Absolute Mode (Opcode=0), various options
       {
         switch(val)
          {
           case 0:  // EOL, no action
            break;
 
           case 1:  // EOF, STOP!
             idCode=0;
            break;
 
           case 2:  // Reposition, Never used
            break;
 
           default: // Copy the next 'val' bytes directly to the image
             for(int i = 0 ; i < val ; i ++ )
              {
			   m_ImgData[index]=pSrc[pos];
               index++;
               pos++;
              }
            break;
          }
 
	}
      if(index>m_DataSize) // Stop if image size exceeded.
       idCode=0;
    }
	return 0;
}



int CBmpFile::LoadBmpPalette(unsigned char* pIn)
 {

   unsigned char pPalette[768];
   unsigned char* pPalPos;
   unsigned char* pDatPos;
 
  // Set starting position for pointers
  pPalPos=pPalette;
  pDatPos=&pIn[54];
 
  // Get colour values, skip redundant 4th value
   for(int iIndex=0;iIndex!=256;++iIndex)
    {
     pPalPos[0]=pDatPos[2]; // Red
     pPalPos[1]=pDatPos[1]; // Green
     pPalPos[2]=pDatPos[0]; // Blue
 
     pPalPos+=3;
     pDatPos+=4;
    }
	std::vector<unsigned char> buf;
	buf.resize(m_ImgData.size() * 3);

	int count = 0;
	for (int i = 0 ; i < m_ImgData.size() ; i ++ ) 
	{
		buf[count] = pPalette[m_ImgData[i]*3+2];count ++;
		buf[count] = pPalette[m_ImgData[i]*3+1];count ++;
		buf[count] = pPalette[m_ImgData[i]*3];count ++;
	}
	m_ImgData = buf;


	 return 1;
 }
 
 
void CBmpFile::FlipImg()
 {
  unsigned char bTemp;
  unsigned char *pLine1, *pLine2;
  int iLineLen,iIndex;
 
  iLineLen=m_Width*(m_Bpp/8);
  pLine1= & (m_ImgData[0]);
  pLine2=& (m_ImgData[iLineLen * (m_Height - 1)]);
 
   for( ;pLine1<pLine2;pLine2-=(iLineLen*2))
    {
     for(iIndex=0;iIndex!=iLineLen;pLine1++,pLine2++,iIndex++)
      {
       bTemp=*pLine1;
       *pLine1=*pLine2;
       *pLine2=bTemp;       
      }
    } 
 
 }
 
