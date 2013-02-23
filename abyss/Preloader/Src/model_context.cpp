#include <vector>
#include <map>
#include "../../VFS/include/section_vfs_interface.h"
#include "../include/model_context.h"
#include "../include/entity_serializer.h"
#include "../Include/jpg_read_section.h"
#include "../Include/bmp_read_section.h"
#include "../Include/tga_subsection.h"



using namespace abyss::preloader;


typedef std::map< std::string , boost::shared_ptr<CRenderable_AbstractTextureMap> > image_map_type;
std::map< std::string , boost::shared_ptr<CRenderable_AbstractTextureMap> > CModelContext::LOADED_IMAGES;


bool CModelContext::LoadMaterial( const CVFSSection_ReadResponse& in_rRead)
{
	const std::vector<unsigned char> & data  = in_rRead.m_pData;
	long count;
	long ptr  = sizeof(long);
	assert_input(data, 0, sizeof(long));
	memcpy(&count, &data[0] , sizeof(long));
	m_MaterialList.resize(count);
	m_LoadedMaterials.resize(count);
	for (int i = 0 ; i < count ; i ++ ) 
	{
		long nameLen;
		long fileLen;
		std::vector<unsigned char> name;
		std::vector<unsigned char> file;
		assert_input(data, ptr, sizeof(long));
		memcpy(&nameLen, &data[ptr] , sizeof(long));
		name.resize(nameLen+1);
		memset(&name[0] , nameLen+1, sizeof(char));
		ptr+=sizeof(long);
		assert_input(data, ptr, nameLen);
		memcpy(&name[0], &data[ptr], nameLen);
		ptr+=nameLen;

		long diffSource;
		long wrapU;
		long wrapV;
		long diffColor;
		assert_input(data, ptr, sizeof(long));
		memcpy(&diffSource, &data[ptr] , sizeof(long));
		ptr+=sizeof(long);
		assert_input(data, ptr, sizeof(long));
		memcpy(&wrapU, &data[ptr], sizeof(long));
		ptr+=sizeof(long);
		assert_input(data, ptr, sizeof(long));
		memcpy(&wrapV, &data[ptr] , sizeof(long));
		ptr+=sizeof(long);
		assert_input(data, ptr, sizeof(long));
		memcpy(&diffColor, &data[ptr], sizeof(long));
		ptr+=sizeof(long);
		/*
		push_long(data, diffSource);
		push_long(data, wrapU);
		push_long(data, wrapV);
		push_long(data, (*it).m_DiffuseColor);
		*/
		
		assert_input(data, ptr, sizeof(long));
		memcpy(&fileLen, &data[ptr] , sizeof(long));
		file.resize(fileLen+1);
		memset(&file[0] , fileLen+1, sizeof(char));
		
		ptr+=sizeof(long);
		assert_input(data, ptr, fileLen);
		memcpy(&file[0], &data[ptr], fileLen);
		ptr+=fileLen;
		
		m_MaterialList[i].m_WrapU = wrapU;
		m_MaterialList[i].m_WrapV = wrapV;
		m_MaterialList[i].m_DiffSource = diffSource;
		m_MaterialList[i].m_DiffColor = diffColor;

		m_MaterialList[i].m_Name = (char*)&name[0];
		if (fileLen > 0 ) {
			
			m_MaterialList[i].m_File = (char*)&file[0];
			const char * name = m_MaterialList[i].m_File.c_str();
			int pos = m_MaterialList[i].m_File.find(".tga");
			pos += m_MaterialList[i].m_File.find(".TGA");
			int isTga = pos;
			int isBmp = (m_MaterialList[i].m_File.find(".bmp")) +  (m_MaterialList[i].m_File.find(".BMP"));
			int isJpg = (m_MaterialList[i].m_File.find(".jpg")) +  (m_MaterialList[i].m_File.find(".JPG") );
			if (isTga > 0)
			{
				m_MaterialList[i].m_Type = TYPE_TGA;
			}
			else
			if (isBmp > 0)
			{
				m_MaterialList[i].m_Type = TYPE_BMP;
			}
			else
			if (isJpg > 0)
			{
				m_MaterialList[i].m_Type = TYPE_JPG;
			}
			else
			{
				// by default we think that this is TGA texture
				m_MaterialList[i].m_Type = TYPE_TGA;
			}

			// we have already loaded this image map
			image_map_type::iterator it = LOADED_IMAGES.find(m_MaterialList[i].m_File);
			if (it != LOADED_IMAGES.end() ) 
			{
				CRenderable_AbstractMaterial mat;
				mat.m_DiffuseSource =(CRenderable_AbstractMaterial::DIFFUSE_SOURCE) m_MaterialList[i].m_DiffSource;
				mat.m_Diffuse.m_pTextureMap = it->second;
				mat.m_Diffuse.m_WrapModeU = (CRenderable_AbstractTexture::WRAP_MODE)m_MaterialList[i].m_WrapU;
				mat.m_Diffuse.m_WrapModeV = (CRenderable_AbstractTexture::WRAP_MODE)m_MaterialList[i].m_WrapV;
				m_LoadedMaterials[i] =mat;

			}
			else
			{
				m_TextureMaterialCount++;
				CVFSSection_OpenRequest openReq;
				openReq.m_Magic = in_rRead.m_Magic;
				openReq.m_Url = "/" + m_MaterialList[i].m_File; // lets expect textures to be at the "root" for now
				CTCommandSender<CVFSSection_OpenRequest>::SendCommand(m_VFS, openReq);
			}
		}
		else // create material immediatly
		{
			CRenderable_AbstractMaterial mat;
			
			mat.m_ARGB = m_MaterialList[i].m_DiffColor;
			mat.m_DiffuseSource = (CRenderable_AbstractMaterial::DIFFUSE_SOURCE) m_MaterialList[i].m_DiffSource;
			m_LoadedMaterials[i] = mat;
		}
	}
	// now lets load textures for our materials;
	m_GlobalMark = ptr;
	m_Read = data;
	m_Stage = NEXT_TEXTURE;
	/*for (int i = 0 ; i < count ; i ++ ) 
	{
		CVFSSection_OpenRequest openReq;
		openReq.m_Magic = GetMyMagic();
		openReq.m_Url = "/" + m_MaterialList[i].m_File; // lets expect textures to be at the "root" for now
		CTCommandSender<CVFSSection_OpenRequest>::SendCommand(m_VFS, openReq);
	}*/
	// well lets skip right to the model.
	if (!m_TextureMaterialCount) 
	{
		m_Stage = GEOMETRY;
		return LoadGeometry(in_rRead.m_Magic, m_Read, m_GlobalMark);
	}
	return false;
}

bool CModelContext::LoadTexture(const CVFSSection_ReadResponse& in_rRead)
{
	
	CReply reply = GetReply(in_rRead.m_Magic);
	for (int i = 0 ; i < m_MaterialList.size() ; i ++ ) 
	{
		if (m_MaterialList[i].m_Handle == in_rRead.m_Handle ) 
		{
			CPreloaderSection_DecodeResponse resp;
			switch (m_MaterialList[i].m_Type)
			{
				case TYPE_JPG: {
									CJpgReadSection ctx;
									ctx.DecodeJPG((void*)&in_rRead.m_pData[0], in_rRead.m_Size, resp);
							}
						break;
				case TYPE_TGA:{
								CTgaReadSection ctx;
								ctx.DecodeTGA((void*)&in_rRead.m_pData[0], in_rRead.m_Size, resp);
							}
						break;
				case TYPE_BMP:{
								CBmpReadSection ctx;
								ctx.DecodeBMP((void*)&in_rRead.m_pData[0], in_rRead.m_Size, resp);

							}
						break;

			}
			CRenderable_AbstractTextureMap* pMap;
			pMap = new CRenderable_AbstractTextureMap();
			pMap->m_Data.resize(resp.m_ARGBData.size());
			memcpy((char*)&pMap->m_Data[0], (char*)&resp.m_ARGBData[0],resp.m_ARGBData.size());
			
			pMap->m_bHasAlpha = true;
			pMap->m_Height = resp.m_Height;
			pMap->m_Width  = resp.m_Width;
				
			CRenderable_AbstractMaterial mat;
			mat.m_DiffuseSource =(CRenderable_AbstractMaterial::DIFFUSE_SOURCE) m_MaterialList[i].m_DiffSource;
			
			
		

			mat.m_Diffuse.m_pTextureMap = boost::shared_ptr<CRenderable_AbstractTextureMap>(pMap);
			LOADED_IMAGES[ m_MaterialList[i].m_File ] = mat.m_Diffuse.m_pTextureMap;

			mat.m_Diffuse.m_WrapModeU = (CRenderable_AbstractTexture::WRAP_MODE)m_MaterialList[i].m_WrapU;
			mat.m_Diffuse.m_WrapModeV = (CRenderable_AbstractTexture::WRAP_MODE)m_MaterialList[i].m_WrapV;
			
			m_LoadedMaterials[i] =mat;

			m_TextureMaterialCount --;
			/// checking if it was a last texture to load
			/// if it is we are trying to load geometry now.
			if (!m_TextureMaterialCount ) 
			{
				m_Stage = GEOMETRY;
				return LoadGeometry(in_rRead.m_Magic, m_Read, m_GlobalMark);
			}
		}
	}
	return false;
}


bool CModelContext::LoadGeometry(long magic, const std::vector<unsigned char>& data, int mark)
{
	
	CReply reply = GetReply(magic);
	RemoveReply(magic);

	CEntitySerializer ser(m_LoadedMaterials, m_MaterialList);
	CPreloaderSection_ModelLoadResponse resp;
	resp.m_pObject = ser.LoadObject(data,mark);
	resp.m_Magic = reply.m_Magic;
	CTCommandSender<CPreloaderSection_ModelLoadResponse>::SendCommand(reply.m_OrigSection, resp);
	return true;
}
void CModelContext::Reaction(long src,const CVFSSection_Error<CVFSSection_OpenRequest>& in_rRead)
{
	try {
		switch (m_Stage) 
		{
			case MATERIAL:
				{
					CReply reply= GetReply(in_rRead.m_Magic);
					RemoveReply( in_rRead.m_Magic);

					CPreloaderSection_DecodeError err;
					err.m_Handle = -1;
					err.m_Magic = reply.m_Magic;
					err.m_Message ="Model file not found";
					CTCommandSender<CPreloaderSection_DecodeError>::SendCommand(reply.m_OrigSection, err);
					return ;

				}
				break;
			case NEXT_TEXTURE:
				{
					m_TextureMaterialCount -- ;
					for (int i = 0 ; i < m_MaterialList.size() ; i ++ ) 
					{
						if ("/" + m_MaterialList[i].m_File == in_rRead.m_Req.m_Url ) 
						{
							CRenderable_AbstractMaterial mat;
							mat.m_ARGB = m_MaterialList[i].m_DiffColor;
							mat.m_DiffuseSource = (CRenderable_AbstractMaterial::DS_CONSTANT);;
							m_LoadedMaterials[i] = mat;	
							break;
						}
					}
					if (!m_TextureMaterialCount ) 
					{
						m_Stage = GEOMETRY;
						 LoadGeometry(in_rRead.m_Magic, m_Read, m_GlobalMark);
					}
					return ;
				}
		}


	}
	catch (...) 

	{
		CReply reply = GetReply(in_rRead.m_Magic);
		RemoveReply(in_rRead.m_Magic);

		CPreloaderSection_DecodeError err;
		err.m_Handle = -1;
		err.m_Magic =reply.m_Magic;
		err.m_Message ="Cannot load model";
		
		CTCommandSender<CPreloaderSection_DecodeError>::SendCommand(reply.m_OrigSection, err);
		return ;
	}
	return ;
}

void CModelContext::Reaction(long src, const CVFSSection_ReadResponse& in_rRead)
{
	CVFSSection_CloseRequest close;
	close.m_Handle = in_rRead.m_Handle;
	
	CTCommandSender<CVFSSection_CloseRequest>::SendCommand(m_VFS, close);
	try {
		switch ( m_Stage ) 
		{
			case MATERIAL:
				 LoadMaterial(in_rRead);
				break;
			case NEXT_TEXTURE:
				 LoadTexture(in_rRead);
				break;
		};
	}
	catch (...) 
	{
		CReply reply = GetReply(in_rRead.m_Magic);
		RemoveReply(in_rRead.m_Magic);
		CPreloaderSection_DecodeError err;
		err.m_Handle = in_rRead.m_Handle;
		err.m_Magic = reply.m_Magic;
		err.m_Message ="Cannot load model";
		
		CTCommandSender<CPreloaderSection_DecodeError>::SendCommand(reply.m_OrigSection, err);
		return ;
	}
	return ;
}

void CModelContext::Reaction(long src, const CVFSSection_OpenResponse& in_rRead)
{
	for (int i = 0 ; i < m_MaterialList.size() ; i ++ ) 
	{
		if ("/" + m_MaterialList[i].m_File == in_rRead.m_Url ) 
		{
			m_MaterialList[i].m_Handle = in_rRead.m_Handle;
			break;
		}
	}
	CVFSSection_ReadRequest req(in_rRead.m_Handle);
	req.m_Magic = in_rRead.m_Magic;
	CTCommandSender<CVFSSection_ReadRequest>::SendCommand(m_VFS , req);
	return ;
}
void CModelContext::Reaction(long src, const CModelDecodeRequest& decodeRequest)
{
	m_Stage = MATERIAL;
	m_TextureMaterialCount = 0;

	m_FileName = decodeRequest.m_FileName;
	std::string material = m_FileName+ ".geom";
	CVFSSection_OpenRequest req;
	req.m_Url = material;
	req.m_Magic =CreateReply( decodeRequest.m_Magic , decodeRequest.m_OrigSectionID);
	m_VFS = decodeRequest.m_VFSSection;
	CTCommandSender<CVFSSection_OpenRequest>::SendCommand(decodeRequest.m_VFSSection , req);
	m_Stage = MATERIAL;
	return ;
}


void CModelContext::assert_input(const std::vector<unsigned char>& data, long pos, long size)
{
		if (pos + size > data.size() ) 
		{
			 throw entity_serializer_exception(pos,size);
		}
}
