#include "../include/field.h"

CField::CField(CObject* pObject) : m_pRenderObject(pObject)
{
	m_pRenderObject->m_Position.Get() = CMatrix(); // I matrix
	m_Box = ::GetBox(*m_pRenderObject, true);

}

const CBox& CField::GetBox() const
{
	return m_Box;
}

CObject* CField::GetRenderObject()
{
	return m_pRenderObject;
}

CObject* CField::NextObject()
{
	if (m_RenderIterator == m_Hamsters.end() ) return NULL;

//	m_RenderIterator->Think();
    CObject* pObj = m_RenderIterator->GetRenderObject();
	pObj->m_Position.Get() = m_RenderIterator->GetPosition();
	long frameUsed = m_RenderIterator->GetAnimationFrame()+1;
	m_pHamsterAnimation->SeekTo(frameUsed, *pObj);
	m_pHamsterAnimation->Process(*pObj);
	//frameUsed = m_pHamsterAnimation->GetCurrentFrame();
	
	frameUsed = min(m_pHamsterAnimation->GetFrameCount(), frameUsed);
	m_RenderIterator->SetAnimationFrame(frameUsed);
	
	m_RenderIterator ++;
	return pObj;
};




CHamsterFactory::CHamsterFactory(const CBox& m_FieldBox, CObject* pHamsterRenderObject, int max_hamsters)
:	m_pHamster(pHamsterRenderObject)
{

	CBox hamsterBox = GetBox(*m_pHamster, true);

	float DXhalf = (hamsterBox.m_Pos.x - hamsterBox.m_Neg.x);
	float DYhalf = (hamsterBox.m_Pos.y - hamsterBox.m_Neg.y);
	float DZhalf = (hamsterBox.m_Pos.z - hamsterBox.m_Neg.z);
	float maxValue = 0;

	if (DXhalf > DYhalf )  maxValue = DXhalf;
					else
						maxValue = DYhalf;

	if (DZhalf > maxValue ) maxValue = DZhalf;
	


	float requested = (m_FieldBox.m_Pos.x - m_FieldBox.m_Neg.x)/max_hamsters;
	m_fScale = requested / maxValue;


}
CHamster CHamsterFactory::CreateHamster()
{
	return CHamster(m_pHamster, m_fScale);
}