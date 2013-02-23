#ifndef MESH_OPTIMIZER_HPP

#define MESH_OPTIMIZER_HPP
#include <string>
#include <windows.h>
#include <list>
#include <vector>
#include <map>
#include "../../Render_DX8/include/algebra.h"
#include "../../objects/include/object.h"
#include "../../objects/include/render/geometry.h"
#include "../../objects/include/render/renderable.h"
#include "../../objects/include/e_static_mesh.h"
#include "animation.h"
#include "ExportScene.h"


struct CTriangle 
{
	CTriangle()
	{
	}
	CTriangle(unsigned int vertex_size, unsigned int bone_offset,unsigned long max_bones, const std::vector<unsigned char>& in ) 

		: m_Vertexes(in) , m_MaxBones(max_bones)
	{
		for (int j = 0 ; j < 3 ; j ++ ) 
		{
			unsigned char* ptr = (unsigned char*)&in[j*vertex_size + bone_offset];
			for (int i = 0 ; i < m_MaxBones ; i ++ ) 
			{
				//assert( *(ptr + i) < 99);
				m_Bones.insert(*(ptr+i));
			}
		}
		m_VertexSize = vertex_size;
		m_BoneOffset = bone_offset;
		CLog::Print("Used Bones \n");
		std::set<unsigned char>::iterator it = m_Bones.begin();
		while (it != m_Bones.end() ) 
		{
			char boneId = (*it);
			CLog::Print("BoneID %d " , (boneId));
			it++;
		}
		CLog::Print("\n");

	}

	bool can_merge(const CTriangle* in ) 
	{
		std::set<unsigned char> test;
		test.insert( in->m_Bones.begin() , in->m_Bones.end() ) ;
		test.insert( m_Bones.begin() , m_Bones.end() ) ;
		bool canMarge = test.size() < 20;
		if (!canMarge) 
		{
			CLog::Print("cannot merge");
		}
		return canMarge;
	}

	int compare(const CTriangle* in, const std::set<unsigned char>& com_set ) 
	{
		std::set<unsigned char> test;
		test.insert( in->m_Bones.begin() , in->m_Bones.end() ) ;
		test.insert( com_set.begin() , com_set.end() ) ;

		return test.size() - com_set.size();
		
	}

	void merge(const CTriangle* in ) 
	{
		m_Vertexes.insert(m_Vertexes.end() , in->m_Vertexes.begin() , in->m_Vertexes.end());
		m_Bones.insert( in->m_Bones.begin() , in->m_Bones.end());
	}

	void fillUsedBones() 
	{
		int max  =0;
		std::set<unsigned char>::iterator it = m_Bones.begin();
		while (it != m_Bones.end() ) 
		{
			unsigned char boneId = (*it);
			if (boneId > max ) 
				max = boneId;
			it++;
		}
		for (int i = 0 ; i <= max/3 ; i++ ) 
			m_UsedBones.push_back(i);

	}

	void remap()
	{
		int i=0;
		std::set<unsigned char>::iterator it = m_Bones.begin();
		while (it != m_Bones.end() ) 
		{
			unsigned char boneId = (*it);
			m_UsedBones.push_back(boneId/3);
			update( boneId, i);
			it++;
			i++;
		}
		CLog::Print("Used Bones in Skeleton " );
		for (int i = 0 ; i < m_UsedBones.size() ;  i ++ ) 
		{
			CLog::Print("%d " , m_UsedBones[i]) ;
		}
		CLog::Print("\n");
	}

	void update( char boneId , char newId ) 
	{
		for (int i = 0 ; i < m_Vertexes.size() ; i += m_VertexSize )
		{
            for (int j = 0 ; j <  m_MaxBones ; j ++ ) 
			{
				if (m_Vertexes[i +m_BoneOffset+  j ] == boneId ) 
				{
					m_Vertexes[i +m_BoneOffset+  j ] = newId*3;
				}
			}
		}
	}
	std::vector<unsigned char> m_UsedBones;
	std::set<unsigned char> m_Bones;
	std::vector<unsigned char> m_Vertexes;
	long m_VertexSize;
	long m_BoneOffset;
	long m_MaxBones;

};


class CMeshOptimizer
{
public:

	std::vector< CTriangle* > m_Triangles;

	virtual ~CMeshOptimizer()
	{
		for (int i = 0 ; i < m_Triangles.size() ; i ++ ) 
			delete m_Triangles[i];
	}
	long get_vertex_size() 
	{
		return m_VertexSize;
	}
	std::vector<CRenderable_AbstractVertexBuffer> SplitMesh(int nVertexes,int indexOffset, CSerializationEntity& entity,const CRenderable_AbstractVertexBuffer& in);

	std::vector<CRenderable_AbstractIndexBuffer> Optimize(int nVertexes,CRenderable_AbstractVertexBuffer& vb,CRenderable_AbstractIndexBuffer& ib);
private:
	std::vector<CTriangle*> merge(const std::vector<CTriangle*> & in, int N);
	std::vector<CTriangle*> optimize(const std::vector<CTriangle*> in , int N);
	bool addIndices(char* unique_bones, long bone);



	void CopyVertex(long index, const std::vector<unsigned char> & inVertexes, std::vector<unsigned char>& outVertexes);
	
	void NormalizeStrip(const std::vector<unsigned char> & inVertexes, 
									const std::vector<unsigned long>& inIndices, 
									std::vector<unsigned char>& outVertexes, 
									std::vector<unsigned long>& outIndices);
	void Normalize(int nVertexes,CRenderable_AbstractVertexBuffer& vb,CRenderable_AbstractIndexBuffer& ib);
	int FindIndice( int i,int k , const std::vector<unsigned char>& newBuffer,const std::vector<unsigned char>& oldBuffer);
	bool Equals(int index1, int index2,  const std::vector<unsigned char>& newBuffer,const std::vector<unsigned char>& oldBuffer);
	long m_VertexSize;
};


#endif