//#include "../../MT_Core/Include/idgen.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../include/section_vfs.h"

#include <io.h>

#define ARCH_MASK "/*.zar"
#define ZZIP_MASK "/*.zip"


void Touch_VFSSection() {};

static CTSectionRegistrator<CVFSSection_CLSID,CVFSSection> g_CVFSSectionRegistrator(SINGLE_INSTANCE_ONLY);

// internal counter to be used in open queues
// just some VFS wide unique number
static long m_open_handle_count = 0;

CVFSSection::CVFSSection( long in_SectionID ) :CSection( in_SectionID )
{
	printf("CVFSSection::CVFSSection()\n");
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL), CTConform<CVFSSection,CCommand_SectionStartup>::Conform );
	/*RegisterReaction( GetID((CCommand_MTInfo*)NULL), CTConform<CVFSSection,CCommand_MTInfo>::Conform );
	RegisterReaction( GetID((CCommand_LaunchThreadsResult*)NULL), CTConform<CVFSSection,CCommand_LaunchThreadsResult>::Conform );*/
	RegisterReaction( GetID((CCommand_LaunchSectionResult*)NULL), CTConform<CVFSSection,CCommand_LaunchSectionResult>::Conform );
	RegisterReaction( GetID((CVFSSection_MountRequest*)NULL), CTConform<CVFSSection,CVFSSection_MountRequest>::Conform );
	RegisterReaction( GetID((CVFSSection_UMountRequest*)NULL), CTConform<CVFSSection,CVFSSection_UMountRequest>::Conform );
	RegisterReaction( GetID((CVFSSection_ReadRequest*)NULL), CTConform<CVFSSection,CVFSSection_ReadRequest>::Conform );
	RegisterReaction( GetID((CVFSSection_CloseRequest*)NULL), CTConform<CVFSSection,CVFSSection_CloseRequest>::Conform );
	RegisterReaction( GetID((CVFSSection_OpenRequest*)NULL), CTConform<CVFSSection,CVFSSection_OpenRequest>::Conform );
	RegisterReaction( GetID((CVFSSection_ExistsRequest*)NULL), CTConform<CVFSSection,CVFSSection_ExistsRequest>::Conform );
	printf("CVFSSection::CVFSSection() end\n");
}

CVFSSection::~CVFSSection()
{
	printf("CVFSSection::~CVFSSection()\n");
	printf("CVFSSection::~CVFSSection() end\n");
}



void CVFSSection::Reaction(long in_SrcSectionID, const CCommand_SectionStartup& in_rStartupCommand )
{
//	printf("Section Startup Detected!");
}


void CVFSSection::Reaction(long in_SrcSectionID, const CVFSSection_WriteRequest& in_rWriteRequest){
	
	CVFSSection_WriteResponse response;

}


void CVFSSection::Reaction( long in_SrcSectionID, const CVFSSection_ExistsRequest& in_rExists)
{
	CVFSSection_ExistsResponse resp;
	resp.m_Url = in_rExists.m_Url;
	resp.m_Exists = false;

	std::string path = m_VfsRoot + "/" +  in_rExists.m_Url;

	_finddata_t findData;
	intptr_t res = _findfirst(path.data(), &findData);
	if (res < 0  )
	{
		// need to scan archives 
		CHeaderEntry* entry = NULL;
		std::vector<boost::shared_ptr<CZipFile> >::iterator it = m_ZipFiles.begin();
		while (it != m_ZipFiles.end()) 
		{
			entry = (*it)->Find(resp.m_Url);
			if (entry)
			{
				resp.m_Exists = true;
				break;
			}
			it++;
		}
		
	
	}
	else
	{
		_findclose(res);
		resp.m_Exists = true;
	}
	resp.m_Magic = in_rExists.m_Magic;
	CTCommandSender<CVFSSection_ExistsResponse>::SendCommand(in_SrcSectionID, resp);

}


void CVFSSection::Reaction( long in_SrcSectionID,const CVFSSection_OpenRequest& in_rOpenRequest)
{
    CVFSSection_Error<CVFSSection_OpenRequest> error;
	error.m_Req = in_rOpenRequest;
	
	std::string path = m_VfsRoot + "/" +  in_rOpenRequest.m_Url;

	_finddata_t findData;
	intptr_t res = _findfirst(path.data(), &findData);
	if (res < 0  )
	{
		path = in_rOpenRequest.m_Url;
		if (in_rOpenRequest.m_Url[0] !='/' ) 
		{
			path = "/" + in_rOpenRequest.m_Url;
		}
		// need to scan archives 
		CHeaderEntry* entry = NULL;
		std::vector<boost::shared_ptr<CZipFile> >::iterator it = m_ZipFiles.begin();
		while (it != m_ZipFiles.end()) 
		{
			entry = (*it)->Find(path);
			if (entry)
			{
				m_open_handle_count ++;
				CFileInfo info;
				info.flag = FLAG_ARCH;
				info.f = NULL;
				info.entry = entry;
				info.zFile = (*it)->Self();
				
				m_OpenFiles[m_open_handle_count] = info;

				CVFSSection_OpenResponse resp;
				resp.m_Handle = m_open_handle_count;
				resp.m_Url = in_rOpenRequest.m_Url;
				resp.m_Magic = in_rOpenRequest.m_Magic;
				CTCommandSender<CVFSSection_OpenResponse>::SendCommand(in_SrcSectionID, resp);	
				return;
			}
			it++;
		}
	}
	else
	{
		_findclose(res);
		// open file section
		if (findData.attrib & _A_SUBDIR ) 
		{
			error.m_ErrorCode = VFS_NOT_A_FILE;
			error.m_ErrorMessage = "Not a file ";
			error.m_Magic = in_rOpenRequest.m_Magic;
			CTCommandSender< CVFSSection_Error<CVFSSection_OpenRequest> >::SendCommand(in_SrcSectionID , error);
			return;
		}
		FILE * f = fopen(path.data() , "rbw+");
		if (ferror(f)) 
		{
			error.m_ErrorCode = ferror(f);
			error.m_ErrorMessage = "File open failed";
			error.m_Magic = in_rOpenRequest.m_Magic;
			CTCommandSender< CVFSSection_Error<CVFSSection_OpenRequest> >::SendCommand(in_SrcSectionID , error);
			return;
		}
		

		m_open_handle_count ++;
		CFileInfo info;
		info.size = findData.size;
		info.flag = FLAG_FILE;
		info.f = f;
		m_OpenFiles[m_open_handle_count] = info;
		CVFSSection_OpenResponse resp;
		resp.m_Handle = m_open_handle_count;
		resp.m_Url = in_rOpenRequest.m_Url;
		resp.m_Magic = in_rOpenRequest.m_Magic;
		CTCommandSender<CVFSSection_OpenResponse>::SendCommand(in_SrcSectionID, resp);	
		return;
	}

	// attempt to read zip file. 






	error.m_ErrorCode = -1;
	error.m_ErrorMessage = "File not found";
	error.m_Magic = in_rOpenRequest.m_Magic;
	CTCommandSender< CVFSSection_Error<CVFSSection_OpenRequest> >::SendCommand(in_SrcSectionID, error);

}

void CVFSSection::Reaction(long in_SrcSectionID, const CCommand_LaunchSectionResult& in_rLaunchSectionResult )
{
}

void CVFSSection::Reaction(long in_SrcSectionID, const CVFSSection_CloseRequest& in_rClose )
{
	CVFSSection_Error<CVFSSection_CloseRequest> error;
	error.m_Req = in_rClose;
	std::map<long, CFileInfo>::iterator it = m_OpenFiles.find(in_rClose.m_Handle);
	if (it == m_OpenFiles.end())
	{
		error.m_ErrorCode = VFS_UNKNOWN_HANDLE;	
		error.m_ErrorMessage = "File handle unknown";
		error.m_Magic = in_rClose.m_Magic;
		CTCommandSender< CVFSSection_Error<CVFSSection_CloseRequest> >::SendCommand(in_SrcSectionID , error);
	}
	CFileInfo& info = it->second;
	if (info.flag == FLAG_FILE ) 
	{
		fclose(info.f);
	}
	m_OpenFiles.erase(in_rClose.m_Handle);
    
	CVFSSection_CloseResponse resp ; 
	resp.m_Handle = in_rClose.m_Handle;
	resp.m_Magic = in_rClose.m_Magic;
	CTCommandSender< CVFSSection_CloseResponse >::SendCommand(in_SrcSectionID , resp);
}



void CVFSSection::Reaction( long in_SrcSectionID,const CVFSSection_ReadRequest& in_rReadCommand)
{
	CVFSSection_Error<CVFSSection_ReadRequest> error;
	error.m_Req = in_rReadCommand;
	
	std::map<long, CFileInfo>::iterator it = m_OpenFiles.find(in_rReadCommand.m_Handle);
	if (it == m_OpenFiles.end())
	{
		error.m_ErrorCode = VFS_UNKNOWN_HANDLE;	
		error.m_ErrorMessage = "File handle unknown";
		error.m_Magic = in_rReadCommand.m_Magic;
		CTCommandSender< CVFSSection_Error<CVFSSection_ReadRequest> >::SendCommand(in_SrcSectionID , error);
		return;
	}
	CFileInfo& info = it->second;

	if (info.flag == FLAG_FILE ) 
	{
		CVFSSection_ReadResponse rc;
		// init rc with transfer params
		rc.m_Offset = in_rReadCommand.m_Offset;
		rc.m_Size = in_rReadCommand.m_Size;
		rc.m_Handle = in_rReadCommand.m_Handle;
		//printf("Resource recieved read request");
		if (rc.m_Size < 0 ) 
		{
			rc.m_Size = info.size;
		}

		rc.m_pData.resize(rc.m_Size);
		if (fseek(info.f , rc.m_Offset , SEEK_SET) < 0 ) 
		{
			error.m_ErrorCode = VFS_FILE_POSITION_UNKNOWN;	
			error.m_ErrorMessage = "Unable to set file position";
			error.m_Magic = in_rReadCommand.m_Magic;
			CTCommandSender< CVFSSection_Error<CVFSSection_ReadRequest> >::SendCommand(in_SrcSectionID , error);
			return;
		}
		assert(rc.m_pData.size() == rc.m_Size);
		if (rc.m_Size == 0 ) 
		{
			rc.m_Magic = in_rReadCommand.m_Magic;
			CTCommandSender<CVFSSection_ReadResponse>::SendCommand( in_SrcSectionID, 
				rc);
			return;

		}
		else 
		{
			size_t read = 	fread( (char*)&(rc.m_pData[0]), 1 ,rc.m_Size ,info.f );
			rc.m_Size =(long) read;
			rc.m_Magic = in_rReadCommand.m_Magic;
			CTCommandSender<CVFSSection_ReadResponse>::SendCommand( in_SrcSectionID, 
				rc);
			return;
		}
	}
	else
	{
	       CVFSSection_ReadResponse rc;
			// init rc with transfer params
			rc.m_Offset = in_rReadCommand.m_Offset;
			rc.m_Size = in_rReadCommand.m_Size;
			rc.m_Handle = in_rReadCommand.m_Handle;
			//printf("Resource recieved read request");
			if (rc.m_Size < 0 ) 
			{
				
				rc.m_Size =info.zFile->SizeOf(info.entry) ;
			}

			rc.m_pData.resize(rc.m_Size);

			if (rc.m_Size == 0) 
			{
				rc.m_Magic = in_rReadCommand.m_Magic;
				CTCommandSender<CVFSSection_ReadResponse>::SendCommand( in_SrcSectionID, 
						rc);		
				return;

			}

			int read = info.zFile->Read(info.entry, (char*)&(rc.m_pData[0]), rc.m_Offset , rc.m_Size);
			rc.m_Size = read;
			rc.m_Magic = in_rReadCommand.m_Magic;
			CTCommandSender<CVFSSection_ReadResponse>::SendCommand( in_SrcSectionID, 
				rc);		

	}

}

void CVFSSection::Reaction(long in_SrcSectionID, const CVFSSection_UMountRequest& in_rReq)
{
	std::map<long, CFileInfo>::iterator it = m_OpenFiles.begin();
	while (it != m_OpenFiles.end() ) 
	{
		CFileInfo info = it->second;
		if (info.flag == FLAG_FILE) 
		{
			fclose(info.f);
		}
		it++;
	}
	m_OpenFiles.clear();

	std::vector<boost::shared_ptr<CZipFile> >::iterator zIt = m_ZipFiles.begin();
	while (zIt != m_ZipFiles.end() ) 
	{
		(*zIt)->Close();
		
		zIt++;
	}
	m_ZipFiles.clear();
	CVFSSection_UMountResponse resp;
	resp.m_Magic = in_rReq.m_Magic;
	CTCommandSender<CVFSSection_UMountResponse>::SendCommand(in_SrcSectionID ,resp );
}

void CVFSSection::Reaction( long in_SrcSectionID,const CVFSSection_MountRequest& in_rMountCommand)
{
	// mount requested
	//printf("In mount stage");
	 _finddata_t findData;
	 intptr_t res = _findfirst(in_rMountCommand.m_pRoot.data(), &findData);
	if (res < 0 ) 
	{
		// handle error = path doesnt exist.
			CVFSSection_Error<CVFSSection_MountRequest> error(in_rMountCommand, MOUNT_NOK, "Mount path was not found ");
			CTCommandSender< CVFSSection_Error<CVFSSection_MountRequest> >::SendCommand(in_SrcSectionID ,
				error); 

	}
	else 
	{
		// handle was found
		_findclose(res);
		// iterate and open all archive resources.
		m_open_handle_count = 0;
		BuildList(in_rMountCommand.m_pRoot.data() , NULL, ARCH_MASK);
		BuildList(in_rMountCommand.m_pRoot.data() , NULL, ZZIP_MASK);
		m_VfsRoot = in_rMountCommand.m_pRoot;
		CVFSSection_MountResponse resp;
		resp.m_Magic = in_rMountCommand.m_Magic;	
		CTCommandSender<CVFSSection_MountResponse>::SendCommand(in_SrcSectionID, resp);
	}
}

void CVFSSection::BuildList(const char* dir, const char* sub, const char* MASK)
{
	int result = 0;
	char buffer[1024];
	memset(buffer, 0, 1024);
	_finddata_t findData;
	strcpy(buffer, dir);
	
	strcat(buffer, MASK);

	result = (int)_findfirst(buffer, &findData);
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
			BuildList(buffer, supp.data(), MASK);
		}
		else
		{
			std::string  entry = dir;
			if (sub ) 
			{
				entry = sub;
				entry += "/";
			}
			else
			{
				entry += "/";
			}
			
			entry += findData.name;

			
			
			if (MASK == ZZIP_MASK) 
			{

				CZZipFile* zFile = new CZZipFile();
				
				zFile->Open(entry.data());
				boost::shared_ptr<CZipFile> px(zFile);
				m_ZipFiles.push_back(px);
			}
			else
			{
				CArchZipFile* zFile = new CArchZipFile();
				zFile->Open(entry.data());
				boost::shared_ptr<CZipFile> px(zFile);
				m_ZipFiles.push_back(px);
			}
			
		}
	} while (!_findnext(result,&findData));
	_findclose(result);
}








