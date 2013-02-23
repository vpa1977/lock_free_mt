#include "../Include/render/animation.h"
#include "../../MT_Core/Include/log.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Quaterion support methods





//////////////////////////////////////////////////////////////////////////////////////////////////////////////

CAnimation::CAnimation()
{
	CLog::Print("CAnimation(%p)::CAnimation(), pOrigin=%p\n",this,(CLinkOrigin*)this);
}

CAnimation::CAnimation( const std::vector< CFrame >& in_Frames ) :
	m_Frames(in_Frames)
{
	CLog::Print("CAnimation(%p)::CAnimation(), pOrigin=%p\n",this,(CLinkOrigin*)this);
}

CAnimation::~CAnimation() {
	CLog::Print("CAnimation(%p)::~CAnimation()\n",this);
	assert( m_ShadowTracker.NoShadowsAlive() );
}

const CFrame& CAnimation::GetFrame(unsigned int frame) 
{
	for (int i = 0 ; i < m_Frames.size() ; i ++ ) 
	{
		if (m_Frames[i].m_Index == frame)
		{
			return m_Frames[i];
		}
		else
		if (m_Frames[i].m_Index > frame  && i > 0) 
		{
			return Interpolate(m_Frames[i-1] , m_Frames[i],frame);
		}
	}
	// frame not found - always return last position 
	return m_Frames[m_Frames.size() - 1];
}


const CFrame& CAnimation::Interpolate(const CFrame& prev, const CFrame& next, int frame )
{
	return prev;
}




