#include "../include/exportscene.h"
#include "../include/entity_writer.h"
#include "../include/animation_transformer.h"
#include "../../Render_DX8/include/algebra.h"


#include "../Include/mesh_optimizer.h"
#include <boost/shared_ptr.hpp>
#include <stdio.h>
#include <map>
#include <string>


////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////



CExportScene::CExportScene(void)
{
	 bFeelingMode = true;
}

CExportScene::~CExportScene(void)
{
}




void CExportScene::Export(IXMLDOMDocument* pDoc, const char* path, const  char* name)
{
	IXMLDOMNode *pNode=NULL;
	IXMLDOMElement *pElement = NULL;
	IXMLDOMNodeList *pList = NULL;

	long len;
	xpathSelect(pDoc, &pElement, L"/COLLADA/asset/contributor/authoring_tool");
	if (!pElement)
	{
		CLog::Print("Tool specification element not found \n");
		printf("Error : only Feeling ColladaMax is supported \n");
		return;
	}
	else
	{
		BSTR bstr;
		//SysAllocString(bstr);
		pElement->get_text(&bstr);
		
		if (wcsstr( bstr,L"3dsMax 9 - Feeling ColladaMax") == NULL)
		{
			printf("Error : only Feeling ColladaMax is supported \n");
			exit(0);

			/*if (wcscmp(L"FBX COLLADA exporter", bstr))
			{
			}
			else
			{
				bFeelingMode = false;
			}*/
		}
	}
	//SysFreeString(bstr);
	// check number of scenes.
	pDoc->selectNodes(L"/COLLADA/library_visual_scenes/visual_scene", &pList);
	pList->get_length(&len);
	assert(len == 1 );
	if (len > 1 ) 
	{
		CLog::Print("Only 1 visual scene is allowed \n");
		return;
	}
	
	pList->Release();

	pDoc->selectNodes(L"/COLLADA/library_visual_scenes/visual_scene/node/node", &pList);
	pList->get_length(&len);
	pList->Release();
	if (bFeelingMode) 
		pDoc->selectNodes(L"/COLLADA/library_visual_scenes/visual_scene/node[@type=\"NODE\"]", &pList);
	else 
		pDoc->selectNodes(L"/COLLADA/library_visual_scenes/visual_scene/node", &pList);
	//assert(len == 1 );
	XMLNodeEnumerator en(pList);
	wchar_t* rootNodeId = getAttribute(en[0], L"id");
	len+= en.size();	
	

	 

	//check if it contains animation
	bool hasAnimation = false;
	pElement = NULL;
	xpathSelect(pDoc, &pElement, L"/COLLADA/library_visual_scenes/visual_scene/node/instance_controller");
	/*if (pElement && !bFeelingMode) 
	{
		CLog::Print("Animation export is supported only for Feeling software exporter\n");
		return;
	}*/
	if (pElement) 
	{
		ExportAnimations(pDoc, path, name);
		hasAnimation = true;
	}
/*	if (len > 2 && hasAnimation) 
	{
		CLog::Print("Only 1 model element is allowed \n");
		return;
	}*/
	
	if (!hasAnimation) 
	{
		CSerializationTree m_Tree;
		if (bFeelingMode) 
			pDoc->selectNodes(L"/COLLADA/library_visual_scenes/visual_scene/node[@type=\"NODE\"]", &pList);
		else // max doesnt write type to geometry nodes.
			pDoc->selectNodes(L"/COLLADA/library_visual_scenes/visual_scene/node[not(@type)]", &pList);
		XMLNodeEnumerator en(pList);
		m_Tree.m_Entity.m_EntityName = "root";
		
		
		for (unsigned int i = 0 ; i < en.size() ; i ++ ) 
		{
			CSerializationTree* next  = new CSerializationTree();
			IXMLDOMElement* pNext = ToElement(en[i]);
			ExportTree(pDoc,pNext, next);
			pNext->Release();
			m_Tree.m_Children.push_back(next);
		}

	
 		ExportMaterials(pDoc, path);

		// set the global scale
		xpathSelect(pDoc, &pElement,L"/COLLADA/asset/unit");
		

		m_Tree.m_Entity.globalScale =(float) _wtof(nocomma(getAttribute(pElement, L"meter")));
		
		// save to the file !!!
		CEntityWriter writer;
		std::vector<unsigned char> geomtery = writer.SaveTree(m_Tree);
		std::vector<unsigned char> mat = writer.SaveMaterials(m_Entity);
	
		std::string geometryName = std::string(path) + "\\" + std::string(name) + ".geom";
		std::string matrixName =  std::string(path) + "\\" + std::string(name) + ".anim";

		FILE * f = fopen(geometryName.c_str(), "wb");
		fwrite(&mat[0], 1, mat.size(), f);
		fwrite(&geomtery[0], 1, geomtery.size(), f);
		fclose(f);


		// export matrix animation
		IXMLDOMElement* pAnimationLibrary = NULL;
		xpathSelect(pDoc, &pAnimationLibrary, L"/COLLADA/library_animations");
		std::map<std::string, std::vector<float> > animations;
		if (pAnimationLibrary)
		{
			AnimationLoadAnimations(pAnimationLibrary, animations);
		}
		IXMLDOMElement *pFrameRate;
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
		//std::map<std::string, std::vector<float> >::iterator it = animations.begin();
        f = fopen(matrixName.c_str() , "wb");		
		std::vector<unsigned char> matrices = writer.SaveMatrices(frame_rate,animations);
		fwrite(&matrices[0],1, matrices.size() , f);
		fclose(f);
		return;

	}
	else
	{
		m_Entity.m_EntityName = wcompress(rootNodeId);
		wchar_t* mesh_source;
//		if (hasAnimation) 
//		{
			xpathSelect(pDoc, &pElement, L"/COLLADA/library_controllers/controller/skin");
			mesh_source = getAttribute(pElement,L"source") + 1;
			pElement->Release();
//		}
//		else
//		{
//			xpathSelect(pDoc, &pElement, L"/COLLADA/library_visual_scenes/visual_scene/node/instance_geometry");
//			mesh_source = getAttribute(pElement,L"url") + 1;
//			pElement->Release();
//		}

		wchar_t buffer[1024];
		swprintf(buffer, L"/COLLADA/library_geometries/geometry[@id=\"%s\"]", mesh_source);
		xpathSelect(pDoc, &pElement, buffer);
		createGeometry(m_Entity,pDoc, pElement);
	}

	ExportMaterials(pDoc, path);

	// set the global scale
	xpathSelect(pDoc, &pElement,L"/COLLADA/asset/unit");
	m_Entity.globalScale =(float) _wtof(nocomma(getAttribute(pElement, L"meter")));

	
	// save to the file !!!
	CEntityWriter writer;
	std::vector<unsigned char> geomtery = writer.SaveEntity(m_Entity);
	std::vector<unsigned char> mat = writer.SaveMaterials(m_Entity);

	std::string geometryName = std::string(path) + "\\" + std::string(name) + ".geom";

	FILE * f = fopen(geometryName.c_str(), "wb");
	fwrite(&mat[0], 1, mat.size(), f);
	fwrite(&geomtery[0], 1, geomtery.size(), f);
	fclose(f);



//	if (hasAnimation)
//	{
		long frame_rate = 30;
		if (bFeelingMode) 
		{
			IXMLDOMElement *pFrameRate;
			xpathSelect(pDoc, &pFrameRate, L"/COLLADA/library_visual_scenes/visual_scene/extra/technique/frame_rate");
			assert(pFrameRate);
			BSTR text;
			pFrameRate->get_text(&text);
			swscanf(nocomma(text), L"%d",  &frame_rate);
			pFrameRate->Release();
		}
		std::string animName = std::string(path) + "\\" + std::string(name) + ".anim";
		std::vector<unsigned char> anim = writer.SaveAnimations(frame_rate,m_Entity);
		f = fopen(animName.c_str(), "wb");
		fwrite(&anim[0], 1, anim.size(), f);
		fclose(f);

//	}


}

void CExportScene::ExportTree(IXMLDOMDocument* pDoc,IXMLDOMElement* pRoot, CSerializationTree* tree)
{
	IXMLDOMElement *pElement = NULL;
	tree->m_Entity.m_EntityName  = wcompress(getAttribute(pRoot, L"id"));
	// export geometry instance
	const char* cStr  = tree->m_Entity.m_EntityName.c_str() ;
	xpathSelect(pRoot, &pElement,L"instance_geometry");
	if (pElement)
	{
		wchar_t* mesh_source = getAttribute(pElement,L"url") + 1;
		pElement->Release();
		wchar_t buffer[1024];
		swprintf(buffer, L"/COLLADA/library_geometries/geometry[@id=\"%s\"]", mesh_source);
		xpathSelect(pDoc, &pElement, buffer);
		createGeometry(tree->m_Entity,pDoc, pElement);
		pElement->Release();
	}

	IXMLDOMElement *pMatrix = NULL;
	
	xpathSelect(pRoot, &pMatrix, L"matrix");

	if (pMatrix)
	{
		tree->m_Entity.m_Pos	 = constructMatrix(pMatrix);
		pMatrix->Release();
	}
	else
	{
		CMatrix matr;
					CLog::Print("Node without matrix - using translate, rotate and scale"); 
			
			XMLNodeEnumerator tr(pRoot,L"translate");
			for (int i = 0 ; i < tr.size() ; i ++ ) 
			{
				 matr*= translate(ToElement(tr[i]));
			}
			XMLNodeEnumerator rt(pRoot,L"rotate");
			for (int i = 0 ; i < rt.size() ; i ++ ) 
			{
				 matr*= rotate(ToElement(rt[i]));
			}
			XMLNodeEnumerator sc(pRoot,L"scale");
			for (int i = 0 ; i < sc.size() ; i ++ ) 
			{
				 matr*= scale(ToElement(sc[i]));
			}
			tree->m_Entity.m_Pos = matr;

	}
	IXMLDOMNodeList *pList = NULL;	
	
	if (bFeelingMode) 
		pRoot->selectNodes(L"node[@type=\"NODE\"]", &pList);
	else
		pRoot->selectNodes(L"node[note(@type)]", &pList);

	if (pList)
	{
		XMLNodeEnumerator en(pList);
		for (unsigned int i = 0 ; i < en.size() ; i ++ ) 
		{
			CSerializationTree* t= new CSerializationTree();
			IXMLDOMElement* pElement = ToElement(en[i]);
			ExportTree(pDoc,pElement  , t);
			pElement->Release();
			
			tree->m_Children.push_back(t);
		}
		
	}
	

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
