#pragma once

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
#include "export_scene_entity.h"
#include "export_scene_util.h"



class CExportScene
{
public:
	CExportScene(void);
	virtual ~CExportScene(void);
	
    void ExportAnimations(IXMLDOMDocument *pDoc, const char* path,  const char* name) ;

	void Export(IXMLDOMDocument *pDoc,  const char* path, const char* name) ;
	
 
	
	void createGeometry(CSerializationEntity& entity,IXMLDOMDocument *pDoc , IXMLDOMElement* pGeomElement	 );
	void createTriangles(IXMLDOMDocument *pDoc ,IXMLDOMNode* pMesheNode,IXMLDOMNode* pTriangleNode, std::vector<CRenderable_Export>& export);
	void createPolygons(IXMLDOMDocument *pDoc ,IXMLDOMNode* pMeshNode,  IXMLDOMNode* pTriangleNode,  std::vector<CRenderable_Export>& export);

private:
	void ExportTree(IXMLDOMDocument* pDoc,IXMLDOMElement* pElement, CSerializationTree* tree);

	void AnimationLoadAnimations(IXMLDOMElement* pAnimationNode , std::map<std::string, std::vector<float> >& animations);
	void AnimationAddChidlren(IXMLDOMElement *pParent, int parent, std::vector<CBoneSerialized>& all_bones);
	void AnimationBindAnimation(std::map<std::string, std::vector<float> >& animation_source, std::vector<CBoneSerialized>& all_bones);
	void AnimationCreateBlenders(IXMLDOMDocument* pDoc, IXMLDOMElement* pControllers , std::vector<CBoneSerialized>& all_bones);
	void LoadMaxAnimantions(IXMLDOMElement* pAnimationLibraryNode, 
										std::vector<CBoneSerialized>& all_bones,
										std::map<std::string, std::vector<float> >& animations) ;

	void ExportMaterials(IXMLDOMDocument* pDoc, const char* outDir);

	std::map<std::string, std::vector<float> > m_AnimationArray;
	CSerializationEntity m_Entity;
	CSerializationTree m_Tree;
	std::vector<long> vertexSkinJoints;
	std::vector<float> vertexSkinWeights;
	bool  bFeelingMode;
	int m_MaxBones;

};


