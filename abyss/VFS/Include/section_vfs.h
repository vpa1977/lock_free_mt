#ifndef SECTION_VFS
#define SECTION_VFS

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
//#include "../ArchiveFile.h"
#include "CZipFile.h"

#include <boost/shared_ptr.hpp>

#include "section_vfs_interface.h"

//#include <string> 
#include <map>
#include <vector>
#include <list>

#pragma warning(disable:4250)


////////////////////////////////////////////////////////////////////////////////////////////////////////
// used to transfer read command from dispatch to 
// section handling request. 
/*
struct CTransferReadCommand : public CCommand 
{
		CTransferReadCommand(const CVFSSection_ReadRequest& readReq) 
		{
		}
		std::string vUrl; // virtual file location 
		std::string url;  // real file location
		std::string m_MountPoint; // mount point for resource
		long m_Offset;    // offset in the file 
		long m_Size;      // length to read 
		long handle;	  // handle of section to respond to 
};
*/


#define FLAG_FILE 0
#define FLAG_ARCH 1
#define FLAG_FZIP 2

struct CFileInfo
{
	short flag; // (0 - plain file , 1 - arch file) 
	long size ; // size in bytes
	FILE* f; // file pointer
	CZipFile* zFile; // pointer to zipFile.
	CHeaderEntry* entry; // pointer to header entry to use
};

////////////////////////////////////////////////////////////////////////////////////////////////////////



/**
	VFS dispatch section . All resource allocation commands should be send to it
 */
class CVFSSection :
	public CSection
	,public CTCommandSender<CCommand_LaunchSection>
	,public CTCommandSender<CVFSSection_MountResponse>
	
	,public CTCommandSender<CVFSSection_ReadResponse>
	,public CTCommandSender<CVFSSection_CloseRequest>
	,public CTCommandSender<CVFSSection_WriteResponse>
	,public CTCommandSender<CVFSSection_UMountResponse>

	,public CTCommandSender< CVFSSection_Error<CVFSSection_MountRequest> >
	
	,public CTCommandSender< CVFSSection_Error<CVFSSection_OpenRequest> >
	,public CTCommandSender< CVFSSection_OpenResponse >
	,public CTCommandSender< CVFSSection_Error<CVFSSection_ReadRequest> >
	,public CTCommandSender< CVFSSection_Error<CVFSSection_CloseRequest> >
	,public CTCommandSender< CVFSSection_Error<CVFSSection_UMountRequest> >
	,public CTCommandSender< CVFSSection_CloseResponse >
	,public CTCommandSender<CVFSSection_ExistsResponse>
{
public:
	CVFSSection( long in_SectionID );
	~CVFSSection();
	void Reaction(long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand );
	void Reaction(long in_SrcSectionID, const CVFSSection_MountRequest& in_rMountCommand);
	void Reaction(long in_SrcSectionID, const CVFSSection_UMountRequest& in_rMountRequest);
	void Reaction( long in_SrcSectionID,const CVFSSection_ReadRequest& in_rBuffer);
	void Reaction(long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rLaunchSectionResult );
	void Reaction( long in_SrcSectionID, const CVFSSection_CloseRequest& in_rClose);
	void Reaction( long in_SrcSectionID, const CVFSSection_ExistsRequest& in_rExists);
	void Reaction(long  in_SrcSectionID, const CVFSSection_WriteRequest& in_rWrite);
	
	void Reaction(long  in_SrcSectionID, const CVFSSection_OpenRequest& in_rOpen);

	virtual std::string GetName() {return "CVfsSection"; };

private:
	//  build archive list
	void BuildList(const char* dir, const char* sub, const char* MASK);
	std::map<long, CFileInfo> m_OpenFiles;
	std::vector<boost::shared_ptr<CZipFile> > m_ZipFiles;
	
	std::string m_VfsRoot;
};




#endif
