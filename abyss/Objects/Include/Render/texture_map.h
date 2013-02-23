#ifndef TEXTURE_MAP_HPP_INCLUDED
#define TEXTURE_MAP_HPP_INCLUDED

#include "boost/shared_ptr.hpp"
#include <vector>


// Texture itself.
// Format is either A8R8G8B8 or X8R8G8B8.
// Thus, m_Data must contain m_Width*m_Height*4 bytes of data.
struct CRenderable_AbstractTextureMap {
	unsigned long m_Width;
	unsigned long m_Height;
	bool m_bHasAlpha;
	std::vector<unsigned long> m_Data;
};

// Texture map.
struct CRenderable_AbstractTexture
{
	CRenderable_AbstractTexture() : m_WrapModeU(WM_REPEAT), m_WrapModeV(WM_REPEAT) {}

	enum WRAP_MODE {
		WM_REPEAT	= 0,
		WM_WRAP		= 1,
		WM_CLAMP	= 2,
		WM_MIRROR	= 3
	};
	WRAP_MODE m_WrapModeU;
	WRAP_MODE m_WrapModeV;
	boost::shared_ptr< CRenderable_AbstractTextureMap > m_pTextureMap;

	bool Present() const { return NULL!=m_pTextureMap.get(); }
};

#endif























