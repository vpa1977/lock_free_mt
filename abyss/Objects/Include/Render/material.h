#ifndef OBJECT_SHADER_HPP_INCLUDED
#define OBJECT_SHADER_HPP_INCLUDED

#include "texture_map.h"



struct CRenderable_AbstractMaterial
{
	enum DIFFUSE_SOURCE {
		DS_TEXTURE,
		DS_VERTEX_COLOR,
		DS_CONSTANT
	};
	
	CRenderable_AbstractMaterial() : 
		m_ARGB(0xffffffff)
		,m_DiffuseSource(DS_CONSTANT)
		,m_bLightingEnabled(false)
		,m_bTwoSided(false)
	{}

	// common for both DIFFUSE and OPACITY
	unsigned long m_ARGB;

	// DIFFUSE :
	CRenderable_AbstractTexture	m_Diffuse;
	DIFFUSE_SOURCE				m_DiffuseSource;

	// OPACITY :
	// opacity map
	// opacity source
	// opacity type - none/filter/add/add_no_alpha/sub/sub_no_alpha

	// normal map (?)
	// lightmap (?)

	// reflection map
	
	// specular level
	// specular power

	// gloss map

	// LIGHTING
	bool m_bLightingEnabled;

	bool m_bTwoSided;
};

#endif



