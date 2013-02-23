#include "../include/exportscene.h"
#include "../include/entity_writer.h"
#include "../include/animation_transformer.h"
#include "../../Render_DX8/include/algebra.h"


#include "../Include/mesh_optimizer.h"
#include <boost/shared_ptr.hpp>
#include <stdio.h>
#include <map>
#include <string>


using namespace std;



void CExportScene::ExportAnimations(IXMLDOMDocument *pDoc, const char* path,const char* name) 
{
	IXMLDOMElement* pVisualScene;
	
	IXMLDOMElement *pFrameRate = NULL;
	IXMLDOMElement *pAnimationLibrary = NULL;
	xpathSelect(pDoc, &pVisualScene, L"/COLLADA/library_visual_scenes/visual_scene");

	std::vector<CBoneSerialized> all_bones;

	
	long frame_rate = 30;
	if (bFeelingMode) 
	{
		xpathSelect(pDoc, &pFrameRate, L"/COLLADA/library_visual_scenes/visual_scene/extra/technique/frame_rate");
		assert(pFrameRate);
		BSTR text;
		pFrameRate->get_text(&text);
		swscanf(nocomma(text), L"%d",  &frame_rate);
		pFrameRate->Release();
	}
	
	// create main root of the skeleton
	IXMLDOMElement* pBindShapeMatrix = NULL;
	xpathSelect(pDoc, &pBindShapeMatrix, L"/COLLADA/library_controllers/controller/skin/bind_shape_matrix");
	CMatrix bindShapeMatrix = constructMatrix(pBindShapeMatrix);
	pBindShapeMatrix->Release();
	m_Entity.m_BindPos = bindShapeMatrix;
	CBoneSerialized pMainRoot(CMatrix(), -1, 0 , "_main_root");
	all_bones.push_back(pMainRoot);

	AnimationAddChidlren(pVisualScene, all_bones.size() -1 , all_bones);
	
	xpathSelect(pDoc, &pAnimationLibrary, L"/COLLADA/library_animations");

	
	 
	if (bFeelingMode) 
	{
		AnimationLoadAnimations(pAnimationLibrary, m_AnimationArray);
	}
	else
	{
		LoadMaxAnimantions(pAnimationLibrary, all_bones, m_AnimationArray);
	}

	
	
	if (pAnimationLibrary) 
	{
		pAnimationLibrary->Release();
	}

	pVisualScene->Release();

	IXMLDOMElement* pSkin = NULL;
	xpathSelect(pDoc, &pSkin , L"/COLLADA/library_controllers/controller/skin");
	
	AnimationCreateBlenders(pDoc,pSkin,all_bones);
	std::map< std::string, std::vector<float> >::iterator it = m_AnimationArray.begin();
	while (it != m_AnimationArray.end() ) 
	{
		CLog::Print("%s\n", it->first.c_str());
		it++;
	}

	AnimationBindAnimation(m_AnimationArray, all_bones);

	pSkin->Release();

	m_Entity.m_Bones = all_bones;
	m_Entity.m_FrameRate = frame_rate;

}


void CExportScene::AnimationCreateBlenders(IXMLDOMDocument* pDoc, IXMLDOMElement* pSkin , std::vector<CBoneSerialized>& all_bones )
{
	IXMLDOMElement* pBoneNames = NULL;

	wchar_t* w_skin_name = getAttribute(pSkin, L"source")+1;
	//char* name = wcompress(getAttribute(pSkin, L"source"))+1;

	IXMLDOMElement *pJointInput;
	IXMLDOMElement *pBindShapeInput;
	IXMLDOMElement *pWeightInput;

	// read bone names and assign indices
	std::vector<float> weights;
	// correct path to bone names  = 
	// <skin>  <joints> <input semantic "JOINT"
	xpathSelect(pSkin, &pJointInput,L"joints/input[@semantic=\"JOINT\"]");
	if (!pJointInput)
	{
		CLog::Print("Cannot find joint definition");
		exit(0);
	}
	wchar_t jointBuf[256];
	wchar_t *pJointSrc =getAttribute(pJointInput, L"source");
	pJointSrc++;
	swprintf(jointBuf, L"source[@id=\"%s\"]/Name_array", pJointSrc);
	xpathSelect(pSkin, &pBoneNames, jointBuf);

	assert(pBoneNames);
	BSTR text;
	pBoneNames->get_text(&text);
	char* s_names_id = wcompress(getAttribute(pBoneNames, L"id"));
	int arrayIndex = 1; // bone 0 is reserved for pseudo root
	
	wchar_t* next_c = wcstok(text, L" \r\n");
	int count =0;
	while (next_c != NULL) 
	{
		std::string name(wcompress(next_c));
		std::vector<CBoneSerialized>::iterator it = all_bones.begin();
		bool remap_done = false;
		
		while (it != all_bones.end())
		{
			const char * s_id = (*it).m_ID.c_str();
			const char*  s_name = name.c_str();
			std::string id  = (*it).m_ID;
			

			if (name == id) 
			{
				count ++;
				(*it).m_Index = arrayIndex;
				remap_done = true;
				break;
			}
			it++;
		}

		if (!remap_done) 
		{
			CLog::Print("Unexpected error - unable to get bone name binds\n");
			assert(0);
			exit(0);

		}
		arrayIndex ++;
		next_c = wcstok(NULL, L" \r\n");
	}
	
	if (count+1!= all_bones.size() ) 
	{
		CLog::Print("Not all bones are used in the animation \n");
		for (int i = 0 ; i < all_bones.size() ; i ++ ) 
		{
			if (all_bones[i].m_Index < 0 ) 
			{
				all_bones[i].m_Index = arrayIndex;
				arrayIndex ++;
				CLog::Print("Bone %s assigned index %d \n", all_bones[i].m_ID.c_str() , all_bones[i].m_Index);
			}

		}
	}


	IXMLDOMElement *pBindPoses = NULL;

	xpathSelect(pSkin, &pBindShapeInput,  L"joints/input[@semantic=\"INV_BIND_MATRIX\"]");
	if (!pBindShapeInput) 
	{
		CLog::Print("Cannot find bind poses for skeleton");
		exit(0);
	}
	wchar_t* pBindSrc= getAttribute(pBindShapeInput, L"source");
	pBindSrc++;
	
	std::vector<float> vPoses;
	wchar_t buffer[1024];
	swprintf((wchar_t*)buffer, L"source[@id=\"%s\"]/float_array", pBindSrc);
   	xpathSelect(pSkin, &pBindPoses, buffer);
	pBindPoses->get_text(&text);
	next_c = wcstok(text, L" \r\n");
	while (next_c)
	{
		float val;
		swscanf(nocomma(next_c), L"%f" , &val);
		vPoses.push_back(val);
		next_c = wcstok(NULL, L" \r\n");
	}
	pBindPoses->Release();
	for (unsigned int i = 0 ; i < vPoses.size() / 16 ; i ++ ) 
	{
		int offset = i * 16;
		int ind = findBone(i+1, all_bones);
		if (ind > 0 ) 
		{
			CBoneSerialized& bone = all_bones[ind];
			bone.m_InvBoneSkinMatrix = CMatrix(&vPoses[offset]);
			bone.m_InvBoneSkinMatrix = bone.m_InvBoneSkinMatrix.Transpose();
		}
		
	}


		


	std::vector<float> v_Weights;
	
	xpathSelect(pSkin, &pWeightInput, L"vertex_weights/input[@semantic=\"WEIGHT\"]");
	if (!pWeightInput)
	{
		CLog::Print("Unable to get vertex weights");
		exit(0);
	}
	wchar_t* pSrc = getAttribute(pWeightInput, L"source");
	pSrc++;
	
	swprintf(buffer, L"source[@id=\"%s\"]/float_array", pSrc);
   	xpathSelect(pSkin, &pBindPoses, buffer);
	if (!pBindPoses) 
	{
		CLog::Print("unable to get Weights array");
		exit(0);
	}
	pBindPoses->get_text(&text);
	next_c = wcstok(text, L" \r\n");
	while (next_c)
	{
		float val;
		swscanf(nocomma(next_c), L"%f" , &val);
		v_Weights.push_back(val);
		next_c = wcstok(NULL, L" \r\n");
	}
	pBindPoses->Release();


	IXMLDOMElement *pWeightNode = NULL;
	IXMLDOMElement *pVNode = NULL;
	IXMLDOMElement *pVCountNode = NULL;

	xpathSelect(pSkin, &pWeightNode , L"vertex_weights");
	xpathSelect(pWeightNode, &pVNode , L"v");
	xpathSelect(pWeightNode, &pVCountNode , L"vcount");

	std::vector<int> vcountArray = read_int_data(pVCountNode); 
	std::vector<int>  vArray = read_int_data(pVNode);

	pVNode->Release();
	pVCountNode->Release();
	pWeightNode->Release();
	
	m_MaxBones = 0;
	for (int i = 0 ; i < vcountArray.size() ; i ++ ) 
	{
		if (vcountArray[i] > m_MaxBones) 
		{
			m_MaxBones = vcountArray[i];
		}
	}
	// max 4 bones
	if (m_MaxBones > 4 ) m_MaxBones = 4;
	assert(m_MaxBones < 5 );

	int vArrayIndex = 0;
	for (unsigned int i = 0 ; i < vcountArray.size() ; i ++ ) 
	{	
		int vcount = vcountArray[i];
		std::vector<int> jointIndices;
		std::vector<int> weightIndices;
		for (int j = 0 ; j < vcount ; j ++ ) 
		{
			jointIndices.push_back( vArray[vArrayIndex] );
			assert(vArray[vArrayIndex] < all_bones.size() -1 );
			weightIndices.push_back(vArray[vArrayIndex + 1 ] );
			vArrayIndex += 2;

		}
		while (jointIndices.size() < m_MaxBones ) jointIndices.push_back(-1);
		while (weightIndices.size() < m_MaxBones ) weightIndices.push_back(-1);

		std::vector<float>  weightValues;
		weightValues.resize(weightIndices.size());
		int bestWeights[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

		for ( unsigned int j = 0 ; j < weightIndices.size() ; j ++ ) 
		{
					if (weightIndices[j] < 0 ||
						weightValues[j] >= v_Weights.size())
						weightValues[j] = 0;
					else
						weightValues[j] = v_Weights[weightIndices[j]];


				if (j >= m_MaxBones)
					{
						float lowestWeight = 1.0f;
						int lowestWeightOverride = 2;
						for (int b = 0; b < m_MaxBones; b++)
							if (lowestWeight > weightValues[bestWeights[b]])
							{
								lowestWeight = weightValues[bestWeights[b]];
								lowestWeightOverride = b;
							} // for if
						// Replace lowest weight
						bestWeights[lowestWeightOverride] = j;
					} // if
		}
				// Now build 2 vectors from the best weights
				float totalWeight = 0;
				for (int i = 0 ; i < m_MaxBones ; i ++ ) 
					totalWeight += weightValues[bestWeights[i]];
				if (!totalWeight) 
				{
					weightValues[bestWeights[0]] = 1.0f;
				}
				else
				{
					for (int i = 0 ; i < m_MaxBones ; i ++ ) 
					{
						weightValues[bestWeights[0]] /= totalWeight;
					}
				}
				for (int i = 0 ; i < m_MaxBones ; i ++) {
					// drop +1 as -1 refers to root bone
					int indice =jointIndices[bestWeights[i]]+1;
					vertexSkinJoints.push_back(indice);
					assert(indice <all_bones.size());
				}
				for (int i = 0 ; i < m_MaxBones ; i ++) {
					float val = weightValues[bestWeights[i]];
					vertexSkinWeights.push_back(val);

				}


		
	}

}

void CExportScene::AnimationBindAnimation(std::map<std::string, std::vector<float> >& animation_source, std::vector<CBoneSerialized>& all_bones)
{
	std::vector<CBoneSerialized>::iterator it = all_bones.begin();
	while (it != all_bones.end())
	{
		std::string sid = (*it).m_Name + "/transform";
		const char* ptr = sid.c_str();

		std::map<std::string, std::vector<float> >::iterator itAnim = animation_source.find(sid);
		
		if ( itAnim== animation_source.end())
		{
			(*it).m_AnimationMatrices.push_back( (*it).m_InitialMatrix );
			
//			(*it)->m_FinalMatrix = (*it)->GetMatrixRecursively();
			it++;
			continue;
		}
//		(*it)->m_FinalMatrix = (*it)->GetMatrixRecursively();
		const std::vector<float>& animation_data = itAnim->second;
		int num_frames = animation_data.size() /16;
		for (int i = 0 ; i < num_frames ; i ++ ) 
		{
			int offset = i * 16;
//			const float *f = &animation_data[offset];
			assert(offset+15 < animation_data.size());
			CMatrix next;
			for (int h= 0 ; h < 16 ; h ++ ) 
			{
				assert(offset + h < animation_data.size());
				next.m[h] = animation_data[offset + h];
			}
			next = next.Transpose();
			(*it).m_AnimationMatrices.push_back(next);
		}
		it++;
	}
}

void CExportScene::LoadMaxAnimantions(IXMLDOMElement* pAnimationLibraryNode, 
										std::vector<CBoneSerialized>& all_bones,
										std::map<std::string, std::vector<float> >& animations) 
{
	if (!pAnimationLibraryNode) 
	{
		// no animations, nothing to do
		return;
	}
	for (int i = 0 ; i < all_bones.size() ; i ++ ) 
	{
		std::string transforms[3] ={ "rotate", "scale", "translate" };
		std::string axis[3] = {"X", "Y", "Z" };
		std::vector<float> rotate[3];
		std::vector<float> scale[3];
		std::vector<float> translate[3];

			for (int trans = 0 ; trans < 3 ; trans ++ )
			{
				for (int ax = 0 ; ax < 3 ; ax ++ ) 
				{
					IXMLDOMElement* channelNode;
					std::string channel = all_bones[i].m_Name + "/" + transforms[trans] + "." + axis[ax];
					if (!trans) channel = all_bones[i].m_Name + "/" + transforms[trans] +  axis[ax]+".ANGLE"; // rotate target has angle in the end
					wchar_t res[2048];
					memset(res, 0, sizeof(wchar_t) *2048);
					mbstowcs( res, channel.c_str(), channel.length() );

					wchar_t buffer[2048];
					swprintf( buffer, L"animation/animation/channel[@target=\"%s\"]", res);
					xpathSelect(pAnimationLibraryNode, &channelNode, buffer);
					if (!channelNode) 
					{
						continue;
					}
					wchar_t* targetSrc = getAttribute(channelNode, L"source");
					targetSrc++;
					swprintf(buffer, L"animation/animation/sampler[@id=\"%s\"]/input[@semantic=\"OUTPUT\"]", targetSrc);
					channelNode->Release();

					IXMLDOMElement* inputNode;
					xpathSelect(pAnimationLibraryNode, &inputNode, buffer);
					if (!inputNode) 
					{
						continue;
					}
					wchar_t* outputSrc = getAttribute(inputNode, L"source");
					outputSrc++;
					swprintf(buffer, L"animation/animation/source[@id=\"%s\"]/float_array", outputSrc);
					inputNode->Release();

					IXMLDOMElement* inputFloatArray;
					xpathSelect(pAnimationLibraryNode, &inputFloatArray, buffer);
					BSTR text;
					inputFloatArray->get_text(&text);
					wchar_t * next = wcstok((wchar_t*)text, L" ");
					while (next) 
					{
							float f;
							swscanf(next, L"%f", &f);
							switch (trans) 
							{
							case 0:
								rotate[ax].push_back(f);
								break;
							case 1: 
								scale[ax].push_back(f);
								break;
							case 2:
								translate[ax].push_back(f);
								break;
							}

							next = wcstok(NULL, L" ");
					}
				}
			}
			std::vector<float> result;
			std::string key=all_bones[i].m_Name + "/transform";
			animations[key] = result;
			std::map<std::string, std::vector<float> >::iterator it = animations.find(key);

			assert( it != animations.end());
			
			// yes potentially a bug but CBA really.
			for (int index = 0 ; index < rotate[0].size() ; index ++ ) 
			{
				CMatrix next;
				next *=CMatrix().ConstructRotationX( rotate[0][index] * CONST_PI/180);
				next *=CMatrix().ConstructRotationY( rotate[1][index] * CONST_PI/180);
				next *=CMatrix().ConstructRotationZ( rotate[2][index] * CONST_PI/180);

				next *=CMatrix().ConstructTranslation( CVector(translate[0][index], translate[1][index], translate[2][index]));
				next *=CMatrix().ConstructScaling( CVector(scale[0][index], scale[1][index], scale[2][index]));
				next = next.Transpose();
				for (int k = 0; k < 16 ; k ++ ) 
				{
					it->second.push_back( next.m[k]);
				}
			}
			
		

	}
}

void CExportScene::AnimationLoadAnimations(IXMLDOMElement* pAnimationNode , std::map<std::string, std::vector<float> >& animations)
{
	IXMLDOMNodeList *pChildren = NULL;
	long len;
	if (!pAnimationNode)
	{
		return;
	}
	pAnimationNode->get_childNodes(&pChildren);
	assert(pChildren);
	pChildren->get_length(&len);
	for (int i = 0 ; i < len ; i ++ ) 
	{
		IXMLDOMNode * pNext;
		pChildren->get_item(i , &pNext);
		IXMLDOMElement *pElement;
		pNext->QueryInterface(IID_IXMLDOMElement, (void**)&pElement);

		BSTR node_name;
		pNext->get_nodeName(&node_name);
		if (!wcscmp(node_name, L"animation"))
		{
			
			AnimationLoadAnimations(pElement, animations);
			
			continue;
		}
		if (wcscmp(node_name, L"channel"))
		{
			continue;
		}

		wchar_t* samplerName = getAttribute(pNext, L"source")+1;
		wchar_t* targetName  = getAttribute(pNext, L"target");
		wchar_t* buffer =(wchar_t*) malloc(sizeof(wchar_t)*1024);
		
		IXMLDOMElement* pSampler;
		swprintf(buffer, L"sampler[@id=\"%s\"]/input[@semantic=\"OUTPUT\"]", samplerName);
		xpathSelect(pAnimationNode,&pSampler, buffer);
		assert(pSampler);
		wchar_t* sourceName = getAttribute(pSampler, L"source")+1;
		pSampler->Release();

		swprintf(buffer, L"source[@id=\"%s\"]/float_array[@id=\"%s-array\"]", sourceName, sourceName);
		IXMLDOMElement* pFloatArrayNode = NULL;
		xpathSelect(pAnimationNode, &pFloatArrayNode, buffer);
		free(buffer);
		
		char * ptr = wcompress(targetName);
		char cbuffer[1024];
		memset(cbuffer, 0, 1024);
		memcpy(cbuffer, ptr, strlen(ptr));


		std::string animationTargetName;
		animationTargetName = (cbuffer);
		if (animations.find(animationTargetName) == animations.end())
		{
			std::vector<float> f;
			animations[animationTargetName] = f;
		}

		
		BSTR float_text;
		pFloatArrayNode->get_text(&float_text);
		std::map< std::string , std::vector<float> >::iterator it = animations.find(animationTargetName);
		if(it == animations.end())
		{
            assert(0);
		}

		wchar_t* next_c = wcstok(float_text, L" \r\n");
		while (next_c)
		{
			float val;
			swscanf(nocomma(next_c), L"%f" , &val);
			it->second.push_back(val);
			next_c = wcstok(NULL, L" \r\n");
		}

		pFloatArrayNode->Release();


		pElement->Release();
		pNext->Release();

	}
	pChildren->Release();


}

void CExportScene::AnimationAddChidlren(IXMLDOMElement *pParent,  int parent, std::vector<CBoneSerialized>& all_bones) 
{
	IXMLDOMNodeList* pNodesList = NULL;
	pParent->selectNodes(L"node[@type=\"JOINT\"]" , &pNodesList);
	long len;
	pNodesList->get_length(&len);
	/*if (parent == NULL && len > 1 ) 
	{
		CLog::Print("Only 1 root bone is allowed in model skeleton !!!\n");
		assert(0);
		exit;
	}*/
	for (int i = 0 ; i < len ; i ++ ) 
	{
		IXMLDOMNode *pNext;
		IXMLDOMElement* pElement = NULL;
		IXMLDOMElement* pMatrix = NULL;

		pNodesList->get_item(i , &pNext);
		pNext->QueryInterface(IID_IXMLDOMElement,(void**) &pElement);

		
		xpathSelect(pElement, &pMatrix, L"matrix");
		CMatrix bonePosition ;
		if (!pMatrix)
		{
			CLog::Print("Node without matrix - using translate, rotate and scale"); 
			
			XMLNodeEnumerator tr(pElement,L"translate");
			for (int i = 0 ; i < tr.size() ; i ++ ) 
			{
				bonePosition*= translate(ToElement(tr[i]));
			}
			XMLNodeEnumerator rt(pElement,L"rotate");
			for (int i = 0 ; i < rt.size() ; i ++ ) 
			{
				bonePosition*= rotate(ToElement(rt[i]));
			}
			XMLNodeEnumerator sc(pElement,L"scale");
			for (int i = 0 ; i < sc.size() ; i ++ ) 
			{
				bonePosition*= scale(ToElement(sc[i]));
			}
			//bonePosition = bonePosition.Transpose();

		}
		else
			bonePosition = constructMatrix(pMatrix);

		if (pMatrix)
		{
			pMatrix->Release();
		}
		const char* long_id = wcompress( getAttribute(pNext, L"id"));
		const char* id = wcompress( getAttribute(pNext, L"sid"));
		if (!id && bFeelingMode)
		{
			CLog::Print("No SID present for %s " , long_id);
			CLog::Print("Bone is skipped \n");
			continue;
		}
		else
		if (!id)
		{
			id = long_id;
		}

		std::string id_str = id;
		CBoneSerialized currentBone( bonePosition , parent,-1, id_str); // set id to -1 - it is changed in remap
		currentBone.m_Name = long_id;
		//currentBone.m_ID_Copy.insert(currentBone->m_ID_Copy.begin(), id_str.begin() , id_str.end());

		all_bones.push_back(currentBone);

		//
		AnimationAddChidlren(pElement, all_bones.size() -1 , all_bones);
		pElement->Release();
		pNext->Release();
	}
	pNodesList->Release();
	
}




