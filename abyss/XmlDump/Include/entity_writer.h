#ifndef ENTITY_WRITER_HPP
#define ENTITY_WRITER_HPP

#include <vector>

#include "../../Render_DX8/include/algebra.h"
#include "../../objects/include/object.h"
#include "../../objects/include/render/geometry.h"
#include "../../objects/include/render/renderable.h"
#include "../../objects/include/e_static_mesh.h"
#include "../../objects/include/entity_types.h"
#include "ExportScene.h"
#include "animation_transformer.h"

class CEntityWriter
{



public:
	std::vector<unsigned char> SaveMatrices(long rate, std::map<std::string, std::vector<float> >& matrices);
	std::vector<unsigned char> SaveEntity(CSerializationEntity& entity);
	std::vector<unsigned char> SaveAnimations(long rate, CSerializationEntity& entity);
	std::vector<unsigned char> SaveMaterials(CSerializationEntity& entity);
	std::vector<unsigned char> SaveTree(CSerializationTree& pTree);

private:
	std::vector<unsigned char> SaveRenderCalls(const  CRenderable_AbstractRenderCall& pCall);
	std::vector<unsigned char> SaveIndexes(const  CRenderable_AbstractIndexBuffer& pIndex);
	std::vector<unsigned char> SaveVertexes(const  CRenderable_AbstractVertexBuffer& pVertexBuffer);


	void push_float(std::vector<unsigned char>& in , float data)
	{
		char * ptr = (char*)&data;
		for (int i = 0 ; i < sizeof(float) ; i ++ ) 
			in.push_back(ptr[i]);

	}
	void push_long(std::vector<unsigned char>& in , long data)
	{
		char * ptr = (char*)&data;
		for (int i = 0 ; i < sizeof(long) ; i ++ ) 
			in.push_back(ptr[i]);
	}

	void push_matrix(std::vector<unsigned char>& in , CMatrix data)
	{
		char *ptr = (char*)&data;
		for (int i = 0 ; i < sizeof(CMatrix) ; i ++ )
		{
			in.push_back(ptr[i]);
		}
		
	}
	void push_string(std::vector<unsigned char>& in , std::string data)
	{
		push_long(in, (long)data.length());
		for (unsigned int i = 0 ; i < data.length() ; i++) 
		{
			in.push_back(data[i]);
		}
	}


	void append(const std::vector<unsigned char>& in, std::vector<unsigned char>& out)
	{
		int size = in.size();
		for (unsigned int i = 0 ; i < in.size() ; i++)
			out.push_back(in[i]);
	}

	void subscript(const std::vector<unsigned char>& in, std::vector<unsigned char>& out, long start, long len)
	{
		for (int i = start ; i < start+len ; i ++ ) 
		{
			out.push_back(in[i]);
		}
	}


};

#endif