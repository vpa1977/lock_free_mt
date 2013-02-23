#include "../include/vorbis_decoder.h"



#include "../include/ogg.h"
#include <vector>





#define WAVE_SIZE 65535

// MFILE -- acts like a FILE, but for memory.
typedef struct {unsigned int pos,size; const unsigned char *buf;} MFILE;
MFILE *mopen(const unsigned char *buf,unsigned int size)
{ MFILE *m = new MFILE;
m->size=size; m->buf=buf; m->pos=0;
return m;
}
void mclose(MFILE *m)
{ delete m;
}
size_t mread_ogg(void *ptr,size_t size,size_t nmemb,void *src)
{ MFILE *m = (MFILE*)src; if (m==0) return 0;
if (m->pos>m->size) return 0;
unsigned int wanted = (unsigned int)size*nmemb;
unsigned int actual = wanted; if (m->pos+actual>m->size) actual=m->size-m->pos;
memcpy(ptr,m->buf+m->pos,actual);
m->pos += actual;
return actual;
}
int mseek_ogg(void *src,__int64 offset,int whence)
{ MFILE *m = (MFILE*)src; if (m==0) return -1;
if (whence==SEEK_CUR) {m->pos+=(unsigned int)offset; return 0;}
else if (whence==SEEK_END) {m->pos=m->size+(unsigned int)offset; return 0;}
else if (whence==SEEK_SET) {m->pos=(unsigned int)offset; return 0;}
else return -1;
}
int mclose_ogg(void *src)
{ MFILE *m = (MFILE*)src; if (m==0) return EOF;
mclose(m); return 0;
}
long mtell_ogg(void *src)
{ MFILE *m=(MFILE*)src; if (m==0) return -1;
return m->pos;
}
const ov_callbacks mfile_callbacks = {mread_ogg,mseek_ogg,mclose_ogg,mtell_ogg};


/** 
	A wrapper around vorbis decoder to feed to directmusic loader
*/

long VorbisFile::GetLength()
{
	return m_Len;
}
long VorbisFile::GetFreq()
{
	return m_Freq;
}
long VorbisFile::GetChannels()
{
	return m_Channels;
}
int VorbisFile::Open(const std::vector<unsigned char>& data)
{
	m_Data= data;
	MFILE *min = mopen((const unsigned char*)&m_Data[0],data.size());
	
	int res=ov_open_callbacks(min,&vf,NULL,0,mfile_callbacks);
	if (res<0) {mclose(min);  return -1;}
	// note that, if ov_open succeeds, then vf has ownership of the file (be it
	// disk or resource) and will be responsible for closing it.
	vorbis_info *vi=ov_info(&vf,-1);
	if (!vi) return -1;
	m_Freq = vi->rate;
	m_Channels = vi->channels;
	const unsigned int extra=10000;
	unsigned int numsamples = extra+(unsigned int)ov_pcm_total(&vf,0);
	m_Len = numsamples*vi->channels*2; // 16bit output wav

	// fill the header information
	unsigned int wavsize = sizeof(WavChunk);
	m_Header.resize(sizeof(WavChunk));
	WavChunk *wc = (WavChunk*)&m_Header[0];
	// this following is just the layout of a WAV file.
	strncpy(wc->id,"RIFF",4);
	wc->size = 4 + sizeof(FmtChunk)+sizeof(DataChunk)+m_Len;
	strncpy(wc->type,"WAVE",4);
	strncpy(wc->fmt.id,"fmt ",4);
	wc->fmt.size=16;
	wc->fmt.wFormatTag=1;
	wc->fmt.wChannels = (unsigned short)vi->channels;
	wc->fmt.dwSamplesPerSec = vi->rate;
	wc->fmt.wBitsPerSample = 16;
	wc->fmt.wBlockAlign = (unsigned short)(wc->fmt.wChannels * wc->fmt.wBitsPerSample/8);
	wc->fmt.dwAvgBytesPerSec = wc->fmt.wBlockAlign * wc->fmt.dwSamplesPerSec;
	strncpy(wc->dat.id,"data",4);
	wc->dat.size = m_Len;
}

const std::vector<char>& VorbisFile::GetHeader(){
	return m_Header;
}




int VorbisFile::Read(void* pBuf , int size )
{
	int current_section=0;
	int pos = (int)ov_pcm_tell(&vf);
	if (size+pos > m_Len ) {
		size = m_Len  - pos;
	}
	int read = 0;
	while (read < size ) 
	{
		int res = ov_read(&vf,(char*)((char*)pBuf +read), size-read, 0,2,1, &current_section);
		if (res <=0 )
		{
			return read;
		}
		read+= res;
	}
	return read;
}
void VorbisFile::Seek(int pos )
{
	// convert byte to sample position - 
	pos = pos / (2*m_Channels);
	ov_pcm_seek(&vf, pos);
}
