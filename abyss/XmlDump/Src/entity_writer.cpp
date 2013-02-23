#include "../include/entity_writer.h"
#include <string> 




void CalcFVFandVertexSize( const CVBFormat& in_Format,  unsigned long& out_rVertexSize ) {

	out_rVertexSize = 0;

	if(in_Format.m_NBonesPerVertex > 0) {
		assert(in_Format.m_XYZ);
		out_rVertexSize = 3*sizeof(float);
		out_rVertexSize += (in_Format.m_NBonesPerVertex-1)*sizeof(float);
		out_rVertexSize += sizeof(unsigned long);

	} else if(in_Format.m_XYZ) {
		out_rVertexSize = 3*sizeof(float);

	} else {
		// in_Format.m_XYZRHW
		out_rVertexSize = 4*sizeof(float);
	}

	if(in_Format.m_Normal) {
		out_rVertexSize += 3*sizeof(float);
	}
	if(in_Format.m_Diffuse) {
		out_rVertexSize += sizeof(unsigned long);
	}
	if(in_Format.m_Specular) {
		out_rVertexSize += sizeof(unsigned long);
	}
	unsigned long NTextureCoordSets = 0;
	if(in_Format.m_Tex0Dimension>0) {
		assert(in_Format.m_Tex0Dimension<=3);
		out_rVertexSize += in_Format.m_Tex0Dimension*sizeof(float);
	}
}

void PrintMatrix(CMatrix m ) 
{
	CLog::Print("%f %f %f %f\n", m.m[0],m.m[1],m.m[2],m.m[3]);
	CLog::Print("%f %f %f %f\n", m.m[4],m.m[5],m.m[6],m.m[7]);
	CLog::Print("%f %f %f %f\n", m.m[8],m.m[9],m.m[10],m.m[11]);
	CLog::Print("%f %f %f %f\n", m.m[12],m.m[13],m.m[14],m.m[15]);
}
/*
void PrintBone(CBoneSerialized* ptr ) 
{
	if (true ) return;
	CLog::Print("Bone %s", ptr->m_ID.c_str());
	CLog::Print("==================================================\n");
	CLog::Print("Bone ID %d\n", ptr->m_Index);
	if (ptr->m_pParent)
		CLog::Print("Bone Parent %s, %d\n", ptr->m_pParent->m_ID.c_str() , ptr->m_pParent->m_Index);
	else
		CLog::Print("Bone Parent -1");
	CLog::Print("Initial inverse matrix \n");
	PrintMatrix(ptr->m_InvBoneSkinMatrix);
	CLog::Print("Initial position matrix \n");
	PrintMatrix(ptr->m_InitialMatrix);
	CLog::Print("Validation \n");
	CMatrix m = ptr->m_InvBoneSkinMatrix * ptr->GetMatrixRecursively();
	PrintMatrix(m);
	CLog::Print("==================================================\n");
	



}
*/
void PrintVB(const CRenderable_AbstractVertexBuffer& vb) 
{
	if (true ) return;
	bool hasXYZ;
	bool hasNormal;
	bool hasTexture;
	int nBones;
	CVBFormat fmt = vb.m_VertexFormat;
	nBones = fmt.m_NBonesPerVertex;
	
	CLog::Print("Vertex Buffer \n");
	CLog::Print("==============================================\n");
	CLog::Print("Bones per vertex %d\n" , nBones);
	CLog::Print("===============================================\n");
	unsigned long vertex_size = 0;
	CalcFVFandVertexSize(fmt, vertex_size);
	for (int i = 0 ; i < vb.m_VertexData.size() ; i +=vertex_size ) 
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
		float bWeight;
		char  boneIdx;

		float * ptr = (float*)&vb.m_VertexData[i];
		x = *ptr;
		y = *(ptr+1);
		z = *(ptr+2);
		CLog::Print("V %d: X=%f, Y=%f, Z=%f " , (i/vertex_size), x, y, z);
		for (int k = 0 ; k < nBones -1 ; k ++ ) 
		{
			int offset = 3+k;
			CLog::Print(" Bweight%d=%f ", k, *(ptr + offset));
		}
		char * boneIdxPtr = (char*)((float*)(ptr + 3 + nBones -1));
		if (nBones > 0 ) 
		{
			
			for (int k = 0 ; k < nBones+1 ; k ++ ) 
			{
				CLog::Print(" bIdx%d=%d ", k, (*(boneIdxPtr +k))/3);
				int offset = (*(boneIdxPtr +k))/3;
				
			}
		}
		boneIdxPtr += sizeof(long);
		
		ptr = (float*) boneIdxPtr;

		nx = *ptr;
		ny = *(ptr+1);
		nz = *(ptr+2);

		u = *(ptr+3) ;
		v = *(ptr+4);
		//CLog::Print(" Nx=%f , Ny=%f , Nz=%f , U=%f, V=%f ", nx, ny, nz, u, v);
		CLog::Print("\n");
	}

	CLog::Print("==================================================\n");




}



std::vector<unsigned char> CEntityWriter::SaveAnimations(long rate, CSerializationEntity& entity)
{
	std::vector<unsigned char> data;
	push_long(data, rate);
	push_long(data, 0); // 0 - bone animation
	long size = entity.m_Bones.size();
	//push_long(data, size);
	

	std::vector< CBoneSerialized >::iterator it = entity.m_Bones.begin();
	while (it != entity.m_Bones.end())
	{
		CBoneSerialized& next = (*it);
		push_long(data, (*it).m_Index);

		push_long(data,(*it).m_ID.size());
		data.insert(data.end() ,(*it).m_ID.begin() , (*it).m_ID.end());

		CAnimationTransformer trans;
	
		std::vector<CMatrix>::iterator matrices =  next.m_AnimationMatrices.begin();
		std::vector<float> f;

		while (matrices != next.m_AnimationMatrices.end())
		{
			for (int i = 0 ; i < 16 ; i ++ ) f.push_back( matrices->m[i]);
			matrices++;
		}
		std::vector<CKeyFrame> frames = trans.GetFrames(f);
		//out.resize(sizeof(long) + pos + sizeof(float)*next.size());
		push_long(data,frames.size());
		for (int i= 0 ; i < frames.size() ; i ++ ) 
		{
			std::vector<unsigned char> frame;
			frame.resize(sizeof(CKeyFrame));
			memcpy(&frame[0], & frames[i] , sizeof(CKeyFrame));
			data.insert(data.end(),frame.begin(), frame.end());
		}

		it ++;
	}
	return data;


}
std::vector<unsigned char> CEntityWriter::SaveMaterials(CSerializationEntity& entity)
{
	std::vector<unsigned char> data;
	
	push_long(data, entity.m_Materials.size());
	std::vector<CRenderable_ExportMaterial>::iterator it = entity.m_Materials.begin();
	while (it !=  entity.m_Materials.end())
	{
		push_long(data, (*it).m_Name.size());
		data.insert(data.end() ,(*it).m_Name.begin() , (*it).m_Name.end());
		long diffSource = (*it).m_DiffuseSource;
		long wrapU = (*it).m_WrapU;
		long wrapV = (*it).m_WrapV;
		push_long(data, diffSource);
		push_long(data, wrapU);
		push_long(data, wrapV);
		push_long(data, (*it).m_DiffuseColor);
		push_long(data, (*it).m_File.size());
		data.insert(data.end() ,(*it).m_File.begin() , (*it).m_File.end());
		
		it++;
	}
	return data;
}

std::vector<unsigned char> CEntityWriter::SaveTree(CSerializationTree& pTree)
{
	std::vector<unsigned char> data;
	push_long(data, 2); // object definition
	std::vector<unsigned char> entity = SaveEntity(pTree.m_Entity);
	push_long(data, entity.size());
	data.insert(data.end(), entity.begin() , entity.end());
	push_long(data, pTree.m_Children.size());
	for (unsigned int i = 0 ; i < pTree.m_Children.size() ; i ++ ) 
	{
		std::vector<unsigned char> subtree = SaveTree(*pTree.m_Children[i]);
		push_long(data, subtree.size());
		data.insert(data.end(), subtree.begin() , subtree.end());
	}
	return data;
}

std::vector<unsigned char> CEntityWriter::SaveEntity(CSerializationEntity& pRoot)
{
	std::vector<unsigned char> data;

	// save mesh information. 
	long magic = 0; // static mesh is being saved.
	if (pRoot.m_Bones.size() > 0 ) 
	{
		magic = 1; // animated mesh is being saved.
	}
	push_long(data, magic);
	push_matrix(data, pRoot.m_Pos);
	assert(pRoot.m_EntityName.size()> 0);
	const char * cStr = pRoot.m_EntityName.c_str();

	push_string(data, pRoot.m_EntityName);
	
	push_float(data, pRoot.globalScale);

	// save mesh information
	std::vector<CRenderable_Export >  v = pRoot.m_ExportShapes;
	long num_renderables = v.size();
	push_long(data, num_renderables);
	for (int i= 0 ; i < num_renderables ; i ++ ) 
	{
		const CRenderable_Export& pRenderable  = v[i];
		push_long(data, pRenderable.m_UsedBones.size() );
		append(pRenderable.m_UsedBones, data);
		push_long(data, pRenderable.m_Geometries.size() );
		for (int geomIndex = 0; geomIndex < pRenderable.m_Geometries.size() ; geomIndex ++ ) 
		{
			
			const CRenderable_AbstractGeometry&  pGeometry = pRenderable.m_Geometries[geomIndex];
			push_long(data, pGeometry.m_VertexBuffers.size());	
			for (unsigned int j = 0 ; j < pGeometry.m_VertexBuffers.size() ; j ++ ) 
			{
				std::vector<unsigned char> vertexes;
				const CRenderable_AbstractVertexBuffer& pBuffer = pGeometry.m_VertexBuffers[j];
				vertexes = SaveVertexes(pBuffer);
				long size = vertexes.size();
				push_long(data, size);
				append(vertexes, data);			
			}
			push_long(data, pGeometry.m_IndexBuffers.size());	
			for (unsigned int j = 0 ; j < pGeometry.m_IndexBuffers.size() ; j ++ ) 
			{
				std::vector<unsigned char> indexes;
				const CRenderable_AbstractIndexBuffer& pIndex = pGeometry.m_IndexBuffers[j];
				indexes = SaveIndexes(pIndex);
				long size = indexes.size();
				push_long(data, size);
				append(indexes, data);

			}
			push_long(data,  pGeometry.m_RenderCalls.size());	
			for (unsigned int j = 0 ; j < pGeometry.m_RenderCalls.size() ; j ++ ) 
			{
				std::vector<unsigned char> calls;
				const CRenderable_AbstractRenderCall& pCall = pGeometry.m_RenderCalls[j];
				calls = SaveRenderCalls(pCall);
				long size = calls.size();
				push_long(data, size);
				append(calls, data);
			
			}
		}
		push_long(data, pRenderable.m_ExportMaterial.m_Name.size());
		for (unsigned int i = 0 ; i < pRenderable.m_ExportMaterial.m_Name.size() ; i ++ ) 
		{
			data.push_back(pRenderable.m_ExportMaterial.m_Name[i]);
		}
	}
	// save static bone information
	push_long(data, pRoot.m_Bones.size());
	
	std::vector<CBoneSerialized>& bonez =  pRoot.m_Bones;
	std::vector<CBoneSerialized>::iterator it = bonez.begin();
	while (it != pRoot.m_Bones.end())
	{
		/*
			CBoneSerialized* m_pParent;
			std::list<CBoneSerialized*> m_ChildrenPtr;
			CMatrix m_InitialMatrix;
			CMatrix m_InvBoneSkinMatrix;
			CMatrix m_FinalMatrix;
			CVector m_Pos;
			long m_Index;
			std::string m_ID;
			std::list<CMatrix> m_AnimationMatrices;
		*/
		long parentIndex = -1;
		if ((*it).m_pParent >= 0)
		{
			parentIndex = bonez[(*it).m_pParent].m_Index;
		}
		push_long(data, parentIndex);

		std::vector<unsigned char> matrixVector;
		matrixVector.resize(sizeof(CMatrix));
		

		memcpy(&matrixVector[0] , &(*it).m_InitialMatrix, sizeof(CMatrix));
		data.insert(data.end() , matrixVector.begin(), matrixVector.end());

		memcpy(&matrixVector[0] ,&(*it).m_InvBoneSkinMatrix, sizeof(CMatrix));
		data.insert(data.end() , matrixVector.begin(), matrixVector.end());

	
		push_long(data,(*it).m_Index);
		push_long(data,(*it).m_ID.size());
		assert( (*it).m_ID.size() < 100 ) ;
		CBoneSerialized& pBone = (*it);
		data.insert(data.end(), pBone.m_ID.begin(), pBone.m_ID.end());
		//data.insert(data.end() ,(*it)->m_ID.begin() , (*it)->m_ID.end());
		/*push_long(data, (*it)->m_MatrixMapping.size() );
		
		for (int i = 0 ; i < (*it)->m_MatrixMapping.size() ; i ++ ) 
		{
			data.push_back( (*it)->m_MatrixMapping[i].first );
			data.push_back( (*it)->m_MatrixMapping[i].second );
		}*/
		//PrintBone(*it);
		it++;
	}



	return data;
}


////////////////////////////////////////////////////////////////////////////////////////////
std::vector<unsigned char>  CEntityWriter::SaveRenderCalls(const CRenderable_AbstractRenderCall& pCall)
{
	std::vector<unsigned char> data;
	data.resize(sizeof( CRenderable_AbstractRenderCall));
	memcpy((char*) &(data[0]) , & pCall, sizeof(CRenderable_AbstractRenderCall));
	return data;
}
std::vector<unsigned char>  CEntityWriter::SaveIndexes(const CRenderable_AbstractIndexBuffer& pIndex)
{
	std::vector<unsigned char> data;
	data.resize(pIndex.m_IndexData.size() * sizeof(long));
	memcpy((char*)&data[0] ,(char*) & pIndex.m_IndexData[0], data.size());
	return data;
}
std::vector<unsigned char>  CEntityWriter::SaveVertexes(const CRenderable_AbstractVertexBuffer& pVertexBuffer)
{
	PrintVB(pVertexBuffer);
	std::vector<unsigned char> data;
	data.resize(sizeof(CVBFormat));
	memcpy( (char*)&data[0], (char*)& (pVertexBuffer.m_VertexFormat) , sizeof(CVBFormat));
	for (unsigned int i = 0 ; i < pVertexBuffer.m_VertexData.size() ; i ++ ) 
	{
		data.push_back(pVertexBuffer.m_VertexData[i]);
	}
	return data;
}

std::vector<unsigned char> CEntityWriter::SaveMatrices(long rate, std::map<std::string, std::vector<float> >& animations)
{
		std::vector<unsigned char> out;
		push_long(out, rate);
		push_long(out, 1); // 1 - matrix animation
		std::map<std::string, std::vector<float> >::iterator it = animations.begin();
		CAnimationTransformer trans;
		while (it != animations.end() ) 
		{
			char buffer[200];
			memset(buffer, 0, 200);
			std::string component_name = it->first;
			int pos= component_name.find("/");
			strncpy(buffer, component_name.c_str(), pos);
			component_name = buffer;
			
			push_string(out,buffer);
			std::vector<float>& next = it->second;
			std::vector<CKeyFrame> frames = trans.GetFrames(next);

			//out.resize(sizeof(long) + pos + sizeof(float)*next.size());
			push_long(out,frames.size());
			for (int i= 0 ; i < frames.size() ; i ++ ) 
			{
				std::vector<unsigned char> frame;
				frame.resize(sizeof(CKeyFrame));
				memcpy(&frame[0], & frames[i] , sizeof(CKeyFrame));
				out.insert(out.end(),frame.begin(), frame.end());
			}
			it++;
		}
		return out;
}


