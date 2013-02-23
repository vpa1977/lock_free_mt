#ifndef CZIP_FILE
#define CZIP_FILE

#include <map>
#include <list>
#include <string>
#include <vector>
#include "../zziplib/zzip/zzip.h"
/*
.......................
.......................

HEADER 
!
header := N_ENTRY(long), header_entry+

header_entry := FILE NAME,0, N CHUNKs (long) , (chunk_desc)+

chunk_desc := file_offset(long), size(long), real_offset(long), real_length

*/

#define VFS_REAL_CHUNK 65536

struct CChunkDesc
{
	CChunkDesc()
	{
	}

	CChunkDesc (const CChunkDesc& desc ) 
	{
		m_Offset = desc.m_Offset;
		m_Size = desc.m_Size;
		m_RealLength = desc.m_RealLength;
		m_RealOffset = desc.m_RealOffset;
	}
	unsigned long m_Offset;
	unsigned long m_Size;
	unsigned long m_RealOffset;
	unsigned long m_RealLength; // real length of chunk, 16kB by default, but can vary
};

struct CHeaderEntry
{
	CHeaderEntry( ) 
	{
	}
	CHeaderEntry(const CHeaderEntry& entry ) 
	{
		m_FileName = entry.m_FileName;
		m_nChunks = entry.m_nChunks;
		m_ChunkDesc = entry.m_ChunkDesc;
	}

	std::string m_FileName;
	long m_nChunks;
	std::vector<CChunkDesc> m_ChunkDesc; // map - real offset in file 
	void Write(FILE * f);
	void Read(FILE *f);

};

struct CZHeaderEntry 
{
	std::string m_FileName;
	ZZIP_DIRENT m_DirEnt;
};

#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define max(a,b)            (((a) > (b)) ? (a) : (b))



class CZipFile 
{
public:
	
	virtual ~CZipFile(){};

	// open archive file.
	virtual bool IsOpen() = 0; 
	virtual void Close() = 0;
	// read archive entry. 
	// file - archive entry
	// buffer - buffer to recieve data
	// offset - offset in file
	// size - to read
	// returns - bytes read.
	virtual int Read(const char* file, void * buffer ,unsigned long offset, unsigned long size)= 0;
	virtual int Read(CHeaderEntry* entry, void * buffer ,unsigned long offset, unsigned long size)= 0;
	virtual bool Exists(const char* resource )= 0;
	virtual long SizeOf(const char* resource)= 0;
	virtual long SizeOf(CHeaderEntry* entry)= 0;
	virtual CHeaderEntry* Find(const std::string& entry)= 0;

	virtual CZipFile* Self() = 0;
};




class CZZipFile  : public CZipFile
{
public:
	CZZipFile()
	{
		m_DIR = NULL;
	}
	CZipFile* Self() { return this; }
	// open archive file.
	bool IsOpen();

	void Open(const char* archName);
	void Close();
	// read archive entry. 
	// file - archive entry
	// buffer - buffer to recieve data
	// offset - offset in file
	// size - to read
	// returns - bytes read.
	int Read(const char* file, void * buffer ,unsigned long offset, unsigned long size);
	int Read(CHeaderEntry* entry, void * buffer ,unsigned long offset, unsigned long size);
	bool Exists(const char* resource );
	long SizeOf(const char* resource);
	long SizeOf(CHeaderEntry* entry);
	CHeaderEntry* Find(const std::string& entry);
private:
	// just for interface compatibility
	std::map<std::string, CHeaderEntry> m_Header;
	std::map<std::string, CZHeaderEntry> m_ZHeader;
	ZZIP_DIR* m_DIR;

};



class CArchZipFile : public CZipFile
{
public:
	CArchZipFile(){
		m_File = NULL;
	};
	// utility method to create archive in our format
	void Create(const char* dir, const char* archName);
	// utility method to decompress archive
	void Decompress(const char* archName, const char* dir );

	CZipFile* Self() { return (CZipFile*)this; }
	


	// open archive file.
	bool IsOpen() {
		return m_File != NULL;
	}

	void Open(const char* archName);
	void Close();
	// read archive entry. 
	// file - archive entry
	// buffer - buffer to recieve data
	// offset - offset in file
	// size - to read
	// returns - bytes read.
	int Read(const char* file, void * buffer ,unsigned long offset, unsigned long size);
	int Read(CHeaderEntry* entry, void * buffer ,unsigned long offset, unsigned long size);
	
	bool Exists(const char* resource ) 
	{
		return m_Header.find(resource) != m_Header.end();
	}

	long SizeOf(const char* resource) 
	{
		if (!Exists(resource))
		{
			return -1;
		}
		long size = 0;
		CHeaderEntry& entry = m_Header.find(resource)->second;
		for (int i = 0 ; i < entry.m_nChunks ; i ++ ) 
		{
			size += entry.m_ChunkDesc[i].m_RealLength;
		}
		return size;
	}

	long SizeOf(CHeaderEntry* entry)
	{
		long size = 0;
		for (int i = 0 ; i < entry->m_nChunks ; i ++ ) 
		{
			size += entry->m_ChunkDesc[i].m_RealLength;
		}
		return size;
	}

	CHeaderEntry* Find(const std::string& entry)
	{
		std::map<std::string, CHeaderEntry>::iterator it = m_Header.find(entry);
		if (it!= m_Header.end())
		{
			return & (it->second);
		}
		return NULL;
	}


private:
	// utility methods
	int decompressChunk(char* outBuffer, FILE * f, CChunkDesc& desc ) ;
	void BuildList(const char* dir, const char* sub);
	void WriteArchive(const char* dir, const char* archName);
	std::map<std::string, CHeaderEntry> m_Header;
	FILE * m_File;
	// keeps data offset in the file.
	unsigned long m_DataOffset;
};






#endif