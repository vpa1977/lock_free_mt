#include "../Include/object_animation.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../Include/e_static_mesh.h"

CObjectAnimationController::CObjectAnimationController()
{

}
CObjectAnimationController::CObjectAnimationController(  long rate, bool bSkeletal, const std::list<CObjectAnimation>& in_rDesc)
: m_Animation(in_rDesc), m_bPlay(false), m_FrameRate(rate), m_bSkeletal(bSkeletal),  m_CurrentFrame(0)
{
	m_bSeekPending = false;
	m_FrameCount = 0;
	std::list<CObjectAnimation>::const_iterator it;
	for  (it   = m_Animation.begin();it != m_Animation.end();it++ ) 
	{
		const CObjectAnimation& anim = *it;
		if (anim.m_AnimationData.size()) 
		{
			long maxFrame = anim.m_AnimationData.rbegin()->m_FrameID;
			if (m_FrameCount < maxFrame ) 
					m_FrameCount = maxFrame;
		}
	}
}
CObjectAnimationController::~CObjectAnimationController()
{
}
bool CObjectAnimationController::UpdateSkeletal(CObject& in_rHost)
{
		if (in_rHost.GetEntity()->GetType() == ET_STATIC_MESH ) 
		{
			CEntity_StaticMesh* pMesh = (CEntity_StaticMesh*)in_rHost.GetEntity();
			CSkeleton& skeleton  = pMesh->GetSkeleton();
			const std::vector< CBone* >& bonez = skeleton.GetBoneIDs();
			std::list<CObjectAnimation>::iterator it = m_Animation.begin();
			while (it != m_Animation.end())
			{
				CObjectAnimation& current = *it;
				if (current.m_BoneID >= bonez.size() ) 
				{
					assert(0);
					continue;
				}
				CBone * ptr = bonez[ current.m_BoneID ];
				CElementHandle<CBonePosition> Handle = ptr->m_Position.Get();
				CBonePosition& pos = Handle;
				
				pos.SetPosition((*it).Get());
				(*it).Next();
			//	CMatrix newMatrix = (*it).Get();
				//if (newMatrix != oldMatrix) 
				//{
				//	printf("HI");
				//}
				it++;
			}
			return true;
		}
		return false;
}
bool CObjectAnimationController::UpdateObject(CObject& in_rHost , CObjectAnimation& current)
{
	if (in_rHost.GetName() == current.m_ObjectName) 
	{
		CElementHandle<CMatrix> Handle = in_rHost.m_Position.Get();
		Handle =    current.Get()*current.m_InitialMatrix;
		return true;
	}
	const std::vector<CObject*>& children =  in_rHost.GetChildren();
	for (unsigned int i= 0 ; i < children.size() ; i ++ ) 
	{
		if (UpdateObject(*children[i], current))
		{
			return true;
		}
	}
	return false;
}
void CObjectAnimationController::UpdateInitialMatrices(CObject& in_rHost)
{
	if (m_bSkeletal) 
	{
		if (in_rHost.GetEntity()->GetType() == ET_STATIC_MESH ) 
		{
			CEntity_StaticMesh* pMesh = (CEntity_StaticMesh*)in_rHost.GetEntity();
			 CSkeleton& skeleton  = pMesh->GetSkeleton();
			const std::vector< CBone* >& bonez = skeleton.GetBoneIDs();
			std::list<CObjectAnimation>::iterator it = m_Animation.begin();
			while (it != m_Animation.end())
			{
				CObjectAnimation& current = *it;
				CElementHandle<CBonePosition> Handle =  bonez[ current.m_BoneID ]->m_Position.Get();
				CBonePosition& pos = Handle;
				current.m_InitialMatrix =pos.GetPosition();
				it++;
			}
			return ;
		}
	}
	else
	{
		std::list<CObjectAnimation>::iterator it = m_Animation.begin();
		while (it != m_Animation.end())
		{
			CObjectAnimation& current = *it;
			if (in_rHost.GetName() == current.m_ObjectName) 
			{
				current.m_InitialMatrix = in_rHost.m_Position.Get();
				break;
			}
			it++;
		}
		const std::vector<CObject*>& children =  in_rHost.GetChildren();
		for (unsigned int i= 0 ; i < children.size() ; i ++ ) 
		{
			UpdateInitialMatrices(*children[i]);
		}
	}
}

bool CObjectAnimationController::Process( CObject& in_rHost )
{
	if (m_bSeekPending) 
	{
		m_bSeekPending = false;
		if (m_bSkeletal) 
		{
			return UpdateSkeletal(in_rHost);
		}
		else
		{
			std::list<CObjectAnimation>::iterator it = m_Animation.begin();
			while (it != m_Animation.end())
			{
				CObjectAnimation& current = *it;
				if (UpdateObject(in_rHost, current))
				{
					if (!current.Next()){
						return m_bLoop;
					}
				}
				it++;
			}
		}
		return true;
	}
	if (!m_bPlay) return true;

	long clockVal = clock();
	long diff = clockVal - m_stClock;
	
	float elapsed = (float)diff / CLOCKS_PER_SEC;
	if (elapsed < 1.0f/m_FrameRate ) 
	{
		return true;
	}
	m_stClock = clockVal;
	m_CurrentFrame ++;
	if (m_CurrentFrame > m_FrameCount ) 
	{
		// all animations have played
		if (!m_bLoop) 
		{
			Stop();
		}
		m_CurrentFrame = 0;
	}
	if (m_bSkeletal) 
	{
		return UpdateSkeletal(in_rHost);
	}
	else
	{
		std::list<CObjectAnimation>::iterator it = m_Animation.begin();
		while (it != m_Animation.end())
		{
			CObjectAnimation& current = *it;
			if (UpdateObject(in_rHost, current))
			{
				if (!current.Next()){
					return m_bLoop;
				}
			}
			it++;
		}
	}
	return true;
}

bool CObjectAnimationController::Play( long in_AnimationID, bool in_bLoop, float in_StartPositionf )
{
	m_stClock = clock();
	m_bPlay = true;
	m_bLoop = in_bLoop;
	return true;
}
bool CObjectAnimationController::Stop( long in_AnimationID )
{
	m_bPlay = false;
	return true;
}
bool CObjectAnimationController::SeekTo( long in_AnimationID, float in_Where )
{

	return false;
}

void CObjectAnimation::SetFrame(long frame)
{
		m_CurrentFrame = frame -1;
		if (m_Next->m_FrameID >= m_CurrentFrame && m_Prev->m_FrameID <= m_CurrentFrame ) 
		{
			return;
		}
		else
		if (m_Next->m_FrameID < m_CurrentFrame) 
		{
			while (m_Next != m_AnimationData.end() && m_Next->m_FrameID < m_CurrentFrame) 
			{
				m_Next ++;
				m_Prev ++;
			}
		}
		else
		if (m_Prev->m_FrameID > m_CurrentFrame ) 
		{
			m_Prev = m_AnimationData.begin();
			m_Next = m_AnimationData.begin();
			m_Next ++;
			while (m_Prev->m_FrameID >=m_CurrentFrame) 
			{
				m_Prev++;
				m_Next++;
			}
		}
		
}
bool CObjectAnimation::Next()
	{
CLog::Print("Next()\n");
		if (!m_AnimationData.size() ) 
		{
			CLog::Print("Empty animation data");
			return false;
		}
		m_CurrentFrame ++;
		CKeyFrame& prev = *m_Prev;
		if (m_CurrentFrame == prev.m_FrameID ) 
		{
CLog::Print("  ==prev\n");
			SetMatrix(prev.m_Quaternion, prev.m_Position);
			return true;
		}

		CKeyFrame& next = *m_Next;
		if (m_CurrentFrame == next.m_FrameID ) 
		{
CLog::Print("  ==next\n");
			SetMatrix(next.m_Quaternion, next.m_Position);
			return true;
		}
		else
		if (m_CurrentFrame > prev.m_FrameID && m_CurrentFrame < next.m_FrameID)
		{
CLog::Print("  middle\n");
			float f = ( (float)(m_CurrentFrame - prev.m_FrameID) ) / ( (float)(next.m_FrameID - prev.m_FrameID) );
			CQuaternion q = SLerp( prev.m_Quaternion, next.m_Quaternion, f );
			q.Normalize();
			CVector v = Lerp( prev.m_Position, next.m_Position, f );
            SetMatrix( q , v);
			return true;

		}
		else
		{
CLog::Print("  outofrange\n");
			m_CurrentFrame --;
			m_Prev++;
			if (m_Prev == m_AnimationData.end())
			{
				m_Prev = m_AnimationData.begin();
				m_Next = m_Prev;
				m_CurrentFrame = -1;
			}
			m_Next++;
			if (m_Next == m_AnimationData.end())
			{
				m_Next = m_AnimationData.begin();
			}
			Next();
			return false;
		}
	}

	void CObjectAnimation::SetMatrix(CQuaternion q, CVector v)
	{
			m_CurrentMatrix = q.ToMatrix();
			m_CurrentMatrix.m[12] = v.x;
			m_CurrentMatrix.m[13] = v.y;
			m_CurrentMatrix.m[14] = v.z;
			CLog::Print("  %f %f %f %f\n",m_CurrentMatrix.m[0],m_CurrentMatrix.m[1],m_CurrentMatrix.m[2],m_CurrentMatrix.m[3]);
			CLog::Print("  %f %f %f %f\n",m_CurrentMatrix.m[4],m_CurrentMatrix.m[5],m_CurrentMatrix.m[6],m_CurrentMatrix.m[7]);
			CLog::Print("  %f %f %f %f\n",m_CurrentMatrix.m[8],m_CurrentMatrix.m[9],m_CurrentMatrix.m[10],m_CurrentMatrix.m[11]);
			CLog::Print("  %f %f %f %f\n",m_CurrentMatrix.m[12],m_CurrentMatrix.m[13],m_CurrentMatrix.m[14],m_CurrentMatrix.m[15]);
	}

