#include "../Include/mesh_optimizer.h"
#include "../Include/NvTriStrip.h"

#include <list>

std::vector<CRenderable_AbstractVertexBuffer> CMeshOptimizer::SplitMesh(int nVertexes,int bone_offset, CSerializationEntity& entity,const CRenderable_AbstractVertexBuffer& in)
{

	std::vector<unsigned char> data = in.m_VertexData;
	m_VertexSize = in.m_VertexData.size() / nVertexes;

	if (bone_offset < 0 ) 
	{
		std::vector<CRenderable_AbstractVertexBuffer> ret;
		ret.push_back(in);
		return ret;
	}

	unsigned long max_bones = in.m_VertexFormat.m_NBonesPerVertex;
	m_Triangles.clear();

	for (int i = 0 ; i < nVertexes/3 ; i ++ ) 
	{
		std::vector<unsigned char> in;
		in.resize(m_VertexSize * 3) ;
		int pos = i *m_VertexSize * 3;
		memcpy(&in[0] , &data[pos] , m_VertexSize * 3);
		CTriangle* tr =new CTriangle(m_VertexSize, bone_offset,max_bones,  in);
		m_Triangles.push_back(tr);
	}
	
	for (int i = 0 ; i < 24 ; i ++ ) 
	{
		m_Triangles = optimize(m_Triangles, i);
	}

	std::vector<CRenderable_AbstractVertexBuffer> a;
	for (int i = 0 ; i < m_Triangles.size() ; i ++ ) 
	{
		m_Triangles[i]->remap();
		//m_Triangles[i]->fillUsedBones();
		CRenderable_AbstractVertexBuffer vb;
		vb.m_VertexFormat = in.m_VertexFormat;
		vb.m_VertexData = m_Triangles[i]->m_Vertexes;
		a.push_back(vb);
	}
	return a;
}

std::vector<CTriangle*> CMeshOptimizer::merge(const std::vector<CTriangle*> & in, int N) 
{
	std::vector<CTriangle*> out;
	if( !in.size()  || in.size() ==1 )  return in;
	
	CTriangle* current  = in[0];
	const std::set<unsigned char>& comp_set = current->m_Bones;
	bool opt = false;
	for (int i = 1; i < in.size(); i ++ ) 
	{

		if (current->compare(in[i],comp_set) <= N  && current->can_merge(in[i])) 
		{
            current->merge(in[i]);
			delete in[i];
/*			for (int j = i +1; j < in.size() ; j ++ ) 
			{
				out.push_back(in[j]);
			}*/
			opt= true;
			//break;
		}
		else
		{
			out.push_back(in[i]);
		}
	}
	out.push_back(current);
	if (opt)
	for (int i = 0 ; i < N -1 ; i ++ ) 
	{
		out = optimize(out, i);
	}
	return out;
}

std::vector<CTriangle*> CMeshOptimizer::optimize(const std::vector<CTriangle*> in , int N)
{
	std::vector<CTriangle*> out = in; 
	int size_before = out.size();
	int size_after = out.size();
	do
	{
		size_before = out.size();
		out = merge(out,N);
		size_after = out.size();
	} while (size_after != size_before);
	return out;
}




std::vector<CRenderable_AbstractIndexBuffer> CMeshOptimizer::Optimize(int nVertexes,CRenderable_AbstractVertexBuffer& vb,CRenderable_AbstractIndexBuffer& ib)
{

	
	std::vector<CRenderable_AbstractIndexBuffer> retIb;

	
	
	m_VertexSize = vb.m_VertexData.size()  / nVertexes;
	CLog::Print("Size Before normalize %d\n" , vb.m_VertexData.size());
	CLog::Print("IB Size Before normalize %d\n" , ib.m_IndexData.size());

	Normalize(nVertexes,vb, ib);

/*	if (true ) 
	{
		retIb.push_back(ib);
		return retIb; 
	}*/

	PrimitiveGroup* g_pPrimitiveGroupsStrip =NULL;
	unsigned short numSections;
	std::vector<unsigned short> indices;
	for (int i = 0 ; i < ib.m_IndexData.size() ; i ++ ) 
	{
		indices.push_back(ib.m_IndexData[i]);
		if (ib.m_IndexData[i] > 65535 ) 
		{
			// cannot optimize too many vertices.
			assert(0);
			CLog::Print("Too many vertexes per mesh. Maximum allowed - 65535!\n");
			exit(0);
		}
	}
	SetCacheSize(CACHESIZE_GEFORCE3);
	SetListsOnly(false);
	//SetStitchStrips(false);
	GenerateStrips(&indices[0], indices.size(), &g_pPrimitiveGroupsStrip, &numSections);
//	assert(numSections == 1);
	assert(g_pPrimitiveGroupsStrip[0].type == PT_STRIP);
	
		
	//ib.m_IndexData.clear();

	for (int k  = 0 ; k < numSections ; k ++ )
	{
		CRenderable_AbstractIndexBuffer newIb;
		std::vector<unsigned long> vec;
		for (int i = 0 ; i < g_pPrimitiveGroupsStrip[0].numIndices ; i ++ ) 
		{
			vec.push_back(g_pPrimitiveGroupsStrip[0].indices[i]);
			
		}
		
		newIb.m_IndexData = vec;
		retIb.push_back(newIb);
	}

/*
	std::vector<unsigned char> normalVertexes;
	std::vector<unsigned long> normalIndexes;
	NormalizeStrip(vb.m_VertexData, ib.m_IndexData , normalVertexes, normalIndexes);
	ib.m_IndexData = normalIndexes;
	vb.m_VertexData = normalVertexes;
	CLog::Print("Size After normalize %d\n" , vb.m_VertexData.size());
	CLog::Print("IB Size After normalize %d\n" , ib.m_IndexData.size());
*/
	
	delete[] g_pPrimitiveGroupsStrip;
	return retIb;

}
void CMeshOptimizer::NormalizeStrip(const std::vector<unsigned char> & inVertexes, 
									const std::vector<unsigned long>& inIndices, 
									std::vector<unsigned char>& outVertexes, 
									std::vector<unsigned long>& outIndices)
{
	long k = 0;
	for (int i = 0 ; i < inIndices.size() ; i ++ ) 
	{
		long index = inIndices[i];
		long newIndex = FindIndice(index, k, outVertexes, inVertexes);
		if (newIndex < 0 ) 
		{
			CopyVertex(index, inVertexes, outVertexes);
			outIndices.push_back(k);
			k++;
		}
		else
		{
			outIndices.push_back(newIndex);
		}


	}
}
void CMeshOptimizer::CopyVertex(long index, const std::vector<unsigned char> & inVertexes, std::vector<unsigned char>& outVertexes)
{
	for (int i = 0 ; i < m_VertexSize ; i ++ ) 
	{
		outVertexes.push_back( inVertexes[index*m_VertexSize + i]);
	}
}

int CMeshOptimizer::FindIndice( int i,int k, const std::vector<unsigned char>& newBuffer,const std::vector<unsigned char>& oldBuffer )
{
	for (int j = 0 ; j < k ; j ++ )
	{
			if (Equals(j, i, newBuffer, oldBuffer))
			{
				return j;
			}
	}
	return -1;
}


void CMeshOptimizer::Normalize(int nVertexes,CRenderable_AbstractVertexBuffer& vb,CRenderable_AbstractIndexBuffer& ib)
{
	
	std::vector<unsigned char> newVertexData;
	std::vector<unsigned long> newIndexData;
	unsigned int k = 0;
	for (int i = 0; i  < nVertexes ; i ++ ) 
	{
		int indice = FindIndice(i,k, newVertexData, vb.m_VertexData);
		if (indice >= 0)
		{
			newIndexData.push_back(indice);
		}
		else
		{
			newIndexData.push_back(k);
			k++;
			for (int j = 0 ; j < m_VertexSize ; j ++ ) 
			{
				newVertexData.push_back(vb.m_VertexData[ i * m_VertexSize + j]);
			}
		}
	}
	vb.m_VertexData = newVertexData;
	ib.m_IndexData = newIndexData;
}

bool CMeshOptimizer::Equals(int index1, int index2, const std::vector<unsigned char>& newBuffer,const std::vector<unsigned char>& oldBuffer)
{
	for (int i = 0 ; i < m_VertexSize ; i++ )
	{
		if (newBuffer[index1 *m_VertexSize + i] != oldBuffer[index2*m_VertexSize + i])
		{
			return false;
		}
	}
	return true;

}