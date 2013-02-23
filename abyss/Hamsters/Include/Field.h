#ifndef FIELD_HPP
#define FIELD_HPP

#include "../include/hamsters.h"

#include <list>
#include <queue>


class CHamster 
{
	enum HAMSTER_STATE {
		IDLE, WALKING
	} m_State;
public:
	CHamster(CObject* renderObject, float scale) : m_pObject(renderObject), m_fScale(scale)
	{
		CMatrix& m = m_pObject->m_Position.Get();
		m = CMatrix().ConstructScaling(CVector(scale,scale,scale));
		m_Pos = m;
		m_AnimationFrame = 0;
	}

	//void WalkTo(float x , float y);
	
	//void Think(); // process state, such as walking, idling etc . update position according 
				  // to the command

	void SetPosition(const CMatrix& position)
	{
		CMatrix result = CMatrix().ConstructScaling(CVector(m_fScale,m_fScale,m_fScale));
		result *= position;
		m_Pos = result;
	}
	long GetAnimationFrame()
	{
		return m_AnimationFrame;
	}
	void SetAnimationFrame(long frame)
	{
		m_AnimationFrame = frame;
	}
	const CMatrix& GetPosition() { return m_Pos ; }

	CObject* GetRenderObject() { return m_pObject; }
private:
	float m_fScale;
	CObject* m_pObject;
	CMatrix m_Pos;
	long m_AnimationFrame;
};

/**
	constructs hamsters for given field box
*/
class CHamsterFactory
{
public:
	CHamsterFactory(const CBox& m_FieldBox, CObject* m_pHamsterRenderObject, int iHamstersPerField);
	CHamster CreateHamster();
private:
	CObject* m_pHamster;
	float m_fScale;

};


class CField 
{
public:
	CField(CObject* pObject);

	const CBox& GetBox() const;

	CObject* GetRenderObject();
	void AddHamster(const CHamster& h)
	{
		m_Hamsters.push_back(h);
	}

	void Arrange()
	{
		float lol = (GetBox().m_Pos.x - GetBox().m_Neg.x)/m_Hamsters.size();
		std::list<CHamster>::iterator it = m_Hamsters.begin();
		int i = -1;
		while (it != m_Hamsters.end() ) 
		{
			CMatrix m;
			m *= CMatrix().ConstructTranslation(CVector(lol*i, lol*i, 10));
			it->SetPosition(m);
			it++;
			i++;
		}
	}

	CObject* FirstObject()
	{
		m_RenderIterator = m_Hamsters.begin();
		return m_pRenderObject;
	}

	CObject* NextObject();
	void SetHamsterAnimation(CObjectAnimationController* pAnim)
	{
		m_pHamsterAnimation = pAnim;
		m_pHamsterAnimation->Play(0,true);
	}

	
private:
	CObject* m_pRenderObject;
	CObject* m_pHamsterObject;
	CObjectAnimationController* m_pHamsterAnimation;
	CBox m_Box;
	std::list<CHamster> m_Hamsters;
	std::list<CHamster>::iterator m_RenderIterator;
	
};

#endif