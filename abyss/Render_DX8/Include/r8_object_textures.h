#ifndef R8_OBJECT_TEXTURES_HPP_INCLUDED
#define R8_OBJECT_TEXTURES_HPP_INCLUDED

#include "material_texture_dx8.h"
#include "../../Objects/Include/Render/texture_map.h"

#include "r8_texture_manager.h"

#include <boost/shared_ptr.hpp>
#include <map>


class CRenderDX8_ObjectsTexturesContainer : public CTextureContainer {
public:
	CRenderDX8_ObjectsTexturesContainer();
	~CRenderDX8_ObjectsTexturesContainer();

	void SetInterface( CRenderDX8TextureManager* );

	unsigned long UseTexture( CRenderable_AbstractTextureMap* );
	void DecrementUsageCounter( unsigned long );
private:
	CRenderDX8TextureManager*	m_pTextureManager;

	typedef std::map< CRenderable_AbstractTextureMap*, unsigned long > CTextureToHandleConvertMap;
	CTextureToHandleConvertMap m_TextureToHandle;

	typedef std::pair< unsigned long, CRenderable_AbstractTextureMap* > CCounterTexture;
	typedef std::map< unsigned long, CCounterTexture > CHandleToTextureConvertMap;
	CHandleToTextureConvertMap m_HandleToTexture;
};

#endif




















