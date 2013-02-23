#include "../include/hamsters.h"
#include "../include/game_section.h"


CVector CamOrg;
CVector dir;
CVector CamRight;
CVector CamTarget;
float ViewDistance;

int m_LoadCounter = 0;

void CGameSection::Reaction(long src, const CPreloaderSection_ModelLoadResponse& in )
{
	switch (in.m_Magic) 
	{
		case LOAD_FIELD_MAGIC:
			{
				m_pField = boost::shared_ptr<CField>( new CField(in.m_pObject));
				CPreloaderSection_ModelLoadRequest reqHamster;
				reqHamster.m_Magic =LOAD_HAMSTER_MAGIC;
				reqHamster.m_VFSSection = m_VFSSection;
				reqHamster.m_FileName = "data/goblin";
				CTCommandSender<CPreloaderSection_ModelLoadRequest>::SendCommand(SECTION_ID_BROADCAST, reqHamster);

				break;
			}
		case LOAD_HAMSTER_MAGIC:
			{
				m_pFactory = boost::shared_ptr<CHamsterFactory>( new CHamsterFactory( m_pField->GetBox(), in.m_pObject, 10));

				CBox m_Box = m_pField->GetBox();
				float DXhalf = 0.5f * (m_Box.m_Pos.x - m_Box.m_Neg.x);
				float DYhalf = 0.5f * (m_Box.m_Pos.y - m_Box.m_Neg.y);
				float DZhalf = 0.5f * (m_Box.m_Pos.z - m_Box.m_Neg.z);

				float ViewDistanceXBased = DXhalf / tanf(CONST_PI_6);
				float ViewDistanceYBased = DYhalf / tanf(0.75f*CONST_PI_6);

				ViewDistance = (ViewDistanceXBased>ViewDistanceYBased) ? ViewDistanceXBased : ViewDistanceYBased;
				CMatrix obj;
				

				CamOrg = CVector( 0, 0, ViewDistance  );
				CamTarget = CVector(0, 0, 0);
				CamRight = CVector( 1.0f, 0.0f, 0.0f );
				dir = (CamTarget-CamOrg).Normalize();


				m_pField->AddHamster(m_pFactory->CreateHamster());
				m_pField->AddHamster(m_pFactory->CreateHamster());
				m_pField->AddHamster(m_pFactory->CreateHamster());
				m_pField->Arrange();

				CPreloaderSection_MatrixLoadRequest req;
				req.m_VFSSection = m_VFSSection;
				req.m_Magic = 0;
				req.m_FileName="data/goblin.anim";
				CTCommandSender<CPreloaderSection_MatrixLoadRequest>::SendCommand(SECTION_ID_BROADCAST, req);
				break;
			}
	};
	// check for completion of load here
	//m_pGameField = in.m_pObject;
	


}

void CGameSection:: Reaction(long src , const CRenderSection_RenderObject_Response& in)
{
	assert(m_pShadowCreator);
	assert(1==in.m_pShadows.size());
	m_pShadowCreator->ReclaimShadow( in.m_pShadows[0] );


	CObject * next = m_pField->NextObject();
	if (!next)
	{
		SendUpdate();
	}
	else
	{
		CShadow* pShadow = m_pShadowCreator->CreateShadow(next);
		CTCommandSender< CRenderSection_RenderObject_Request >::SendCommand(
			m_RenderSectionID,
			CRenderSection_RenderObject_Request(pShadow)
		);

	}
}

void CGameSection::Reaction(long src, const CPreloaderSection_MatrixLoadResponse& in )
{
	m_pField->SetHamsterAnimation(in.m_pController);
	StartState();
}



void CGameSection::UpdateState(long inSectionID,const CState_UpdateRequest& update)
{
	// set camera
		CTCommandSender<CRenderSection_SetCamera>::SendCommand(
			m_RenderSectionID,
			CRenderSection_SetCamera(
				CamOrg ,							// org
				dir,	// dir
				CamRight,						// right
				CONST_PI_3, 0.75f*CONST_PI_3,	// view angles
				0.01f*ViewDistance,				// min Z
				150.0f*ViewDistance				// max Z
			)
		);

		CTCommandSender<CRenderSection_SetAmbient>::SendCommand( 
			m_RenderSectionID,
			CRenderSection_SetAmbient(0x80808080)
			);



	
	assert(m_pShadowCreator);
	CShadow* pShadow = m_pShadowCreator->CreateShadow(m_pField->FirstObject());
	CTCommandSender< CRenderSection_RenderObject_Request >::SendCommand(
		m_RenderSectionID,
		CRenderSection_RenderObject_Request(pShadow)
	);
}
	/** 
		Initialization of resources which are needed for lifetime is done here
	*/
void CGameSection::InitState(long inSectionID,const CState_Init& update)
{
	// do nothing for now.
}
	/**
		Allocate temporary resources, free them before invoking EndState
	*/
void CGameSection::Allocate(long inSectionID,const CState_AllocateResourcesRequest& update)
{
	
	CPreloaderSection_ModelLoadRequest reqField;
	reqField.m_Magic =LOAD_FIELD_MAGIC;
	reqField.m_VFSSection = m_VFSSection;
	reqField.m_FileName = "data/play_field";
	CTCommandSender<CPreloaderSection_ModelLoadRequest>::SendCommand(SECTION_ID_BROADCAST, reqField);


}

	/**
		Invoked when we dispose of section
	*/
void CGameSection::Release(long inSectionID,const CState_ReleaseResourcesRequest& update)
{
}