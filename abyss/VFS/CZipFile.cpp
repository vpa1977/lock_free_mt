#include "./include/czipfile.h"



#include <io.h>
#include <stdio.h>
#include <direct.h>
#include "../zlib/include/zlib.h"




#define COMPRESSION_LEVEL 4

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif


void CArchZipFile::Create(const char * dir, const char* archName)
{

	m_Header.clear();
	BuildList(dir,NULL);
	WriteArchive(dir, archName);
}

void CArchZipFile::WriteArchive(const char * dir, const char* archName)
{
	FILE * chunkStreamFile = tmpfile();
	
	long offset = 0; // offset in chunkStreamFile	

	int size = (int) m_Header.size();
	if (size == 0 ) 
	{
		printf("No files found to archive" );
		return;
	}
	std::map<std::string, CHeaderEntry>::iterator it = m_Header.begin();

	while (it != m_Header.end())
	{
		// process header entries

		CHeaderEntry& entry = m_Header[it->first];
		it++;

		std::string realFileName = dir + entry.m_FileName;
		FILE* entryFile = fopen(realFileName.data() , "rb");
		int flush = Z_FINISH;
		char buffer[VFS_REAL_CHUNK];
		char bufferOut[5*VFS_REAL_CHUNK];
		
		long size = VFS_REAL_CHUNK; // default chunk size
		long realLength = VFS_REAL_CHUNK; // default chunk size
		long realOffset = 0; // offset in entryFile.
		int nChunk = 0;
		while (!feof(entryFile))
		{
			memset(buffer, 0, VFS_REAL_CHUNK);
			memset(bufferOut, 0 , VFS_REAL_CHUNK);
			CChunkDesc desc;
			realLength = (long)fread(buffer,1, VFS_REAL_CHUNK, entryFile);
			if (realLength > 0 ) 
			{
				
				int ret;
	
				uLongf destLen = 10*VFS_REAL_CHUNK;
				ret = compress2( (Bytef*)bufferOut , &destLen , (Bytef*)buffer,realLength, COMPRESSION_LEVEL);
				if (ret != Z_OK)
				{
					printf("Funneh");
					return;
				}
				int have = destLen;
				// 
				//
				desc.m_Offset = offset;
				desc.m_RealLength = realLength;
				desc.m_Size = have;
				desc.m_RealOffset = realOffset;
				//
				//
				offset += have;
				fwrite(bufferOut , 1, have, chunkStreamFile);

			}
			realOffset += realLength;

			entry.m_ChunkDesc.push_back(desc);
	
			if (nChunk > 0)
	
			if (entry.m_ChunkDesc[nChunk-1].m_RealOffset + entry.m_ChunkDesc[nChunk-1].m_RealLength != 
				entry.m_ChunkDesc[nChunk].m_RealOffset) 
			{
						CChunkDesc& d1 =  entry.m_ChunkDesc[nChunk-1];
					CChunkDesc& d2 = entry.m_ChunkDesc[nChunk];
					printf("FUNNNEH");
			}
			nChunk++;
			entry.m_nChunks  = nChunk;
		
		}
		fclose(entryFile);
	}
	// 
	FILE * test = fopen(archName, "wb");
	//long size = m_Header.size();
	if (!test || ferror(test))
	{
		printf("Error opening output archive file\n");
		_rmtmp();
		return;
	}



	fwrite(&size , sizeof(long),1 , test);
	 it = m_Header.begin();
	 while (it != m_Header.end()) 
	{
		CHeaderEntry& entry = m_Header[it->first];
		it++;
		entry.Write(test);
	}
	//fclose(chunkStreamFile);
	//chunkStreamFile = fopen("c:/temp.dat","rb");
	fseek(chunkStreamFile, 0 , SEEK_SET);

	
	char buffer[4096];
	size_t read = 0;
	do
	{
		read = fread(buffer, 1, 4096, chunkStreamFile);
		fwrite(buffer, 1, read, test);
	} while (read > 0);

	 fclose(chunkStreamFile);

	 fflush(test);
	 fclose(test);
	 _rmtmp();
}


void CArchZipFile::BuildList(const char* dir, const char* sub)
{
	intptr_t result = 0;
	char buffer[1024];
	memset(buffer, 0, 1024);
	_finddata_t findData;
	strcpy(buffer, dir);
	strcat(buffer, "/*.*");

	result = _findfirst(buffer, &findData);
	if (result < 0 ) 
	{
		return;
	}
	do 
	{

		if (!strcmp(findData.name, ".." ))
		{
			continue;
		}

		if (!strcmp(findData.name, "." ))
		{
			continue;
		}

		if (findData.attrib & _A_SUBDIR ) 
		{
			memset(buffer,0,1024);
			strcpy(buffer, dir);
			strcat(buffer, "/");
			strcat(buffer,findData.name);
			std::string supp;
			if (sub)
			{
				supp +=sub;
			}
			supp +="/";
			supp += findData.name;
			BuildList(buffer, supp.data());
		}
		else
		{
			CHeaderEntry entry;
			if (sub ) 
			{
				entry.m_FileName = sub;
				entry.m_FileName += "/";
			}
			else
			{
				entry.m_FileName = "/";
			}
			
			entry.m_FileName += findData.name;
			
			m_Header[entry.m_FileName] =entry;
		}
	} while (!_findnext(result,&findData));
	_findclose(result);
}

void CArchZipFile::Open(const char* archName)
{
	m_Header.clear();
	m_File = fopen(archName , "rb");
	long headerSize = 0;
	fread(&headerSize , sizeof(long ) , 1, m_File );
	for (int i = 0 ; i < headerSize ; i ++ ) 
	{
		CHeaderEntry entry;
		entry.Read(m_File);
		m_Header[entry.m_FileName] = entry;
	}
	fpos_t pos;
	fgetpos(m_File, &pos);
	m_DataOffset =(unsigned long) pos;
}

void CArchZipFile::Close()
{
	fclose(m_File);
	m_File = NULL;
}



int CArchZipFile::decompressChunk(char* outBuffer, FILE * f, CChunkDesc& desc ) 
{
	int ret;


    char* in;

	in = (char*)malloc(desc.m_Size);
	fseek(f, m_DataOffset + desc.m_Offset, SEEK_SET);

	uLong len = (uLong)fread(in, 1, desc.m_Size, f);
    if (ferror(f)) {
        return Z_ERRNO;
    }
	uLongf destLen = desc.m_RealLength;
	ret = uncompress((Bytef*)outBuffer, &destLen, (const Bytef *)in, len); 
	free(in);
	if (ret != Z_OK)
	{
		printf("FUnneh");
		return -1;
	}
	return destLen;
}

int CArchZipFile::Read(CHeaderEntry* entry , void * buffer , unsigned long offset, unsigned long size ) 
{
	
	bool chunkFound = false;
	long i = 0;
	for (i = 0 ; i < entry->m_nChunks ; i ++ ) 
	{
		CChunkDesc& desc = entry->m_ChunkDesc[i];
		if (desc.m_RealOffset <= offset && desc.m_RealOffset + desc.m_RealLength > offset ) 
		{
			chunkFound = true;
			break;
		}
	}
	if (!chunkFound ) 
	{
		return 0;
	}
	// copy
	char* chunkBuffer;

	unsigned int bufPos = 0;
	bool firstChunk = true;
	while (bufPos < size ) 
	{
		CChunkDesc& desc = entry->m_ChunkDesc[i];
		chunkBuffer = (char*)malloc(desc.m_RealLength);
		memset(chunkBuffer, 0 , desc.m_RealLength);
		// read chunks until either eof ecountered or offset reached. 
		int ret = decompressChunk(chunkBuffer, m_File , desc);
		if (ret < 0 ) 
		{
			// failed to decompress chunk
			
			return 0;
		}
		
		
		int bufferOffset = offset - desc.m_RealOffset + bufPos; // (should be 0 for subseqent chunks )
		if (!firstChunk) 
		{
			bufferOffset = 0;
		}
		long currentChunkUnread = ret - bufferOffset;
		

		long unread = size - bufPos; // mean we are choosing buffer offset for the first processed chunk
		long sizeToCopy = min(currentChunkUnread, unread);
		
		firstChunk = false;

		if (sizeToCopy < 0 ) 
		{
			return 0;
		}
		memcpy((char*)((char*)buffer+bufPos), (char*)(chunkBuffer +bufferOffset),sizeToCopy );
		free(chunkBuffer);
		bufPos += sizeToCopy;
		i++;
		if (i >= entry->m_nChunks)
		{
			// chunks exausted
			return bufPos;
		}

	}
	return bufPos;
}

int CArchZipFile::Read(const char * file , void * buffer , unsigned long offset, unsigned long size ) 
{
	
	std::map<std::string, CHeaderEntry>::iterator it= m_Header.find(file);
	if (it == m_Header.end())
	{
		return 0;
	}
	CHeaderEntry entry = it->second;
	return Read( &entry, buffer, offset, size);

/// some ascii graphics :)
///
///
///   realOffset      offset           realOffset 
///   |               |                |
///    pos = offset - realOffset 
///    pos =  

		


}

void CArchZipFile::Decompress(const char* archName, const char* dir )
{
	Open(archName);
	

	std::map<std::string, CHeaderEntry>::iterator it= m_Header.begin();
	while (it != m_Header.end())
	{
		
		std::string nextFile = dir;
		char buffer[VFS_REAL_CHUNK];
		nextFile += m_Header[it->first].m_FileName;
		std::string dir= "";
		std::string temp = nextFile;
		char* part = strtok((char*)temp.data(), "/");
		char* lastpart =NULL;
		do 
		{
			lastpart = part;
			part = strtok(NULL, "/");
			if (part ) {
				dir += lastpart;;
				dir +="/";
			}
			_mkdir(dir.data());
		}while (part != NULL );
		
		


		FILE * outFile = fopen(nextFile.data(), "wb");
		
		if (ferror(outFile))
		{
			printf("Funneh !");
		}
		int res = 0;
		int offset = 0;
		do {
			res = Read(m_Header[it->first].m_FileName.data(), buffer, offset, VFS_REAL_CHUNK);
			if (res > 0 ) 
			{
				fwrite(buffer, sizeof(char),res,outFile);
				offset += VFS_REAL_CHUNK;
			}
		}
		while (res == VFS_REAL_CHUNK);	
		fflush(outFile);
		fclose(outFile);
		outFile = NULL;
		it++;
	}
}

//////
void CHeaderEntry::Write(FILE * f) 
{
	unsigned char terminator = 0;
	fprintf(f, "%s",m_FileName.data());

	fwrite(&terminator, 1 , 1, f);// zero byte terminator
	fwrite(&m_nChunks, sizeof(long) , 1, f);
	//fprintf(f, "%d",);
	for (unsigned int i = 0 ; i < m_ChunkDesc.size() ; i ++ ) 
	{
		CChunkDesc desc = m_ChunkDesc[i];

		fwrite(& (desc.m_Offset), sizeof(long) , 1, f);
		fwrite(& (desc.m_Size), sizeof(long) , 1, f);
		fwrite(& (desc.m_RealOffset), sizeof(long) , 1, f);
		fwrite(& (desc.m_RealLength), sizeof(long) , 1, f);
	}

}

void CHeaderEntry::Read(FILE * f)
{
	char buffer[1024];
	memset(buffer, 0 , 1024);
	m_ChunkDesc.clear();
	int i = -1;
	do 
	{
		i++;
		buffer[i] = fgetc(f);
	} while (buffer[i] != 0 );
	
	m_FileName = buffer;
	fread(&m_nChunks, sizeof(long) , 1, f);
	//fprintf(f, "%d",);
	for (long i = 0 ; i < m_nChunks ; i ++ ) 
	{
		CChunkDesc desc;
		fread(& desc.m_Offset, sizeof(long) , 1, f);
		fread(& desc.m_Size, sizeof(long) , 1, f);
		fread(& desc.m_RealOffset, sizeof(long) , 1, f);
		fread(& desc.m_RealLength, sizeof(long) , 1, f);
		m_ChunkDesc.push_back(desc);
	}
	if (ferror(f))
	{
		printf("error came here ");
		if (feof(f))
		{
			printf("eof");
		}
	}

}
//////

void CZZipFile::Open(const char* archName)
{
	// 
	ZZIP_DIRENT entry;

    m_DIR = zzip_dir_open(archName, NULL);
	while (zzip_dir_read(m_DIR,&entry))
	{
		// do some fun.
		if (entry.d_csize > 0 ) 
		{
			std::string slash = "/";
			slash +=entry.d_name;
			CHeaderEntry header;
			header.m_FileName = slash;
			
			m_Header[slash] = header;

			CZHeaderEntry zHeader;
			zHeader.m_FileName = entry.d_name;
			zHeader.m_DirEnt = entry;

			m_ZHeader[slash] = zHeader;

		}
		
	}
}

bool CZZipFile::IsOpen()
{
	return m_DIR!= NULL;
} 
void CZZipFile::Close()
{
	zzip_closedir(m_DIR);
}

int CZZipFile::Read(const char* file, void * buffer ,unsigned long offset, unsigned long size){
	std::map<std::string, CZHeaderEntry>::iterator it = m_ZHeader.find(file);
	if (it!= m_ZHeader.end())
	{
		ZZIP_FILE* f = zzip_file_open(m_DIR, file,O_BINARY);
		zzip_seek(f, offset, SEEK_SET);
		long read = zzip_read(f, (char*)buffer, size);
		zzip_fclose(f);
		return read;
	}
	return -1;
}
int CZZipFile::Read(CHeaderEntry* entry, void * buffer ,unsigned long offset, unsigned long size){
	std::map<std::string, CZHeaderEntry>::iterator it = m_ZHeader.find(entry->m_FileName);
	if (it!= m_ZHeader.end())
	{
		ZZIP_FILE* f = zzip_file_open(m_DIR, it->second.m_FileName.data(),O_BINARY);
		zzip_seek(f, offset, SEEK_SET);
		long read = zzip_read(f, (char*)buffer, size);
		zzip_fclose(f);
		return read;
	}
	return -1;
}

bool CZZipFile::Exists(const char* resource ){
	std::map<std::string, CZHeaderEntry>::iterator it = m_ZHeader.find(resource);
	if (it!= m_ZHeader.end())
	{
		
		return true;
	}
	return false;

}
long CZZipFile::SizeOf(const char* resource){
	std::map<std::string, CZHeaderEntry>::iterator it = m_ZHeader.find(resource);
	if (it!= m_ZHeader.end())
	{
		ZZIP_DIRENT dirent = it->second.m_DirEnt;
		return dirent.st_size;
	}
	return 0;
}
long CZZipFile::SizeOf(CHeaderEntry* entry){

	std::map<std::string, CZHeaderEntry>::iterator it = m_ZHeader.find(entry->m_FileName);
	if (it!= m_ZHeader.end())
	{
		ZZIP_DIRENT dirent = it->second.m_DirEnt;
		return dirent.st_size;
	}
	return 0;
}
CHeaderEntry* CZZipFile::Find(const std::string& entry){
		std::map<std::string, CHeaderEntry>::iterator it = m_Header.find(entry);
		if (it!= m_Header.end())
		{
			return & (it->second);
		}
		return NULL;
}

	
