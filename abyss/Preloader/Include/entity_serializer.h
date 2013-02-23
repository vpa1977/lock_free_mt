#ifndef ENTITY_SERIALIZER_HPP 

#define ENTITY_SERIALIZER_HPP 

#include <vector>

#include "../../Render_DX8/include/algebra.h"
#include "../../objects/include/object.h"
#include "../../objects/include/render/geometry.h"
#include "../../objects/include/render/renderable.h"
#include "../../objects/include/e_static_mesh.h"
#include "../../objects/include/entity_types.h"



namespace abyss {
	namespace preloader {

	struct entity_serializer_exception
	{
		entity_serializer_exception(long p, long s  ) : pos(p) , size(s) 
		{
		}
		long pos; 
		long size;
	};

	
	

/*
		struct CSavedMaterials{
			long m_Count;
			struct CMaterialDescriptor
			{
				long m_NameSize;
				char *m_pName;
				long m_FileSize;
				char *m_pFile;
			}* m_pDesc;
		}
*/
		struct CSavedMaterials 
		{
			long m_WrapU;
			long m_WrapV;
			long m_DiffSource;
			long m_DiffColor;
			long m_Handle;
			long m_Type;
			std::string m_Name;
			std::string m_File;
		};

		struct CSerializedBone
		{
			CMatrix m_InitialMatrix;
			CMatrix m_InvBoneMatrix;
			long m_Parent;
			long m_ID;
			std::string m_Name;
		};


		

		class CEntitySerializer 
		{
		public:
			CEntitySerializer(const std::vector<CRenderable_AbstractMaterial>& in_AbsMat, 
				const std::vector<CSavedMaterials>& in_List) 
				: m_MaterialList(in_List) , m_LoadedMaterials(in_AbsMat)
				{
				}

		public:
			CObject * LoadObject(const std::vector<unsigned char>& data, long mark =0);
			CEntity * LoadEntity(const std::vector<unsigned char>& data, long mark, bool bAnimPresent = false);
			CObject*  LoadObjectTree(const std::vector<unsigned char>& data, long mark);

		private:
			void assert_input(const std::vector<unsigned char>& data, long pos, long size);
			std::vector<CBone*>  BuildBones(const std::vector<CSerializedBone>& bonez, int index);
			CRenderable_AbstractMaterial FindMaterial(std::string name);
			CRenderable_AbstractRenderCall  LoadRenderCall(const char * data, long size);
			CRenderable_AbstractIndexBuffer  LoadIndexBuffer(const char * data, long size);
			CRenderable_AbstractVertexBuffer LoadVertexBuffer(const char * data, long size);

			void push_long(std::vector<unsigned char>& in , long data)
			{
				char * ptr = (char*)&data;
				for (int i = 0 ; i < sizeof(long) ; i ++ ) 
					in.push_back(ptr[i]);
			}


/*			void append(const std::vector<unsigned char>& in, std::vector<unsigned char>& out)
			{
				int size = in.size();
				for (int i = 0 ; i < in.size() ; i++)
					out.push_back(in[i]);
			}
*/
			void subscript(const std::vector<unsigned char>& in, std::vector<unsigned char>& out, long start, long len)
			{
				out.resize(len);
				assert_input(in, start,len);
				memcpy(&out[0], &in[start], len);
				/*
				for (int i = start ; i < start+len ; i ++ ) 
				{
					out.push_back(in[i]);
				}*/
			}

			void pop_string(const std::vector<unsigned char>& in,std::string& ret, int mark)
			{
				long pos = 0;
				assert_input(in, mark, sizeof(long));
				memcpy(&pos, (char*) &in[mark], sizeof(long));
				mark += sizeof(long);
				std::vector<unsigned char> out;
				subscript(in,out,  mark, pos);
				for (unsigned int i = 0 ; i< out.size() ; i ++ ) 
				{
					ret += (char)out[i];
				}
			}


		private:
			std::vector<CRenderable_AbstractMaterial> m_LoadedMaterials;
			std::vector<CSavedMaterials> m_MaterialList;

		};
	}
}

#endif 