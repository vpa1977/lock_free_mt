#ifndef MATERIAL_DX8_HPP_INCLUDED
#define MATERIAL_DX8_HPP_INCLUDED

#include "material_texture_dx8.h"
#include <vector>

struct CRenderable_MaterialDX8 {
	std::vector< CTextureDX8_Wrapper > m_Textures;
	unsigned long m_ShaderHandle;
};

#endif



























