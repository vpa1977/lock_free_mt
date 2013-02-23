#include "../../MT_Core/Include/section_registrator.h"
#include "../include/section_preloader.h"
#include "../include/model_context.h"
#include "../include/matrix_animation_context.h"

using namespace abyss::preloader;

void Touch_PreloaderSection() {};


static CTSectionRegistrator<CPreloaderSection_CLSID,CPreloaderSection> g_CPreloaderRegistrator(MULTIPLE_INSTANCES_ALLOWED);

#define JPG 0 
#define TGA 1
#define BMP 2 
#define MODEL 3
#define ANIM 4 

CPreloaderSection::CPreloaderSection(long sectionID ) : CSection(sectionID)
{
	RegisterReaction( GetID((CCommand_SectionStartup*)NULL), CTConform<CPreloaderSection,CCommand_SectionStartup>::Conform );
	RegisterReaction( GetID((CPreloaderSection_DecodeRequest*)NULL), CTConform<CPreloaderSection,CPreloaderSection_DecodeRequest>::Conform );
	RegisterReaction( GetID((CPreloaderSection_FileDecodeRequest*)NULL), CTConform<CPreloaderSection,CPreloaderSection_FileDecodeRequest>::Conform );
	RegisterReaction( GetID((CPreloaderSection_ModelLoadRequest*)NULL), CTConform<CPreloaderSection,CPreloaderSection_ModelLoadRequest>::Conform );
	RegisterReaction( GetID((CPreloaderSection_MatrixLoadRequest*)NULL), CTConform<CPreloaderSection,CPreloaderSection_MatrixLoadRequest>::Conform );
	
	RegisterSubSection(JPG, new CJpgReadSection());
	RegisterSubSection(TGA, new CTgaReadSection());
	RegisterSubSection(BMP, new CBmpReadSection());
	RegisterSubSection(MODEL, new CModelContext());
	RegisterSubSection(ANIM, new CMatrixAnimationContext());

}

CPreloaderSection::~CPreloaderSection()
{
}



void CPreloaderSection::Reaction(long in_SrcSectionID , const CPreloaderSection_ModelLoadRequest& in_rRead)
{
	long id  = GetSubSections().find(MODEL)->second->GetThisID();
	CModelDecodeRequest req;
	req.m_FileName= in_rRead.m_FileName;
	req.m_Magic = in_rRead.m_Magic;
	req.m_VFSSection = in_rRead.m_VFSSection;
	req.m_OrigSectionID = in_SrcSectionID;
	CTCommandSender<CModelDecodeRequest>::SendCommand( id , req);

}

void CPreloaderSection::Reaction(long in_SrcSectionID , const CPreloaderSection_MatrixLoadRequest& in_rRead)
{
	long id  = GetSubSections().find(ANIM)->second->GetThisID();
	CMatrixDecodeRequest req;
	req.m_FileName= in_rRead.m_FileName;
	req.m_Magic= in_rRead.m_Magic;
	req.m_VFSSection = in_rRead.m_VFSSection;
	req.m_OrigSectionID = in_SrcSectionID;
	CTCommandSender<CMatrixDecodeRequest>::SendCommand( id , req);}



void CPreloaderSection::Reaction(long in_SrcSectionID , const CPreloaderSection_FileDecodeRequest& decodeRequest)
{
	if (decodeRequest.m_Type == TYPE_JPG) 
	{
		long dest = GetSubSections().find(JPG)->second->GetThisID();
		CJpgFileDecodeRequest req;
		req.m_Handle = decodeRequest.m_Handle;
		req.m_Magic = decodeRequest.m_Magic;
		req.m_OrigSectionID = in_SrcSectionID;
		req.m_VFSSection = decodeRequest.m_VFSSection;
		
		CTCommandSender<CJpgFileDecodeRequest>::SendCommand( dest, req);
		
	}
	else
	if (decodeRequest.m_Type == TYPE_BMP) 
	{
		long dest = GetSubSections().find(BMP)->second->GetThisID();
		CBmpFileDecodeRequest req;
		req.m_Handle = decodeRequest.m_Handle;
		req.m_Magic = decodeRequest.m_Magic;
		req.m_OrigSectionID = in_SrcSectionID;
		req.m_VFSSection = decodeRequest.m_VFSSection;
		
		CTCommandSender<CBmpFileDecodeRequest>::SendCommand( dest, req);
		
	}
	else
	if (decodeRequest.m_Type == TYPE_TGA)
	{
		long dest = GetSubSections().find(TGA)->second->GetThisID();
		CTgaFileDecodeRequest req;
		req.m_Handle = decodeRequest.m_Handle;
		req.m_Magic = decodeRequest.m_Magic;
		req.m_OrigSectionID = in_SrcSectionID;
		req.m_VFSSection = decodeRequest.m_VFSSection;
		CTCommandSender<CTgaFileDecodeRequest>::SendCommand( dest, req);

	}

}


void CPreloaderSection::Reaction(long in_SrcSectionID , const CPreloaderSection_DecodeRequest& in_rRead)
{
	if (in_rRead.m_Type == TYPE_JPG) 
	{
		long dest = GetSubSections().find(JPG)->second->GetThisID();
		CJpgDecodeRequest req;
		req.m_Data = in_rRead.m_Data;
		req.m_Magic = in_rRead.m_Magic;
		req.m_OrigSectionID = in_SrcSectionID;
		CTCommandSender<CJpgDecodeRequest>::SendCommand( dest, req);


	}
	else
	if (in_rRead.m_Type == TYPE_BMP) 
	{
		long dest = GetSubSections().find(BMP)->second->GetThisID();
		CBmpDecodeRequest req;
		req.m_Data = in_rRead.m_Data;
		req.m_Magic = in_rRead.m_Magic;
		req.m_OrigSectionID = in_SrcSectionID;
		CTCommandSender<CBmpDecodeRequest>::SendCommand( dest, req);


	}
	else
	if (in_rRead.m_Type == TYPE_TGA)
	{
		long dest = GetSubSections().find(TGA)->second->GetThisID();
		CTgaDecodeRequest req;
		req.m_Data = in_rRead.m_Data;
		req.m_Magic = in_rRead.m_Magic;
		req.m_OrigSectionID = in_SrcSectionID;
		CTCommandSender<CTgaDecodeRequest>::SendCommand( dest, req);
	}
}
