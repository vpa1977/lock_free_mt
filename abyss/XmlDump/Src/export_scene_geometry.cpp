#include "../include/exportscene.h"
#include "../include/entity_writer.h"
#include "../include/animation_transformer.h"
#include "../../Render_DX8/include/algebra.h"


#include "../Include/mesh_optimizer.h"
#include <boost/shared_ptr.hpp>
#include <stdio.h>
#include <map>
#include <string>


void CExportScene::createGeometry(CSerializationEntity& entity,IXMLDOMDocument *pDoc , IXMLDOMElement* pGeomElement	 )
{
	// at the moment we only support mesh/triangles
	IXMLDOMNodeList * pList;
	BSTR bstr = SysAllocString(L"mesh");
	pGeomElement->selectNodes(bstr,&pList);
	SysFreeString(bstr);

	std::vector< CEntity_StaticMesh* > m_MeshCollection;
	long len;
	pList->get_length(&len);
	assert(len == 1);
	if (len > 1 ) 
	{
		CLog::Print("Only 1 mesh is supported\n");
		return;
	}
	for (int i = 0 ; i < len ; i ++ ) 
	{
		IXMLDOMNode * pMeshNode;
		
		pList->get_item(i, &pMeshNode);
		// extract triangles from mesh
//		{
			std::vector<CRenderable_Export > m_RenderableCollection;
			IXMLDOMNodeList * pTriangleList;
			bstr = SysAllocString(L"triangles");
			pMeshNode->selectNodes(bstr, &pTriangleList);
			SysFreeString(bstr);
			
			long triLen;
			pTriangleList->get_length(&triLen);
			for (int j = 0 ; j < triLen ; j ++ ) 
			{
				
				std::vector<CRenderable_Export> export;
				
				IXMLDOMNode* pTriangleNode;
				pTriangleList->get_item(j, &pTriangleNode);
				wchar_t* material_name = getAttribute(pTriangleNode, L"material");
				
				createTriangles(pDoc, pMeshNode,pTriangleNode,export);
				CRenderable_ExportMaterial exportMaterial;
				exportMaterial.m_Name = std::string(wcompress(material_name));
				std::vector<CRenderable_Export>::iterator it = export.begin();
				while (it != export.end()) 
				{
					it->m_ExportMaterial = exportMaterial;
					it++;
				}
				
				//// texture should be read here
				m_RenderableCollection.insert(m_RenderableCollection.end() , export.begin(), export.end());
			}
			pTriangleList->Release();
			// polygons nodes
			XMLNodeEnumerator polygons(pMeshNode, L"polygons");
			for (int i = 0 ; i < polygons.size(); i ++ ) 
			{
				std::vector<CRenderable_Export> export;
				
				IXMLDOMNode* pTriangleNode = polygons[i];

				wchar_t* material_name = getAttribute(pTriangleNode, L"material");
				
				createPolygons(pDoc, pMeshNode,pTriangleNode,export);
				CRenderable_ExportMaterial exportMaterial;
				if (!material_name ) 
				{
					exportMaterial.m_Name ="__dummy_material";
				}
				else
					exportMaterial.m_Name = std::string(wcompress(material_name));
				std::vector<CRenderable_Export>::iterator it = export.begin();
				while (it != export.end()) 
				{
					it->m_ExportMaterial = exportMaterial;
					it++;
				}
				
				//// texture should be read here
				m_RenderableCollection.insert(m_RenderableCollection.end() , export.begin(), export.end());
			}
//		}
		entity.m_ExportShapes = m_RenderableCollection;

	//	pMeshNode->Release();
	}
	

}

void CExportScene::createPolygons(IXMLDOMDocument *pDoc ,IXMLDOMNode* pMeshNode,
								   IXMLDOMNode* pTriangleNode,  std::vector<CRenderable_Export>& export)
{
	CRenderable_AbstractVertexBuffer  vertex_buffer;
	CRenderable_AbstractIndexBuffer index;
	CRenderable_AbstractRenderCall call;
	int num_primitives;
	wchar_t* texture_name = getAttribute(pTriangleNode, L"material");
	wchar_t* num_primitives_str = getAttribute(pTriangleNode, L"count");
	swscanf(num_primitives_str, L"%d" , &num_primitives);

	call.m_NPrimitives = num_primitives;
	call.m_PrimitiveType = TRIANGLE_LIST;
	call.m_IB = -1; // we dont use indexing
	call.m_BaseVertexIndex = 0;
	

	// read inputs 
	// possible semantics
	/*
	BINORMAL Geometric binormal (bitangent) vector
		COLOR Color coordinate vector
		CONTINUITY Continuity constraint at the CV
		IMAGE Raster or MIP-level input
		INPUT Sampler input
		IN_TANGENT Tangent vector for preceding control point
		INTERPOLATION Sampler interpolation type
		INV_BIND_MATRIX Inverse of local-to-world matrix
		JOINT Skin influence identifier
		LINEAR_STEPS Number of piece-wise linear approximation steps to use for the
		spline segment that follows this CV
		MORPH_TARGET Morph targets for mesh morphing
		MORPH_WEIGHT Weights for mesh morphing
		NORMAL Normal vector
		OUTPUT Sampler output
		OUT_TANGENT Tangent vector for succeeding control point
		POSITION Geometric coordinate vector
		TANGENT Geometric tangent vector
		TEXBINORMAL Texture binormal (bitangent) vector
		TEXCOORD Texture coordinate vector
		TEXTANGENT Texture tangent vector
		UV Generic parameter vector
		VERTEX Mesh vertex
		WEIGHT Skin influence weighting valu
	*/
	// reading following semantics
	// VERTEX -> <vertices>element, 
	// TEXCOORD
	// COLOR
	// POSITION
	//IXMLDOMElement* pIndices;
	IXMLDOMElement* pVertexInput; // vertexes
	IXMLDOMElement* pNormalInput; // normals
	IXMLDOMElement* pTextureInput; // texture coordinates
	wchar_t * wcVertexURI, *wcNormalURI, *wcTextureURI;
	unsigned int iVertexOffset = 0, iNormalOffset = 0, iTextureOffset=0;
	xpathSelect(pTriangleNode, &pVertexInput, L"input[@semantic=\"VERTEX\"]");
	xpathSelect(pTriangleNode, &pNormalInput, L"input[@semantic=\"NORMAL\"]");
	xpathSelect(pTriangleNode, &pTextureInput, L"input[@semantic=\"TEXCOORD\"]");

	IXMLDOMNodeList* pInputList;
	long stride = 0;
	BSTR query = SysAllocString(L"input");
	pTriangleNode->selectNodes(query, &pInputList);
	pInputList->get_length(&stride);
	pInputList->Release();
		

	if (!pVertexInput)
	{
		// no triangles here
		return;
	}
	wcVertexURI = getAttribute(pVertexInput, L"source");
	wcVertexURI++;


	wchar_t buffer[1024];
	swprintf(buffer,  L"vertices[@id=\"%s\"]", wcVertexURI);
	IXMLDOMElement* pVerticesElement;
	xpathSelect(pMeshNode, &pVerticesElement, buffer);
	pVertexInput->Release();
	xpathSelect(pVerticesElement, &pVertexInput, L"input[@semantic=\"POSITION\"]");
	pVerticesElement->Release();

	wcVertexURI = getAttribute(pVertexInput , L"source");
	wcVertexURI++;



	//swscanf(getAttribute(pVertexInput, L"offset"), L"%d", &iVertexOffset);

	if (pNormalInput)
	{
		wcNormalURI = getAttribute(pNormalInput, L"source");
		wcNormalURI++;
		swscanf(getAttribute(pNormalInput, L"offset"), L"%d", &iNormalOffset);
	}
	if (pTextureInput) 
	{
		wcTextureURI = getAttribute(pTextureInput, L"source");
		wcTextureURI++;
		swscanf(getAttribute(pTextureInput, L"offset"), L"%d", &iTextureOffset);
	}
	XMLNodeEnumerator pIndexNode(pTriangleNode, L"p");
	if (!pIndexNode.size())
	{
		// no indices defined - exit
		return;
	}
	std::vector<unsigned int> vIndexes;
	for (int i = 0 ; i < pIndexNode.size() ; i ++ ) 
	{
		BSTR text;
		pIndexNode[i]->get_text(&text);
		
		int offset;	
		wchar_t * next = wcstok((wchar_t*)text, L" ");
		while (next) 
		{
					swscanf(next, L"%d", &offset);
					vIndexes.push_back(offset);
					next = wcstok(NULL, L" ");
		}
	}


	CVBFormat format;
	format.m_Diffuse = 0;
	format.m_XYZ = true;
	format.m_Normal = true;
	if (vertexSkinJoints.size() > 0 ) 
	{
		format.m_NBonesPerVertex =m_MaxBones;
	}
	

	IXMLDOMElement * pMeshElement;
	pMeshNode->QueryInterface(IID_IXMLDOMElement ,(void**) &pMeshElement);

	std::map< std::basic_string<wchar_t>  , FloatSource > m_source = create_sources(pMeshElement);

	std::map< std::basic_string<wchar_t>  , FloatSource >::iterator it = m_source.begin();
	while (it != m_source.end())
	{
		std::basic_string<wchar_t>  wc = it->first;

		it++;
	}
	 
	assert(m_source.find(wcVertexURI) != m_source.end());
	assert(m_source.find(wcNormalURI) != m_source.end());
//	assert(m_source.find(wcTextureURI) != m_source.end());

	 FloatSource& pVertexSource = m_source.find(wcVertexURI)->second;


	 FloatSource& pNormalSource = m_source.find(wcNormalURI)->second;
	 FloatSource pTextureSource;
	 if (pTextureInput)
		pTextureSource= m_source.find(wcTextureURI)->second;

	if (pTextureInput)
	{
		int size = pTextureSource.get(0).size();
		switch (size ) 
		{	
			case 1:		format.m_Tex0Dimension = CVBFormat::TEXCOORDSET_ONEDIMENSIONAL;
				break;
			case 2:		format.m_Tex0Dimension = CVBFormat::TEXCOORDSET_FLAT;
				break;
			case 3:		format.m_Tex0Dimension =CVBFormat::TEXCOORDSET_FLAT;// CVBFormat::TEXCOORDSET_CUBIC;
				break;


		};
	}
	else
	{
		format.m_Tex0Dimension = CVBFormat::TEXCOORDSET_NONE;
	}

	int bone_offset = -1;
	pMeshElement->Release();
	int indexSize = vIndexes.size();
	for (int i= 0; i < num_primitives*3 ; i ++ )
	{
		
		int base_index = i*stride;
		int vertex_offset = vIndexes[ base_index + iVertexOffset];
		int textcoord_offset = vIndexes[ base_index + iTextureOffset];
		int normal_offset = vIndexes[base_index + iNormalOffset] ;
		int joint_offset = vIndexes[ base_index + iVertexOffset] * m_MaxBones;

		std::vector<float> vertexData;
		vertexData= pVertexSource.get(vertex_offset);
		std::vector<float> normalData;
		normalData =   pNormalSource.get(normal_offset);
		
		std::vector<float> textureData;
		if (pTextureInput) {
			textureData= pTextureSource.get(textcoord_offset);
			assert(textureData.size());
		}
		// dumping vertex
		assert(normalData.size());
		assert(vertexData.size());
		CVector v(vertexData[0], vertexData[1], vertexData[2]);
		CVector n(normalData[0], normalData[1], normalData[2]);


		if (vertexSkinJoints.size() > 0 ) 
		{
		
			v = v*m_Entity.m_BindPos;
			vertexData[0] = v.x;
			vertexData[1] = v.y;
			vertexData[2] = v.z;


		}
		
		int bufferSize = sizeof(float)* vertexData.size()
			+ sizeof(float)* normalData.size();// +
		   	
		if (format.m_Tex0Dimension ==CVBFormat::TEXCOORDSET_ONEDIMENSIONAL
			
			) 
		{
			bufferSize +=  sizeof(float);
		}
		else
		if (format.m_Tex0Dimension ==CVBFormat::TEXCOORDSET_FLAT) 
		{
			bufferSize +=sizeof(float)* 2;
		}
		if (vertexSkinJoints.size() > 0 ) 
		{
			bufferSize += sizeof(float)*(m_MaxBones-1)+ sizeof(long);
		}

		unsigned char* buffer = (unsigned char*) malloc( bufferSize );
		size_t offset = 0;
		memcpy(buffer+offset, & vertexData[0], sizeof(float)* vertexData.size());
		offset += sizeof(float)* vertexData.size();
		if (vertexSkinJoints.size() > 0 ) 
		{
			long weight = 0;
			char *pWeight = (char*)&weight;
			
			for (int i = 0 ; i< m_MaxBones ; i ++ ) 
			{
				char b =(char) vertexSkinJoints[joint_offset+i]; 
				//pWeight[i] = b*3;
				assert( b < m_Entity.m_Bones.size() );
				weight += (3*b) << (8*i);
			}
			if (m_MaxBones > 1) {
				memcpy(buffer + offset, &vertexSkinWeights[joint_offset], sizeof(float)*(m_MaxBones-1));
				offset+=sizeof(float)*(m_MaxBones-1);
			}
			bone_offset = offset;
			memcpy(buffer + offset, &weight, sizeof(long));
			offset +=sizeof(long);
		}
		memcpy(buffer+offset, & normalData[0], sizeof(float)* normalData.size());
		
		offset +=sizeof(float)* normalData.size();

		if (format.m_Tex0Dimension !=CVBFormat::TEXCOORDSET_NONE)
		{
			if (format.m_Tex0Dimension ==CVBFormat::TEXCOORDSET_ONEDIMENSIONAL) 
			{
				memcpy(buffer+offset  , &textureData[0], sizeof(float)* 1);
			}
			else
			if (format.m_Tex0Dimension ==CVBFormat::TEXCOORDSET_FLAT) 
			{
					memcpy(buffer+offset  , &textureData[0], sizeof(float)*2);
			}
		}

		for (int i = 0 ; i < bufferSize; i ++ )
		{
			vertex_buffer.m_VertexData.push_back(buffer[i]);
		}
		free(buffer);
	}
	CMeshOptimizer opt;

	vertex_buffer.m_VertexFormat = format;
	std::vector<CRenderable_AbstractVertexBuffer> bufs =  opt.SplitMesh(num_primitives* 3 , bone_offset, m_Entity , vertex_buffer);

	for (int k = 0 ; k < bufs.size() ; k ++ ) 
	{
			CRenderable_Export exp;
			CRenderable_AbstractGeometry  geometry;

			vertex_buffer = bufs[k];		
			num_primitives = vertex_buffer.m_VertexData.size() / opt.get_vertex_size() ; 
			num_primitives = num_primitives /3;
			CRenderable_AbstractIndexBuffer index_buffer;
			for (int i = 0 ; i < num_primitives*3  ; i ++ ) 
			{
				index_buffer.m_IndexData.push_back(i);
			}
			vertex_buffer.m_VertexFormat = format;

			// test without optimization
			if (false ) 
			{
				
				geometry.m_VertexBuffers.push_back(vertex_buffer);
				long vb_index = geometry.m_VertexBuffers.size() -1;
				CRenderable_AbstractRenderCall call;
				call.m_PrimitiveType = TRIANGLE_LIST;
				call.m_VB = vb_index;
				call.m_BaseVertexIndex = 0;
				//geometry.m_IndexBuffers.push_back(index_buffer);
				//long ib_index = geometry.m_IndexBuffers.size() -1;
				call.m_IB = -1;
				call.m_NPrimitives = num_primitives;
				geometry.m_RenderCalls.push_back(call);
				exp.m_Geometries.push_back(geometry);
				if (opt.m_Triangles.size())
					exp.m_UsedBones = opt.m_Triangles[k]->m_UsedBones;
				for (int i = 0 ; i < exp.m_UsedBones.size() ; i ++ ) 
				{
					CLog::Print("Used bone %d " , exp.m_UsedBones[i]);
				}

				export.push_back(exp);
				continue;

			}
			
			
			std::vector<CRenderable_AbstractIndexBuffer> ret = opt.Optimize(num_primitives*3, vertex_buffer , index_buffer);
			
			geometry.m_VertexBuffers.push_back(vertex_buffer);
			long vb_index = geometry.m_VertexBuffers.size() -1;
	
			std::vector<CRenderable_AbstractIndexBuffer>::iterator indexes = ret.begin();
			while (indexes != ret.end() ) 
			{
				CRenderable_AbstractRenderCall call;
				
				//call.m_PrimitiveType = TRIANGLE_LIST;
				//call.m_NPrimitives = num_primitives;

				call.m_PrimitiveType = TRIANGLE_STRIP;
				call.m_NPrimitives = indexes->m_IndexData.size() -2;


				call.m_VB = vb_index;
				call.m_BaseVertexIndex = 0;
				geometry.m_IndexBuffers.push_back(*indexes);
				long ib_index = geometry.m_IndexBuffers.size() -1;
				
				call.m_IB = ib_index;
				geometry.m_RenderCalls.push_back(call);
				indexes ++;
			}
			exp.m_Geometries.push_back(geometry);
			if (opt.m_Triangles.size())
				exp.m_UsedBones = opt.m_Triangles[k]->m_UsedBones;
			export.push_back(exp);
	}



}


void CExportScene::createTriangles(IXMLDOMDocument *pDoc ,IXMLDOMNode* pMeshNode,
								   IXMLDOMNode* pTriangleNode,  std::vector<CRenderable_Export>& export)
{
	CRenderable_AbstractVertexBuffer  vertex_buffer;
	CRenderable_AbstractIndexBuffer index;
	
	int num_primitives;
	wchar_t* texture_name = getAttribute(pTriangleNode, L"material");
	wchar_t* num_primitives_str = getAttribute(pTriangleNode, L"count");
	swscanf(num_primitives_str, L"%d" , &num_primitives);


	// read inputs 
	// possible semantics
	/*
	BINORMAL Geometric binormal (bitangent) vector
		COLOR Color coordinate vector
		CONTINUITY Continuity constraint at the CV
		IMAGE Raster or MIP-level input
		INPUT Sampler input
		IN_TANGENT Tangent vector for preceding control point
		INTERPOLATION Sampler interpolation type
		INV_BIND_MATRIX Inverse of local-to-world matrix
		JOINT Skin influence identifier
		LINEAR_STEPS Number of piece-wise linear approximation steps to use for the
		spline segment that follows this CV
		MORPH_TARGET Morph targets for mesh morphing
		MORPH_WEIGHT Weights for mesh morphing
		NORMAL Normal vector
		OUTPUT Sampler output
		OUT_TANGENT Tangent vector for succeeding control point
		POSITION Geometric coordinate vector
		TANGENT Geometric tangent vector
		TEXBINORMAL Texture binormal (bitangent) vector
		TEXCOORD Texture coordinate vector
		TEXTANGENT Texture tangent vector
		UV Generic parameter vector
		VERTEX Mesh vertex
		WEIGHT Skin influence weighting valu
	*/
	// reading following semantics
	// VERTEX -> <vertices>element, 
	// TEXCOORD
	// COLOR
	// POSITION
	IXMLDOMElement* pIndices;
	IXMLDOMElement* pVertexInput; // vertexes
	IXMLDOMElement* pNormalInput; // normals
	IXMLDOMElement* pTextureInput; // texture coordinates
	wchar_t * wcVertexURI, *wcNormalURI, *wcTextureURI;
	unsigned int iVertexOffset = 0, iNormalOffset = 0, iTextureOffset=0;
	xpathSelect(pTriangleNode, &pVertexInput, L"input[@semantic=\"VERTEX\"]");
	xpathSelect(pTriangleNode, &pNormalInput, L"input[@semantic=\"NORMAL\"]");
	xpathSelect(pTriangleNode, &pTextureInput, L"input[@semantic=\"TEXCOORD\"]");

	IXMLDOMNodeList* pInputList;
	long stride = 0;
	BSTR query = SysAllocString(L"input");
	pTriangleNode->selectNodes(query, &pInputList);
	pInputList->get_length(&stride);
	pInputList->Release();
		

	if (!pVertexInput)
	{
		// no triangles here
		return;
	}
	wcVertexURI = getAttribute(pVertexInput, L"source");
	wcVertexURI++;


	wchar_t buffer[1024];
	swprintf(buffer,  L"vertices[@id=\"%s\"]", wcVertexURI);
	IXMLDOMElement* pVerticesElement;
	xpathSelect(pMeshNode, &pVerticesElement, buffer);
	pVertexInput->Release();
	xpathSelect(pVerticesElement, &pVertexInput, L"input[@semantic=\"POSITION\"]");
	pVerticesElement->Release();

	wcVertexURI = getAttribute(pVertexInput , L"source");
	wcVertexURI++;



	//swscanf(getAttribute(pVertexInput, L"offset"), L"%d", &iVertexOffset);

	if (pNormalInput)
	{
		wcNormalURI = getAttribute(pNormalInput, L"source");
		wcNormalURI++;
		swscanf(getAttribute(pNormalInput, L"offset"), L"%d", &iNormalOffset);
	}
	if (pTextureInput) 
	{
		wcTextureURI = getAttribute(pTextureInput, L"source");
		wcTextureURI++;
		swscanf(getAttribute(pTextureInput, L"offset"), L"%d", &iTextureOffset);
	}
	else
	{
		wcTextureURI = wcNormalURI;
	}
	xpathSelect(pTriangleNode, &pIndices, L"p");
	if (!pIndices)
	{
		// no indices defined - exit
		return;
	}
	BSTR text;
	pIndices->get_text(&text);
	std::vector<unsigned int> vIndexes;
	int offset;	
	wchar_t * next = wcstok((wchar_t*)text, L" ");
	while (next) 
	{
				swscanf(next, L"%d", &offset);
				vIndexes.push_back(offset);
				next = wcstok(NULL, L" ");
	}

	CVBFormat format;
	format.m_Diffuse = 0;
	format.m_XYZ = true;
	format.m_Normal = true;
	if (vertexSkinJoints.size() > 0 ) 
	{
		format.m_NBonesPerVertex =m_MaxBones;
	}
	

	IXMLDOMElement * pMeshElement;
	pMeshNode->QueryInterface(IID_IXMLDOMElement ,(void**) &pMeshElement);

	std::map< std::basic_string<wchar_t>  , FloatSource > m_source = create_sources(pMeshElement);

	std::map< std::basic_string<wchar_t>  , FloatSource >::iterator it = m_source.begin();
	 
	assert(m_source.find(wcVertexURI) != m_source.end());
	assert(m_source.find(wcNormalURI) != m_source.end());
//	assert(m_source.find(wcTextureURI) != m_source.end());

	 FloatSource& pVertexSource = m_source.find(wcVertexURI)->second;


	 FloatSource& pNormalSource = m_source.find(wcNormalURI)->second;
 
	 FloatSource& pTextureSource = m_source.find(wcTextureURI)->second;

	if (pTextureInput)
	{
		int size = pTextureSource.get(0).size();
		switch (size ) 
		{	
			case 1:		format.m_Tex0Dimension = CVBFormat::TEXCOORDSET_ONEDIMENSIONAL;
				break;
			case 2:		format.m_Tex0Dimension = CVBFormat::TEXCOORDSET_FLAT;
				break;
			case 3:		format.m_Tex0Dimension =CVBFormat::TEXCOORDSET_FLAT;// CVBFormat::TEXCOORDSET_CUBIC;
				break;


		};
	}
	else
	{
		format.m_Tex0Dimension = CVBFormat::TEXCOORDSET_NONE;
	}
	
	int bone_offset = -1;
	
	pMeshElement->Release();
	int indexSize = vIndexes.size();
	for (int i= 0; i < num_primitives*3 ; i ++ )
	{
		
		int base_index = i*stride;
		int vertex_offset = vIndexes[ base_index + iVertexOffset];
		int textcoord_offset = vIndexes[ base_index + iTextureOffset];
		int normal_offset = vIndexes[base_index + iNormalOffset] ;
		int joint_offset = vIndexes[ base_index + iVertexOffset] * m_MaxBones;

		std::vector<float> vertexData;
		vertexData= pVertexSource.get(vertex_offset);
		std::vector<float> normalData;
		normalData =   pNormalSource.get(normal_offset);
		
		std::vector<float> textureData;
		if (pTextureInput) {
			textureData= pTextureSource.get(textcoord_offset);
			assert(textureData.size());
		}
		// dumping vertex
		assert(normalData.size());
		assert(vertexData.size());
		CVector v(vertexData[0], vertexData[1], vertexData[2]);
		CVector n(normalData[0], normalData[1], normalData[2]);


		if (vertexSkinJoints.size() > 0 ) 
		{
		
			v = v*m_Entity.m_BindPos;
			vertexData[0] = v.x;
			vertexData[1] = v.y;
			vertexData[2] = v.z;


		}
		
		int bufferSize = sizeof(float)* vertexData.size()
			+ sizeof(float)* normalData.size();// +
		   	
		if (format.m_Tex0Dimension ==CVBFormat::TEXCOORDSET_ONEDIMENSIONAL
			
			) 
		{
			bufferSize +=  sizeof(float);
		}
		else
		if (format.m_Tex0Dimension ==CVBFormat::TEXCOORDSET_FLAT) 
		{
			bufferSize +=sizeof(float)* 2;
		}
		if (vertexSkinJoints.size() > 0 ) 
		{
			bufferSize += sizeof(float)*(m_MaxBones-1)+ sizeof(long);
		}

		unsigned char* buffer = (unsigned char*) malloc( bufferSize );
		size_t offset = 0;
		memcpy(buffer+offset, & vertexData[0], sizeof(float)* vertexData.size());
		offset += sizeof(float)* vertexData.size();
		if (vertexSkinJoints.size() > 0 ) 
		{
			long weight = 0;
			char *pWeight = (char*)&weight;
			
			for (int i = 0 ; i< m_MaxBones ; i ++ ) 
			{
				char b =(char) vertexSkinJoints[joint_offset+i]; 
				//pWeight[i] = b*3;
				assert( b < m_Entity.m_Bones.size() );
				weight += (3*b) << (8*i);
			}
			if (m_MaxBones > 1) {
				memcpy(buffer + offset, &vertexSkinWeights[joint_offset], sizeof(float)*(m_MaxBones-1));
				offset+=sizeof(float)*(m_MaxBones-1);
			}
			bone_offset = offset;
			memcpy(buffer + offset, &weight, sizeof(long));
			offset +=sizeof(long);
		}
		memcpy(buffer+offset, & normalData[0], sizeof(float)* normalData.size());
		
		offset +=sizeof(float)* normalData.size();

		if (format.m_Tex0Dimension !=CVBFormat::TEXCOORDSET_NONE)
		{
			if (format.m_Tex0Dimension ==CVBFormat::TEXCOORDSET_ONEDIMENSIONAL) 
			{
				memcpy(buffer+offset  , &textureData[0], sizeof(float)* 1);
			}
			else
			if (format.m_Tex0Dimension ==CVBFormat::TEXCOORDSET_FLAT) 
			{
					memcpy(buffer+offset  , &textureData[0], sizeof(float)*2);
			}
		}

		for (int i = 0 ; i < bufferSize; i ++ )
		{
			vertex_buffer.m_VertexData.push_back(buffer[i]);
		}
		free(buffer);
	}
	CMeshOptimizer opt;

	vertex_buffer.m_VertexFormat = format;
	std::vector<CRenderable_AbstractVertexBuffer> bufs =  opt.SplitMesh(num_primitives* 3 , bone_offset, m_Entity , vertex_buffer);

	for (int k = 0 ; k < bufs.size() ; k ++ ) 
	{
			CRenderable_Export exp;
			CRenderable_AbstractGeometry  geometry;

			vertex_buffer = bufs[k];		
			num_primitives = vertex_buffer.m_VertexData.size() / opt.get_vertex_size() ; 
			num_primitives = num_primitives /3;
			CRenderable_AbstractIndexBuffer index_buffer;
			for (int i = 0 ; i < num_primitives*3  ; i ++ ) 
			{
				index_buffer.m_IndexData.push_back(i);
			}
			vertex_buffer.m_VertexFormat = format;

			// test without optimization
			if (false ) 
			{
				
				geometry.m_VertexBuffers.push_back(vertex_buffer);
				long vb_index = geometry.m_VertexBuffers.size() -1;
				CRenderable_AbstractRenderCall call;
				call.m_PrimitiveType = TRIANGLE_LIST;
				call.m_VB = vb_index;
				call.m_BaseVertexIndex = 0;
				//geometry.m_IndexBuffers.push_back(index_buffer);
				//long ib_index = geometry.m_IndexBuffers.size() -1;
				call.m_IB = -1;
				call.m_NPrimitives = num_primitives;
				geometry.m_RenderCalls.push_back(call);
				exp.m_Geometries.push_back(geometry);
				if (opt.m_Triangles.size())
					exp.m_UsedBones = opt.m_Triangles[k]->m_UsedBones;
				for (int i = 0 ; i < exp.m_UsedBones.size() ; i ++ ) 
				{
					CLog::Print("Used bone %d " , exp.m_UsedBones[i]);
				}

				export.push_back(exp);
				continue;

			}
			
			
			std::vector<CRenderable_AbstractIndexBuffer> ret = opt.Optimize(num_primitives*3, vertex_buffer , index_buffer);
			
			geometry.m_VertexBuffers.push_back(vertex_buffer);
			long vb_index = geometry.m_VertexBuffers.size() -1;
	
			std::vector<CRenderable_AbstractIndexBuffer>::iterator indexes = ret.begin();
			while (indexes != ret.end() ) 
			{
				CRenderable_AbstractRenderCall call;
				
				//call.m_PrimitiveType = TRIANGLE_LIST;
				//call.m_NPrimitives = num_primitives;

				call.m_PrimitiveType = TRIANGLE_STRIP;
				call.m_NPrimitives = indexes->m_IndexData.size() -2;


				call.m_VB = vb_index;
				call.m_BaseVertexIndex = 0;
				geometry.m_IndexBuffers.push_back(*indexes);
				long ib_index = geometry.m_IndexBuffers.size() -1;
				
				call.m_IB = ib_index;
				geometry.m_RenderCalls.push_back(call);
				indexes ++;
			}
			exp.m_Geometries.push_back(geometry);
			if (opt.m_Triangles.size())
				exp.m_UsedBones = opt.m_Triangles[k]->m_UsedBones;
			export.push_back(exp);
	}


}

