#include "../Include/m4_section.h"
#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/rtassert.h"

#include "../../preloader/include/entity_serializer.h"

#include "../Include/m4_test.h"

#include <assert.h>
#include "CEGUI.h"
#include "CEGUIBase.h"
#include "CEGUIRenderer.h"
#include "CEGUITexture.h"

using namespace CEGUI;

bool g_bFirstFrame = true;

#pragma pack(push,1)
struct CVert {
	float x,y,z;
	unsigned long c;
};
#pragma pack(pop)

#pragma pack(push,1)
struct CVert_1 {
	float x,y,z;
};
#pragma pack(pop)

#pragma pack(push,1)
struct CVertTex {
	float x,y,z;
	unsigned long c;
	float u,v;
};
#pragma pack(pop)

void DumpBoneRecurse(int level, CBone* in_pBone) {
	if(in_pBone) {
		char Buffer[1024];
		memset(Buffer,' ',level*2);
		Buffer[level*2] = 0;
		CLog::Print("%sbone %lu {\n",Buffer,in_pBone->GetID());
		const CBonePosition& rPos = in_pBone->m_Position;
		CMatrix M = rPos.GetPosition();
		CLog::Print("%s  %f %f %f %f\n",Buffer,M.m[0*4+0],M.m[0*4+1],M.m[0*4+2],M.m[0*4+3]);
		CLog::Print("%s  %f %f %f %f\n",Buffer,M.m[1*4+0],M.m[1*4+1],M.m[1*4+2],M.m[1*4+3]);
		CLog::Print("%s  %f %f %f %f\n",Buffer,M.m[2*4+0],M.m[2*4+1],M.m[2*4+2],M.m[2*4+3]);
		CLog::Print("%s  %f %f %f %f\n",Buffer,M.m[3*4+0],M.m[3*4+1],M.m[3*4+2],M.m[3*4+3]);
		for(std::vector< CBone* >::const_iterator It=in_pBone->GetChildren().begin(); It!=in_pBone->GetChildren().end(); ++It) {
			DumpBoneRecurse(level+1,*It);
		}
		CLog::Print("%s}\n",Buffer);
	}
}

void DumpSkeleton( int level, const CObject& in_rO ) {
	unsigned long ET = in_rO.GetEntity()->GetType();
	if(ET==ET_STATIC_MESH) {
		CEntity_StaticMesh* pMesh = (CEntity_StaticMesh*)in_rO.GetEntity();
		CBone* pRootBone = pMesh->GetSkeleton().GetRootBone();
		DumpBoneRecurse(level,pRootBone);
	}
}

void DumpObject( int level, const CObject* pO ) {
	char Buffer[1024];
	memset(Buffer,' ',level*2);
	Buffer[level*2] = 0;
	CLog::Print("%sobject: '%s' {\n", Buffer, pO->GetName().c_str() );
	DumpSkeleton(level+1,*pO);
	//const CMatrix& rM = pO->m_Position;
	//CLog::Print("%s  %f %f %f %f\n",Buffer,rM.m[0*4+0],rM.m[0*4+1],rM.m[0*4+2],rM.m[0*4+3]);
	//CLog::Print("%s  %f %f %f %f\n",Buffer,rM.m[1*4+0],rM.m[1*4+1],rM.m[1*4+2],rM.m[1*4+3]);
	//CLog::Print("%s  %f %f %f %f\n",Buffer,rM.m[2*4+0],rM.m[2*4+1],rM.m[2*4+2],rM.m[2*4+3]);
	//CLog::Print("%s  %f %f %f %f\n",Buffer,rM.m[3*4+0],rM.m[3*4+1],rM.m[3*4+2],rM.m[3*4+3]);
	CLog::Print("%s}\n",Buffer);
	const std::vector<CObject*>& rChildren( pO->GetChildren() );
	for( std::vector<CObject*>::const_iterator It=rChildren.begin(); It!=rChildren.end(); ++It ) {
		DumpObject( level+1, *It );
	}
}

bool bFirstFrame = true;

long position = 0;
float m_fMult=1;
void
CM4Section::OneFrame()
{
	
	if (!m_pO && m_pLoadedObject) 
	{
		if (m_pO) 
			delete m_pO;
		m_pController = NULL;
		m_pO = m_pLoadedObject;
		m_Box = GetBox(*m_pO);
		CLog::Println("On FRAME============");
		CLog::Print("m_Box:\n");
		CLog::Print("  %f %f %f\n",m_Box.m_Pos.x,m_Box.m_Pos.y,m_Box.m_Pos.z);
		CLog::Print("  %f %f %f\n",m_Box.m_Neg.x,m_Box.m_Neg.y,m_Box.m_Neg.z);

	}
	
	assert(GetHWND()!=NULL);
	assert(GetRenderSectionID()!=SECTION_ID_ERROR);
	if(m_NFramesToRender>0)
	{
		
		
		/*if(bFirstFrame && m_pO) {
			// TEST {
			//m_pO = CreateTestObject();
			// } TEST

		   {
				box = GetBox( *m_pO , true );

				DumpObject( 1, m_pO );
				
	//			CElementHandle<CMatrix> Handle = m_pO->m_Position.Get();
				//CMatrix m = Handle;
				//Handle = m;
				
				//m_pController->Play(1, true);
				CLog::Print("box:\n");
				CLog::Print("  %f %f %f\n",box.m_Pos.x,box.m_Pos.y,box.m_Pos.z);
				CLog::Print("  %f %f %f\n",box.m_Neg.x,box.m_Neg.y,box.m_Neg.z);
				bFirstFrame = false;
			}
			
		}*/

	//	



		float DXhalf = 0.5f * (m_Box.m_Pos.x - m_Box.m_Neg.x);
		float DYhalf = 0.5f * (m_Box.m_Pos.y - m_Box.m_Neg.y);
		float DZhalf = 0.5f * (m_Box.m_Pos.z - m_Box.m_Neg.z);

		float ViewDistanceXBased = DXhalf / tanf(CONST_PI_6);
		float ViewDistanceYBased = DYhalf / tanf(0.75f*CONST_PI_6);

		float ViewDistance = (ViewDistanceXBased>ViewDistanceYBased) ? ViewDistanceXBased : ViewDistanceYBased;
		CMatrix obj;
		if (m_pO ) {
			obj = m_pO->m_Position.Get();
		}

		CVector CamOrg( 0, 0, 2*ViewDistance * m_pManager->getScale() );
		//CamOrg *= obj;
		
		CamOrg *= CMatrix().ConstructRotationZ( CONST_PI * m_pManager->m_ZRotate / 180 );	
		CamOrg *= CMatrix().ConstructRotationY( CONST_PI * m_pManager->m_YRotate / 180 );
		CamOrg *= CMatrix().ConstructRotationX( CONST_PI * m_pManager->m_XRotate / 180 );		
		CamOrg *= CMatrix().ConstructTranslation( CVector(obj.m[12], obj.m[13], obj.m[14]));
		
		CLog::Println("Scale is %f " , m_fScale);
		CLog::Println("ViewDistance is %f " , ViewDistance);
		CVector CamTarget(obj.m[12], obj.m[13], obj.m[14]);
		CVector CamRight( 1.0f, 0.0f, 0.0f );
		CVector dir = (CamTarget-CamOrg).Normalize();

		CTCommandSender<CRenderSection_SetCamera>::SendCommand(
			GetRenderSectionID(),
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
			GetRenderSectionID(),
			CRenderSection_SetAmbient( 0x808080 )
		);
		CTCommandSender<CRenderSection_SetDirectionalLight>::SendCommand(
			GetRenderSectionID(),
			CRenderSection_SetDirectionalLight( CVector(0.0f,0.0f,-1.0f).Normalize(), 255.0f, 255.0f, 255.0f )
		);

		if (m_pController && m_pO) 
		{
			if (m_pManager->m_bPlayRequested) 
			{
				if (m_pManager->m_bPlaying) 
				{
					m_pController->Play(0,true);
				}
				else
				{
					m_pController->Stop();
				}
				m_pManager->m_bPlayRequested	= false;
			}
			if (m_pManager->m_bSeekRequested) 
			{
				m_pController->SeekTo(m_pManager->m_CurrentFrame, *m_pO);
				m_pManager->m_bSeekRequested = false;
			}
			if (m_pManager->m_bPlaying) 
			{
				m_pManager->DoScroll( m_pController->GetCurrentFrame() );
			}
		}
		if (m_pO ) 
		{
				m_pO->ProcessControllers();
				assert(m_pO);
				assert(m_pShadowCreator);

				CShadow* pShadow = m_pShadowCreator->CreateShadow(m_pO);
				CTCommandSender< CRenderSection_RenderObject_Request >::SendCommand(
					GetRenderSectionID(),
					CRenderSection_RenderObject_Request(pShadow)
				);
				m_NShadowsSentToRender++;
		}


		if (!m_pO)
		{
			CEGUI_UpdateRequest req;
			m_pProxy->ComposeRequest(req);
			CTCommandSender< CEGUI_UpdateRequest > ::SendCommand( 
				m_ProxySectionID , req
				);

		}
		else
		{
		CTCommandSender< CRenderSection_PresentAndClear >::SendCommand(
			GetRenderSectionID(),
			CRenderSection_PresentAndClear(0x80808080)
		);

		}
		--m_NFramesToRender;
	}
}

void CM4Section::Reaction(long in_SrcSectionID, const CEGUI_UpdateResponse& in ) 
{
		CTCommandSender< CRenderSection_PresentAndClear >::SendCommand(
			GetRenderSectionID(),
			CRenderSection_PresentAndClear(0x80808080)
		);
		
}

void
CM4Section::Reaction( long in_SrcSectionID, const CRenderSection_RenderObject_Response& in_rCmd ) {
	CLog::Println("Reclaiming shadow");
	assert(m_pShadowCreator);
	assert(1==in_rCmd.m_pShadows.size());

	assert(m_NShadowsSentToRender>0);
	m_pShadowCreator->ReclaimShadow( in_rCmd.m_pShadows[0] );
	m_NShadowsSentToRender--;
	if( GameWindowWasClosed() && 0==m_NShadowsSentToRender ) {
		Event_WindowWasClosedByUser(); // hack

	} else {
		if (m_pO != m_pLoadedObject) {
			m_pShadowCreator->DeleteAllShadowsForObject(m_pO);
			if (m_pO) 
				delete m_pO;
			m_pO = m_pLoadedObject;
			m_Box = GetBox(*m_pO);
			CLog::Println("On FRAME============");
			CLog::Print("m_Box:\n");
			CLog::Print("  %f %f %f\n",m_Box.m_Pos.x,m_Box.m_Pos.y,m_Box.m_Pos.z);
			CLog::Print("  %f %f %f\n",m_Box.m_Neg.x,m_Box.m_Neg.y,m_Box.m_Neg.z);

		}

	}
}

float adj = 0.2;

bool m_bDown = false;
void 
CM4Section::Reaction(long in_SrcSectionID, const CInputConsoleSection_MouseUpdate& in_rCmd ) 
{

	if (m_pManager->IsRunning() ) 
	{
		CEGUI::System::getSingleton().injectMousePosition(in_rCmd.m_dwX, in_rCmd.m_dwY);
		if (in_rCmd.m_bLeftPressed && !m_bDown) 
		{
			CEGUI::System::getSingleton().injectMouseButtonDown(LeftButton);
			m_bDown = true;
		}
		if (!in_rCmd.m_bLeftPressed && m_bDown) 
		{
			CEGUI::System::getSingleton().injectMouseButtonUp(LeftButton);
			m_bDown = false;
		}
		// process reactions
		if (m_pManager->m_AnimationLoad.size() ) 
		{
			CPreloaderSection_MatrixLoadRequest& req  = m_pManager->m_AnimationLoad[ m_pManager->m_AnimationLoad.size()-1];
			CTCommandSender<CPreloaderSection_MatrixLoadRequest>::SendCommand(GetPreloaderSectionID(), req);
			m_pManager->m_AnimationLoad.clear();
		}

		if (m_pManager->m_ModelLoad.size() ) 
		{
			CPreloaderSection_ModelLoadRequest& req  = m_pManager->m_ModelLoad[ m_pManager->m_ModelLoad.size()-1];
			CTCommandSender<CPreloaderSection_ModelLoadRequest>::SendCommand(GetPreloaderSectionID(), req);
			m_pManager->m_ModelLoad.clear();
		}

	}
	


}






















