#ifndef EXPORT_SCENE_ENTITY

#define EXPORT_SCENE_ENTITY




struct CRenderable_ExportMaterial 
{
	std::string m_Name;
	std::string m_File;

	long m_DiffuseColor;
	CRenderable_AbstractMaterial::DIFFUSE_SOURCE m_DiffuseSource;
	CRenderable_AbstractTexture::WRAP_MODE m_WrapU;
	CRenderable_AbstractTexture::WRAP_MODE m_WrapV;
};

struct CRenderable_Export : public CRenderable_BoneRemap
{
	CRenderable_ExportMaterial m_ExportMaterial;
};

struct CSerializationEntity 
{
	CSerializationEntity()
	{
		m_EntityName = "";
		globalScale=1;
	}

	CMatrix m_BindPos;
	long m_FrameRate;
	std::string m_EntityName;
	CMatrix m_Pos;
	float globalScale;
	std::vector< CBoneSerialized > m_Bones;
	std::vector< CRenderable_Export>  m_ExportShapes;
	std::vector< CRenderable_ExportMaterial> m_Materials;
};

struct CSerializationTree
{
	CSerializationEntity m_Entity;
	std::vector<CSerializationTree*> m_Children;
};


#endif