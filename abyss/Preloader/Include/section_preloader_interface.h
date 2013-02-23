#ifndef SECTION_PRELOADER_INTERFACE_HPP
#define SECTION_PRELOADER_INTERFACE_HPP

#include "../../MT_Core/Include/command.h"
#include "../../Objects/Include/object_animation.h"
#include <vector> 
#include <string> 

#define CPreloaderSection_CLSID 12011977

#define TYPE_JPG 0
#define TYPE_TGA 1 
#define TYPE_BMP 2
#define TYPE_MODEL 1001

class CObject;

struct CPreloaderSection_DecodeError : public CCommand
{
	std::string m_Message;
	long m_ErrorCode;
	long m_Handle;
    unsigned char m_Type;
	long m_Magic;
};


// request to decode bytestream into image 
struct CPreloaderSection_DecodeRequest : public CCommand
{
	unsigned char m_Type;
	long m_Handle;
	std::vector<unsigned char> m_Data;
	long m_Magic;
};

struct CPreloaderSection_FileDecodeRequest : public CCommand 
{
	unsigned char  m_Type;
	long m_Handle;
	long m_VFSSection;
	long m_Magic;
};

struct CPreloaderSection_ModelLoadRequest : public CCommand 
{
	std::string m_FileName;
	long m_VFSSection;
	long m_Magic;
};

struct CPreloaderSection_ModelLoadResponse : public CCommand 
{
	CObject * m_pObject;
	long m_Magic;
};

struct CPreloaderSection_MatrixLoadRequest : public CCommand
{
	std::string m_FileName;
	long m_VFSSection;
	long m_Magic;
};

struct CPreloaderSection_MatrixLoadResponse : public CCommand
{
	CObjectAnimationController* m_pController;
	long m_Magic;

};



struct CPreloaderSection_DecodeResponse : public CCommand
{
	long m_Handle;
	long m_Height;
	long m_Width;
	// A8R8G8B8 byte stream
	std::vector<unsigned char> m_ARGBData;
	long m_Magic;
};

#endif