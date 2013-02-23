#include "../include/exportscene.h"
#include "../include/entity_writer.h"
#include "../include/animation_transformer.h"
#include "../../Render_DX8/include/algebra.h"


#include "../Include/mesh_optimizer.h"
#include <boost/shared_ptr.hpp>
#include <stdio.h>
#include <map>
#include <string>


void CExportScene::ExportMaterials(IXMLDOMDocument *pDoc, const char* outdir)
{
	IXMLDOMElement* pLibraryEffects = NULL;
	xpathSelect(pDoc, &pLibraryEffects , L"/COLLADA/library_effects");
	XMLNodeEnumerator en(pLibraryEffects, L"effect");
	for (unsigned int i = 0 ; i < en.size() ; i ++ ) 
	{
		CRenderable_ExportMaterial mat;
		IXMLDOMElement* next = ToElement(en[i]);
		if (!next ) 
			continue;
		wchar_t* name = getAttribute(next,L"id");
		if (name)
		{
			name[wcslen(name)-3]=0;
		}
		if (name == NULL )
			name = getAttribute(next, L"name");

		mat.m_Name =  std::string(wcompress(name));

		IXMLDOMElement* pTexture = NULL;
		xpathSelect(next, &pTexture, L"profile_COMMON/technique[@sid=\"common\"]/blinn/diffuse/texture");
		if (!pTexture)
		{
			xpathSelect(next, &pTexture, L"profile_COMMON/technique[@sid=\"common\"]/phong/diffuse/texture");
		}
		if (!pTexture) 
		{
			xpathSelect(next, &pTexture, L"profile_COMMON/technique[@sid=\"standard\"]/blinn/diffuse/texture");
		}
		if (!pTexture)
		{
			xpathSelect(next, &pTexture, L"profile_COMMON/technique[@sid=\"standard\"]/phong/diffuse/texture");
		}

		if (!pTexture){
			next->Release();
			continue;
		}
		

		
		IXMLDOMElement *pModeNode = NULL;
		xpathSelect(pTexture, &pModeNode, L"extra/technique[@profile=\"MAYA\"]");
		if (pModeNode ) 
		{
			IXMLDOMElement* pWrapU;
			IXMLDOMElement* pWrapV;
			IXMLDOMElement* pMirrorV;
			IXMLDOMElement* pMirrorU;
			IXMLDOMElement* pRepeatV;
			IXMLDOMElement* pRepeatU;


			xpathSelect(pModeNode, &pWrapU, L"wrapU");
			xpathSelect(pModeNode, &pWrapV, L"wrapV");

			xpathSelect(pModeNode, &pMirrorU, L"mirrorU");
			xpathSelect(pModeNode, &pMirrorV, L"mirrorV");

			xpathSelect(pModeNode, &pRepeatU, L"repeatU");
			xpathSelect(pModeNode, &pRepeatV, L"repeatV");

			if (pWrapU)
			{
				BSTR text;
				pWrapU->get_text(&text);
				if (!wcscmp(text, L"1") || !wcscmp(text, L"TRUE"))
				{
					mat.m_WrapU = CRenderable_AbstractTexture::WM_WRAP;
				}
				pWrapU->Release();
			}
			if (pWrapV)
			{
				BSTR text;
				pWrapV->get_text(&text);
				if (!wcscmp(text, L"1") || !wcscmp(text, L"TRUE"))
				{
					mat.m_WrapV = CRenderable_AbstractTexture::WM_WRAP;
				}
				pWrapV->Release();
			}

			if (pRepeatV)
			{
				BSTR text;
				pRepeatV->get_text(&text);
				if (!wcscmp(text, L"1") || !wcscmp(text, L"TRUE"))
				{
					mat.m_WrapV = CRenderable_AbstractTexture::WM_REPEAT;
				}
				pRepeatV->Release();
			}

			if (pRepeatU)
			{
				BSTR text;
				pRepeatU->get_text(&text);
				if (!wcscmp(text, L"1") || !wcscmp(text, L"TRUE"))
				{
					mat.m_WrapU = CRenderable_AbstractTexture::WM_REPEAT;
				}
				pRepeatU->Release();
			}

			if (pMirrorU)
			{
				BSTR text;
				pMirrorU->get_text(&text);
				if (!wcscmp(text, L"1") || !wcscmp(text, L"TRUE"))
				{
					mat.m_WrapU = CRenderable_AbstractTexture::WM_MIRROR;
				}
				pMirrorU->Release();
			}

			if (pMirrorV)
			{
				BSTR text;
				pMirrorV->get_text(&text);
				if (!wcscmp(text, L"1") || !wcscmp(text, L"TRUE"))
				{
					mat.m_WrapV = CRenderable_AbstractTexture::WM_MIRROR;
				}
				pMirrorV->Release();
			}


			pModeNode->Release();
		}

		mat.m_DiffuseSource = CRenderable_AbstractMaterial::DS_CONSTANT;
		mat.m_DiffuseColor =0xFFFFFFFF;
		


		wchar_t* texSource = getAttribute(pTexture, L"texture");
		wchar_t buffer[1024];
		swprintf(buffer, L"profile_COMMON/newparam[@sid=\"%s\"]/sampler2D/source", texSource);
		IXMLDOMElement* pSource = NULL;
		xpathSelect(next,&pSource, buffer);
		if (pSource) 
		{
			BSTR source;
			pSource->get_text(&source);
			swprintf(buffer, L"profile_COMMON/newparam[@sid=\"%s\"]/surface/init_from", source);
			IXMLDOMElement* pFile = NULL;
			xpathSelect(next, &pFile , buffer);
			if (pFile ) 
			{
				mat.m_DiffuseSource = CRenderable_AbstractMaterial::DS_TEXTURE;
				pFile->get_text(&source);
				mat.m_File = std::string(wcompress(source));
			}
			pSource->Release();
		}
		else
		{
			swprintf(buffer, L"/COLLADA/library_images/image[@id=\"%s\"]/init_from", texSource);
			xpathSelect(next,&pSource, buffer);
			BSTR source;
			if (pSource ) 
			{
				mat.m_DiffuseSource = CRenderable_AbstractMaterial::DS_TEXTURE;
				pSource->get_text(&source);
				mat.m_File = std::string(wcompress(source));
				
			}
			pSource->Release();
		}
		// we have a file for material
		if (mat.m_File.length() ) 
		{
               std::string file_name = mat.m_File;
			 /*  int start = mat.m_File.find_last_of("\\");
			   int start2  = mat.m_File.find_last_of("/");
			   start = max(start, start2);
			   if (start >= 0 ) 
			   {
					file_name = mat.m_File.substr(start+1, (mat.m_File.length()-start-1));
			   }*/
			   mat.m_File = file_name;
		}

		m_Entity.m_Materials.push_back(mat);

		pTexture->Release();
		next->Release();
	}

}
