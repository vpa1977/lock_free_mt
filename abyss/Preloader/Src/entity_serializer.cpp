#include "../include/entity_serializer.h"

#include <string> 

using namespace abyss::preloader;

void CEntitySerializer::assert_input(const std::vector<unsigned char>& data, long pos, long size)
{
		if (pos + size > data.size() ) 
		{
			 throw entity_serializer_exception(pos,size);
		}
}


struct DummyTexture : public CRenderable_AbstractTextureMap
{
	DummyTexture()
	{
		m_Width = 2;
		m_Height = 2;
		for (int i = 0 ; i< 16 ; i ++ ) 
		{
			m_Data.push_back(255);
		}
	}
};

boost::shared_ptr<DummyTexture> pGDummyTexture(new DummyTexture());

std::vector<CBone*> CEntitySerializer::BuildBones(const std::vector<CSerializedBone>& bonez, int index)
{

	std::vector<CBone*> result;
	CSerializedBone current;
	current.m_ID = -1;
	for (int i = 0 ; i < bonez.size() ; i ++ ) 
	{
		if (bonez[i].m_Parent == index ) 
		{
			current = bonez[i];
			std::vector<CBone*> children = BuildBones(bonez, current.m_ID);
			CBone* next = new CBone(current.m_ID, current.m_InitialMatrix, current.m_InvBoneMatrix, children, current.m_Name);
			result.push_back(next);
		}
	}
	/* Dump the Nodes */
	
	return result;		
};

DumpChildren(const std::vector<CBone*>& vec, int j =0)
{
	
	for (int i = 0 ; i < vec.size() ; i ++ ) 
	{
		CBone* next = vec[i];
		for (int k = 0 ; k < j ; k ++ ) CLog::Print(" ");
		CLog::Println(next->GetName().c_str());
		DumpChildren(next->GetChildren(),j+1);
	}
}

CObject* CEntitySerializer::LoadObjectTree(const std::vector<unsigned char>& data, long mark)
{
	long ent_size;

	long magic;
	assert_input(data, mark , sizeof(long));
	memcpy(&magic , (char*)&data[mark], sizeof(long));
	mark += sizeof(long);
	assert(magic == 2);
	assert_input(data, mark , sizeof(long));
	memcpy(&ent_size, (char*) &data[mark], sizeof(long));
	mark += sizeof(long);
	//std::vector<unsigned char> child;
	//subscript(data, child, mark, ent_size);
	
	//
	
	long m =mark;
	assert_input(data, m , sizeof(long));
	memcpy(&magic , (char*)&data[m], sizeof(long));
	CMatrix m_Pos;
	std::string name;
	
	
	float globalScale;
	m+=sizeof(long);
	assert_input(data, m , sizeof(CMatrix));
	memcpy(&m_Pos, (char*) &data[m] , sizeof(CMatrix));
	m+= sizeof(CMatrix);

	pop_string(data, name, m);
	m+= name.size() + sizeof(long);
	const char * cPtr = name.c_str();
	assert(name.size());
	assert_input(data, m , sizeof(float));
	memcpy(&globalScale , (char*)&data[m], sizeof(float));

	m+=sizeof(float);

	CEntity * ent = LoadEntity(data, m);

	m_Pos *= CMatrix().ConstructScaling(CVector(globalScale,globalScale,globalScale));
	//
	std::vector<CObject*> children;
	mark += ent_size;
	long childSize = 0;
	assert_input(data, mark , sizeof(long));
	memcpy(&childSize, (char*)&data[mark], sizeof(long));
	mark += sizeof(long);
	for (int i = 0 ; i< childSize ; i ++ ) 
	{
		long subTreeSize;
		assert_input(data, mark , sizeof(long));
		memcpy(&subTreeSize, (char*)&data[mark], sizeof(long));
		mark+= sizeof(long);
		//std::vector<unsigned char> subtree;
		//subscript(data, subtree, mark, subTreeSize);
		children.push_back(LoadObjectTree(data,mark));
		mark+= subTreeSize;
		
	}
	CObject* pObject = new CObject(name, true, m_Pos,children, ent);
	return pObject;
}

CObject* CEntitySerializer::LoadObject(const std::vector<unsigned char>& data, long mark)
{
	if (!data.size() ) 
	{
		return NULL;
	}
	assert(data.size() ) ;
	// load magic
	long magic;
	assert_input(data, mark, sizeof(long));
	memcpy(&magic , (char*)&data[mark], sizeof(long));
	if (magic == 2 ) 
	{
		return LoadObjectTree(data, mark);
	}
	else
	{
		CMatrix m_Pos;
		std::string name;
		
		
		float globalScale;
		long m =mark + sizeof(long);
		assert_input(data, m, sizeof(CMatrix));
		memcpy(&m_Pos, (char*) &data[m] , sizeof(CMatrix));
		m+= sizeof(CMatrix);

		pop_string(data, name, m);
		m+= name.size() + sizeof(long);

		assert_input(data, m,sizeof(float));
		memcpy(&globalScale , (char*)&data[m], sizeof(float));

		m+=sizeof(float);

		CEntity * ent = LoadEntity(data, m, magic ==1);

		std::vector<CObject*> children;
		CMatrix mat;
		mat = mat.ConstructScaling(CVector(globalScale,globalScale,globalScale));
		CObject* pObject = new CObject("", true, mat,children, ent);
		return pObject;
	}
	return NULL;	
}

CEntity* CEntitySerializer::LoadEntity(const std::vector<unsigned char>& data, long m, bool bAnimPresent)
{

	
	long renderable_size = 0;
	
	assert_input(data, m, sizeof(long));
	memcpy(&renderable_size , &data[m] , sizeof(long));
	m += sizeof(long);

	
	std::vector< CRenderable_BoneRemap >  v;
	std::vector< CRenderable_AbstractMaterial> materials;
	for (int i= 0 ; i < renderable_size ; i ++ ) 
	{
		CRenderable_BoneRemap renderable;
		CRenderable_AbstractGeometry geometry;

		std::vector<unsigned char> used_bones;
		long used_bone_size = 0;
		assert_input(data, m, sizeof(long));
		memcpy(&used_bone_size, &data[m], sizeof(long));
		m+= sizeof(long);
		used_bones.resize(used_bone_size);
		if (used_bone_size ) 
		{
			assert_input(data, m, used_bone_size);
			memcpy(&used_bones[0], &data[m], used_bone_size);
			m+= used_bone_size;
		}
		renderable.m_UsedBones = used_bones;
		long geom_size =0;
		assert_input(data, m,  sizeof(long));
		memcpy(&geom_size, &data[m] , sizeof(long));
		m+= sizeof(long);
		for (int geom_index = 0 ; geom_index < geom_size ; geom_index ++ ) 
		{

				long vb_size = 0;
				// load vertex buffers
				assert_input(data, m,  sizeof(long));
				memcpy(& vb_size , &data[m] , sizeof(long));
				m += sizeof(long);
				for (int i = 0 ; i < vb_size ; i ++ ) 
				{
					long buffer_size = 0;
					assert_input(data, m,  sizeof(long));
					memcpy(& buffer_size , &data[m], sizeof(long));
					m += sizeof(long);
					std::vector<unsigned char> vertexes;
					//subscript(data, vertexes, m, buffer_size);
					assert_input(data, m,   buffer_size);
					CRenderable_AbstractVertexBuffer pVertexBuffer = LoadVertexBuffer((const char*)&data[m], buffer_size);
					m += buffer_size;
					geometry.m_VertexBuffers.push_back(pVertexBuffer);	
				}

				long index_size = 0;
				assert_input(data, m,    sizeof(long));
				memcpy(&index_size, &data[m] , sizeof(long));
				m += sizeof(long);
				for (int i = 0 ; i < index_size ; i ++ ) 
				{
					long buffer_size = 0;
					 assert_input(data, m,    sizeof(long));
					memcpy(& buffer_size , &data[m], sizeof(long));
					m += sizeof(long);
					std::vector<unsigned char> indexes;
					//subscript(data, indexes, m, buffer_size);
					assert_input(data, m,   buffer_size);
					CRenderable_AbstractIndexBuffer pIndexBuffer = LoadIndexBuffer((const char*)&data[m], buffer_size);
					m += buffer_size;
					geometry.m_IndexBuffers.push_back(pIndexBuffer);	
				}
				
				long call_size = 0;
				assert_input(data, m,    sizeof(long));
				memcpy(&call_size, &data[m] , sizeof(long));
				m += sizeof(long);

				for (int i = 0 ; i < call_size ; i ++ ) 
				{
					long buffer_size = 0;
					assert_input(data, m,    sizeof(long));
					memcpy(& buffer_size , &data[m], sizeof(long));
					m += sizeof(long);
					std::vector<unsigned char> calls;
					assert_input(data, m,   buffer_size);
					CRenderable_AbstractRenderCall pRenderCall = LoadRenderCall((const char*)&data[m], buffer_size);
					m += buffer_size;
					geometry.m_RenderCalls.push_back(pRenderCall);	
				}
				renderable.m_Geometries.push_back(geometry);
		}
		long mat_size = 0;
		assert_input(data, m,   sizeof(long));
		memcpy(&mat_size, &data[m] , sizeof(long));
		m += sizeof(long);
		std::vector<unsigned char> name;

		subscript(data, name, m, mat_size);
		m+=mat_size;
		name.push_back(0);
		materials.push_back(FindMaterial( std::string( (char*) & name[0])));
		//renderable.m_Geometry = geometry;
		v.push_back(renderable);
	}

	CBone* rootBone = NULL;
	if (bAnimPresent) 
	{
		long bone_size = 0;
		memcpy(&bone_size, &data[m] , sizeof(long));
		m+=sizeof(long);
		std::map<long, CBone*> bones;
		CSerializedBone next;
		std::vector<CSerializedBone> bonez;
		for (int i = 0 ; i < bone_size ; i ++ ) 
		{
			long parentIndex = -1;
			assert_input(data, m,   sizeof(long));
			memcpy(&parentIndex, &data[m] , sizeof(long));
			m+= sizeof(long);
			CMatrix initialMatrix;
				assert_input(data, m,   sizeof(CMatrix));
			memcpy(&initialMatrix, &data[m], sizeof(CMatrix));
			m+= sizeof(CMatrix);
			CMatrix inverseMatrix; 
			assert_input(data, m,   sizeof(CMatrix));
			memcpy(&inverseMatrix, &data[m], sizeof(CMatrix));
			m+= sizeof(CMatrix);
			long myIndex = -1;
			assert_input(data, m,   sizeof(long));
			memcpy(&myIndex, &data[m] , sizeof(long));
			m+= sizeof(long);
			std::string name;
			pop_string(data,name, m);
			m+= name.size()+sizeof(long);
			const char * n = name.c_str();
			next.m_InitialMatrix = initialMatrix;
			next.m_Name = name;
			next.m_Parent = parentIndex;
			next.m_ID = myIndex;
			next.m_InvBoneMatrix = inverseMatrix;
			bonez.push_back(next);
			//next = new CBone(myIndex, initialMatrix, std::vector<CBone>(), name);
			//bonez.push_back(next);
		};
		std::vector<CBone*> result = BuildBones(bonez, -1);
		CLog::Println(" SKELETON IS");
		DumpChildren(result);
		if (result.size() ==1 ) 
		{
			rootBone = result[0];
		}
		else
		{
			CLog::Println("ERROR - more than one or zero root bones found !!!!!");
			assert(0);
		}
	}
	
	CEntity_StaticMesh* mesh = new CEntity_StaticMesh(materials,v, rootBone);
	return mesh;
}
///////////////
CRenderable_AbstractMaterial   CEntitySerializer::FindMaterial(std::string name)
{
	CRenderable_AbstractMaterial mat;
	for (int i = 0 ; i < m_MaterialList.size() ; i ++ ) 
	{
		if (m_MaterialList[i].m_Name == name ) 
		{
			return m_LoadedMaterials[i];
		}
	}
	mat.m_DiffuseSource = CRenderable_AbstractMaterial::DS_TEXTURE;
	CRenderable_AbstractTexture abs;
	abs.m_pTextureMap = pGDummyTexture;
	mat.m_Diffuse = abs;
	return mat;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////
CRenderable_AbstractRenderCall  CEntitySerializer::LoadRenderCall(const char * data, long size)
{	
	CRenderable_AbstractRenderCall call;
	memcpy(&call, data,size);
	return call;
}
CRenderable_AbstractIndexBuffer  CEntitySerializer::LoadIndexBuffer(const char * data, long size)
{
	CRenderable_AbstractIndexBuffer index;
	int indexSize = size/sizeof(long);
	index.m_IndexData.resize(indexSize);
	memcpy(&index.m_IndexData[0], data, size);
	return index;
}

CRenderable_AbstractVertexBuffer CEntitySerializer::LoadVertexBuffer(const char * data, long size)
{
	CVBFormat fmt;
	CRenderable_AbstractVertexBuffer pBuffer;
	
	memcpy(&fmt,  (char*)data , sizeof(CVBFormat));
	pBuffer.m_VertexFormat = fmt;
	int mark  = sizeof(CVBFormat);
	pBuffer.m_VertexData.resize(size -mark);
	memcpy(& pBuffer.m_VertexData[0] , &data[mark], size -mark);
	if (false) 
	{
		float x, float y, float z;
		int point =0;
		int vertexsize = 36;
		for (int i = 0 ; i < pBuffer.m_VertexData.size() ; i += vertexsize ) {
			memcpy(&x , &pBuffer.m_VertexData[i] , sizeof(float));
			memcpy(&y , &pBuffer.m_VertexData[i+sizeof(float)] , sizeof(float));
			memcpy(&z , &pBuffer.m_VertexData[i+sizeof(float)*2] , sizeof(float));
			CLog::Println(" Point %f %f %f" , x, y, z);
		}
	}
	return pBuffer;

}


