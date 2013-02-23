#ifndef ANIMATION_TRANSFORMER_HPP

#define ANIMATION_TRANSFORMER_HPP

#include "../../Render_DX8/Include/algebra.h"
#include "../../Objects/include/object_animation.h"
#include <vector>



class CAnimationTransformer
{
public:
	std::vector<CKeyFrame> GetFrames( std::vector<float> m_seq);


};

#endif