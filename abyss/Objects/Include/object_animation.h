#ifndef OBJECT_ANIMATION_HPP

#define OBJECT_ANIMATION_HPP


#include "object.h"
#include "../../Common/Include/animation.h"
#include <list>

struct CKeyFrame
{
	long m_FrameID;
	CVector m_Position;
	CQuaternion m_Quaternion;
};



class CObjectAnimation
{
public:
	CObjectAnimation(){};
	CObjectAnimation(const CObjectAnimation& in)
		: m_ObjectName(in.m_ObjectName) , m_AnimationData(in.m_AnimationData), m_BoneID(in.m_BoneID)
	{
		m_CurrentFrame = 0;
		m_Prev = m_AnimationData.begin();
		m_Next = m_Prev;
		m_Next++;
		SetMatrix(m_Prev->m_Quaternion, m_Prev->m_Position);
	}

	CObjectAnimation(std::string name ,long boneId,  const std::list<CKeyFrame>& matrices) 
		: m_ObjectName(name), m_AnimationData(matrices),  m_BoneID(boneId)
	{
		m_CurrentFrame = 0;
		m_Prev = m_AnimationData.begin();
		m_Next = m_Prev;
		m_Next++;
		SetMatrix(m_Prev->m_Quaternion, m_Prev->m_Position);
	}

	const CMatrix& Get()
	{
		return m_CurrentMatrix;
	}

	void SetFrame(long frame);


	bool Next();

	long m_BoneID;
	std::string m_ObjectName;
	CMatrix m_InitialMatrix;
	std::list<CKeyFrame> m_AnimationData;
private:
	void SetMatrix(CQuaternion q, CVector v);
	
private:
	CMatrix m_CurrentMatrix;
	
	
	long m_CurrentFrame;
    std::list<CKeyFrame>::iterator m_Prev;
	std::list<CKeyFrame>::iterator m_Next;
};

/**
Object animation controller class. 
For each frame it updates object tree matrices according to  the stored values.
*/
class CObjectAnimationController : public CController< CObject >
{
public:
	CObjectAnimationController();
	CObjectAnimationController(long rate, bool bSkeletal, const std::list<CObjectAnimation>& in_rDesc );
	~CObjectAnimationController();

	virtual void Added( CObject& in_rHost ) 
	{
		UpdateInitialMatrices(in_rHost);
	}
////////////////////////////////////////
// virtual from CController
////////////////////////////////////////
	bool Process( CObject& in_rHost );
	const std::string& GetName() const {
		static std::string Name("ObjectAnimationController");
		return Name;
	}
////////////////////////////////////////
	long GetFrameCount()
	{
		return m_FrameCount;
	}

	long GetCurrentFrame()
	{
		return m_CurrentFrame;
	}

	bool Play( long in_AnimationID = 0, bool in_bLoop = false, float in_StartPosition = 0.0f );
	bool Stop( long in_AnimationID =0);
	bool SeekTo( long in_AnimationID, float in_Where );
	
	void SeekTo(long in_Frame, CObject& in_rHost)
	{
		std::list<CObjectAnimation>::iterator it;
		for (it= m_Animation.begin(); it != m_Animation.end() ; it++ ) 
		{
			CObjectAnimation& anim = *it;
			anim.SetFrame(in_Frame);
		}
		m_bSeekPending = true;
		

	}
	void SeekTo(float in_Time)
	{
		long frame = (long)(in_Time * m_FrameRate);
		SeekTo(frame);

	}
	

private:
	  bool UpdateSkeletal(CObject& in_rHost);
	  bool UpdateObject(CObject& in_rHost , CObjectAnimation& current);
	  void UpdateInitialMatrices(CObject& in_rHost);
	  
	  bool m_bSkeletal;
	  bool m_bPlay;
	  bool m_bLoop;
	  long m_stClock;
	  long m_FrameRate;
	  long m_FrameCount;
	  long m_CurrentFrame;
	  bool m_bSeekPending;

	  std::list<CObjectAnimation> m_Animation;
};



#endif