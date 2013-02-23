#include "../Include/m3_section.h"
#include "../../MT_Core/Include/log.h"

void
CM3Section::Reaction( long in_SrcSectionID, const CInputConsoleSection_KeyboardUpdate& update )
{
	if (update.m_bPressed) 
	{
		
		m_pWorld->AcceptKey(update.m_bScanCode);
	}
}
static int count = 0;

void
CM3Section::Reaction(long in_SrcSectionID , const CInputConsoleSection_MouseUpdate& update )
{
	// TODO: correctly calculate relative mouse coordinates.
	// -1.0<=MX<=1.0 - positive=right
	// -1.0<=MY<=1.0 - positive=up
	assert( 0 == (TEST_WINDOW_WIDTH%2) );
	assert( 0 == (TEST_WINDOW_HEIGHT%2) );
	float WIDTH_2 = float(TEST_WINDOW_WIDTH/2);
	float HEIGHT_2 = float(TEST_WINDOW_HEIGHT/2);
	float MX = ((float(update.m_dwX))-WIDTH_2) / WIDTH_2;
	float MY = ((float(TEST_WINDOW_HEIGHT-1-update.m_dwY))-HEIGHT_2) / HEIGHT_2;
	bool bClickDetected = false;
	
	bClickDetected  = update.m_bLeftClick;
	/*static bool flag = false;
	if (!flag)
	{
		long soundSectionID = GetSoundSectionID();
		CTCommandSender<CSoundsSection_OpenSoundRequest>::SendCommand(GetSoundSectionID(), 
			CSoundsSection_OpenSoundRequest(SND_WAVE,"/data/2.ogg",GetVFSSectionID(), 33));
		CTCommandSender<CSoundsSection_OpenSoundRequest>::SendCommand(GetSoundSectionID(), 
		CSoundsSection_OpenSoundRequest(SND_OGG,"/data/1.wav",GetVFSSectionID(), 44000));

		flag = true;
	}
	int random =  100*rand()/RAND_MAX;
	CSoundsSection_SetVolumeRequest req(random);
		CTCommandSender<CSoundsSection_SetVolumeRequest>::SendCommand(GetSoundSectionID(), 
			req);
*/
/*	if (bClickDetected)
	{
		CTCommandSender<CSoundsSection_PlaySoundRequest>::SendCommand(GetSoundSectionID(), 
			CSoundsSection_PlaySoundRequest(33));
		count ++ ;
		if (!(count % 10 ))
		{
		CTCommandSender<CSoundsSection_PlaySoundRequest>::SendCommand(GetSoundSectionID(), 
				CSoundsSection_PlaySoundRequest(44000,1));
		}

	}*/
	
	if (m_pWorld) 
	{
		CTranslatedInfo TranslatedMouseInfo = TranslateMouseCoordinates(MX,MY,m_pWorld);
		//CLog::Print("click  org: { %f, %f, %f }  dir: { %f, %f, %f }\n",
		//	TranslatedMouseInfo.m_Org.x,
		//	TranslatedMouseInfo.m_Org.y,
		//	TranslatedMouseInfo.m_Org.z,
		//	TranslatedMouseInfo.m_Dir.x,
		//	TranslatedMouseInfo.m_Dir.y,
		//	TranslatedMouseInfo.m_Dir.z
		//);
		m_pWorld->AcceptMouse(TranslatedMouseInfo.m_Org,TranslatedMouseInfo.m_Dir, bClickDetected);
	}
}














