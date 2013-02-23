#ifndef SECTION_VFS_HEADER
#define SECTION_VFS_HEADER

#include <vector>
#include "../../MT_Core/Include/core.h"

#define CVFSSection_CLSID 22011977
//#define CVFSResource_CLSID 26021979
//#define CARCResource_CLSID 260219791

#define MOUNT_OK 0
#define MOUNT_NOK 1



/// ERROR code

/* 
todo 
MountRequest(string) -> MountResponse() | MountError(errorcode,errortext)
OpenRequest (string ) -> OpenResponse(handle)  | OpenError(string,errorcode,errortext)
ReadRequest (handle, offset, length ) -> ReadResponse(handle, offset, length, buffer) | ReadError(handle, offset, length, errorcode,errortext)
WriteRequest (handle, offset, length ) -> WriteResponse(handle, offset, length, buffer) | WriteError(handle, offset, length, errorcode,errortext)
CloseRequest(handle) -> CloseResponse()| CloseError( errorcode,errortext )
UMountRequest(string ) -> UMountResponse() | UMountError(errorcode,errortext)

response - char* -> std::vector<char>

+ dispatch section -> access to headers.

*/



#define VFS_NO_MOUNT_POINT_FOUND 1000
#define VFS_RESOURCE_DOESNT_EXIST 1001
#define VFS_NOT_A_FILE 1002
#define VFS_UNKNOWN_HANDLE 1003
#define VFS_FILE_POSITION_UNKNOWN 1004

#include <string>

// ========================================================================================

template< typename T >
struct CVFSSection_Error : public CCommand
{
	CVFSSection_Error() {}
	CVFSSection_Error(const T& t, int code, const std::string& message) :
		m_Req(t)
		,m_ErrorCode(code)
		,m_ErrorMessage(message)
	{}
	T m_Req;         // request which caused error
	int m_ErrorCode; // error code.
	std::string m_ErrorMessage; // error message
	long m_Magic;
};

// ========================================================================================

struct CVFSSection_OpenRequest : public CCommand 
{
	CVFSSection_OpenRequest() {}
	CVFSSection_OpenRequest(const std::string& url ) : m_Url(url), m_Magic(-1) {}
	CVFSSection_OpenRequest(const CVFSSection_OpenRequest& req ) : m_Url(req.m_Url), m_Magic(req.m_Magic) {}
	std::string m_Url;
	long m_Magic;
};

struct CVFSSection_OpenResponse : public CCommand
{
	std::string m_Url;
	long m_Handle;
	long m_Magic;
};

// ========================================================================================

struct CVFSSection_ExistsRequest : public CCommand
{
	std::string m_Url;
	long m_Magic;
};

struct CVFSSection_ExistsResponse : public CCommand
{
	bool m_Exists;
	std::string m_Url;
	long m_Magic;
};

// ========================================================================================

struct CVFSSection_MountRequest : public CCommand
{
	CVFSSection_MountRequest(){};
	CVFSSection_MountRequest(const std::string& root )
	{
		m_pRoot = root;
	}
	std::string m_pRoot;
	long m_Magic;
};

struct CVFSSection_MountResponse : public CCommand
{
	long m_Magic;
};

// ========================================================================================

struct CVFSSection_UMountRequest : public CCommand
{
	long m_Magic;
};

struct CVFSSection_UMountResponse :  public CCommand 
{
	long m_Magic;
};

// ========================================================================================

struct CVFSSection_ReadRequest : public CCommand
{
	CVFSSection_ReadRequest( long in_Handle=0, long in_Offset=0, long in_Size=-1 ) :
		m_Handle(in_Handle)
		,m_Offset(in_Offset)
		,m_Size(in_Size)
	{}
	long m_Handle;    // handle of file to read
	long m_Offset;    // offset in the file 
	long m_Size;      // length to read 
	long m_Magic;
};

struct CVFSSection_ReadResponse : public CCommand
{
	CVFSSection_ReadResponse() {}
	CVFSSection_ReadResponse(const CVFSSection_ReadRequest& req)
	{
		m_Offset= req.m_Offset;
		m_Size= req.m_Size;
		m_Handle = req.m_Handle;
	}
	~CVFSSection_ReadResponse()
	{
	}
	std::vector<unsigned char> m_pData; // read buffer - allocated in resource section 
	long m_Offset; // offset in the file 
	long m_Size;   // size of data read
	long m_Handle; // handle
	long m_Magic;
};

// ========================================================================================

struct CVFSSection_WriteRequest : public CCommand 
{
	std::string vUrl; // url of file to write
	char* m_Buffer;   // byte buffer
	long m_Offset;    // offset in the file to write, -1 to append
	long m_Size;      // size of the buffer
	long m_Magic;
};

struct CVFSSection_WriteResponse : public CCommand 
{
	std::string vUrl; // url of the file to write
	char* m_Buffer;   // byte buffer
	long m_Size;      // size of the buffer
	long m_Result;    // result of the operation
	long m_Magic;
};

// ========================================================================================

struct CVFSSection_CloseRequest : public CCommand {
	CVFSSection_CloseRequest( long in_Handle=0 ) : m_Handle(in_Handle) {}
	long m_Handle;
	long m_Magic;
};

struct CVFSSection_CloseResponse : public CCommand {
	long m_Handle;
	long m_Magic;
};

// ========================================================================================


#endif 

