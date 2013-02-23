#include "../Include/m3_gamedevice.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

#define DOWN 208
#define UP 200
#define LEFT 203
#define RIGHT 205
#define F2 60

#define UNIT_MOVE_MILISECONDS 150
#define DEVICE_IDLE_MILISECONDS 151

CGameDevice::CGameDevice( CGameObjectOwner* in_pOwner ) :
	CGameObject( in_pOwner )
	,m_bSelfPreloadComplete(false)
	,m_bWasErrorDuringPreload(false) 
	,m_Org(0,0,0)
	,m_Dir(0,0,0)
	,m_iSeqLength(0)
	,m_iDir(0)
	,m_bNeedUpdateHighlight(false)
	,m_LastUpdateTime(0)
	,m_bMoving(false)
{
	m_Units.resize(15);
	std::vector< CGameObject* >& rChildren = GetChildren();
	for(unsigned long i=0; i<15; ++i)
	{
		CGameUnit* pUnit = new CGameUnit(i+1,this);
		m_Units[i] = pUnit;
		rChildren.push_back( pUnit );
		m_ChildrenWithPendingPreload.insert( pUnit );
	}
	m_Box = new CGameBox(this);
	rChildren.push_back( m_Box );
	m_ChildrenWithPendingPreload.insert( m_Box );

	m_pMoveCounter = new CMoveCounter(this);
	rChildren.push_back( m_pMoveCounter );
	m_ChildrenWithPendingPreload.insert( rChildren.back() );

}

// stupid method to init matrix
void CGameDevice::SetMatrix()
{
	int k = 0;
	for (int i = 0 ; i < 4; i ++ ) 
	{
		for (int j = 0; j < 4 ; j ++ ) 
		{
			m_Field[i][j] = k+1;

			if (i ==3 && j == 3 ){
				m_Field[i][j] = 0;
				m_xHole = 3;
				m_yHole = 3;
				return;
			}
			if(m_Units[k])
			{
				m_Units[k]->SetPosition(i,j);
			}
			k++;
		}
	}
}

void
CGameDevice::SomeObjectPreloadComplete( bool in_bSuccess, void* in_Handle )
{
	CLog::Print("  CGameDevice::SomeObjectPreloadComplete()\n");
	// Check that in_Handle is in both
	//   GetChildren() and m_ChildrenWithPendingPreload.
	bool FoundInChildren = false;
	std::vector< CGameObject* >& rChildren = GetChildren();
	for( std::vector< CGameObject* >::iterator ItV = rChildren.begin(); ItV!=rChildren.end(); ++ItV )
	{
		if( *ItV == in_Handle )
		{
			FoundInChildren = true;
			break;
		}
	}
	std::set<CGameObject*>::iterator ItS = m_ChildrenWithPendingPreload.find( (CGameObject*)in_Handle );
	bool FoundInPending = ItS!=m_ChildrenWithPendingPreload.end();
	assert(FoundInChildren && FoundInPending);

	m_bWasErrorDuringPreload = m_bWasErrorDuringPreload || (!in_bSuccess);
	m_ChildrenWithPendingPreload.erase( ItS );

	CheckPreloadComplete();
}

void
CGameDevice::vStartPreload()
{
	CLog::Print("START PRELOAD CGameDevice.\n");
	m_bSelfPreloadComplete = true;
	m_bWasErrorDuringPreload = m_bWasErrorDuringPreload || false;
	CheckPreloadComplete();
}

void
CGameDevice::vUpdateBeforeChildren( unsigned long in_Time )
{
	m_LastUpdateTime = in_Time;
	if(m_bMoving)
	{
		if(in_Time>=m_StartMoveTime+DEVICE_IDLE_MILISECONDS)
		{
			m_bMoving = false;
			m_bNeedUpdateHighlight = true;
		}
	}
	if(m_bNeedUpdateHighlight)
	{
		UpdateHighlight();
		m_bNeedUpdateHighlight = false;
	}
}

void
CGameDevice::CheckPreloadComplete()
{
	CLog::Print("  CGameDevice::CheckPreloadComplete()\n");
	if( m_ChildrenWithPendingPreload.empty() && m_bSelfPreloadComplete )
	{
		Shuffle();
		PreloadComplete( !m_bWasErrorDuringPreload );
		
	}
}
void
CGameDevice::CheckComplete()
{
	for (int i = 0 ; i < 4 ; i ++ ) 
	{
		for (int j = 0 ; j < 4 ; j ++ ) 
		{
			if (i == 3 && j == 3 ) 
			{
			}
			else
			if (m_Field[i][j] !=  4*j +i +1 ) 
			{
				return;
			}
		}
	}
	m_bComplete = true;
	m_pMoveCounter->Complete();
}

void CGameDevice::Shuffle()
{
	srand(timeGetTime());
	SetMatrix();
	for (int i = 0 ; i < 1000 ; i ++ ) 
	{
		int r = rand() % 4;
		switch(r)
		{
			case 0: MovePieceForShuffle(UP);
				break;
			case 1: MovePieceForShuffle(DOWN);
				break;
			case 2: MovePieceForShuffle(LEFT);
				break;
			case 3: MovePieceForShuffle(RIGHT);
				break;
		}

	}
	m_bComplete = false;
	m_pMoveCounter->Reset();
}

void CGameDevice::UpdateHighlight()
{
	// find hole position
	int holeX = -1;
	int holeY = -1;
	for (int i = 0 ; i < 4 ; i ++ ) 
	{
		for (int j = 0 ; j < 4 ; j ++ ) 
		{
			
			if (!m_Field[i][j])
			{
				holeX = i;
				holeY = j;
				break;
			}
		}
	}
	

	// find highlight position
	int highlightX = -1, highlightY = -1;
	for (int i = 0 ; i < 4 ; i ++ ) 
	{
		for (int j = 0 ; j < 4 ; j ++ ) 
		{
			
			if (m_Field[i][j])
			{
				bool result = m_Units[m_Field[i][j]-1]->GetHighlight(m_Org,m_Dir);
				if (result ) 
				{
					highlightX = i;
					highlightY = j;
					break;
				}
			}
		}
	}
	

	for (int i = 0 ; i < 15 ; i ++ ) m_Units[i]->SetHighlight(false);
	if (highlightX >= 0 ) 
	{
		if (highlightX  == holeX ) 
		{
			m_iSeqLength = max(highlightY, holeY) -  min(highlightY, holeY);
			if (highlightY > holeY ) 
			{
				m_iDir = UP;
			}
			else
			{
				m_iDir = DOWN;
			}
			// vertical highlight
			for (int i = min(highlightY, holeY) ; i <= max(highlightY, holeY) ; i ++ ) 
			{
				if (m_Field[holeX][i])
				{
					m_Units[m_Field[holeX][i]-1]->SetHighlight(true);
				}
			}
		}
		else
		if (highlightY == holeY ) 
		{
			m_iSeqLength = max(highlightX, holeX) -  min(highlightX, holeX);
			if (highlightX > holeX ) 
			{
				m_iDir = LEFT;
			}
			else
			{
				m_iDir = RIGHT;
			}
			// horizontal highlight
			for (int i = min(highlightX, holeX) ; i <= max(highlightX, holeX) ; i ++ ) 
			{
				if (m_Field[i][holeY])
				{
					m_Units[m_Field[i][holeY]-1]->SetHighlight(true);
				}
			}
		}

	}
	else
	{
		// reset highlight state
		
		m_iSeqLength = 0;
		m_iDir = 0;
	}


}

bool CGameDevice::vAcceptMouse(const CVector& in_Org, const CVector& in_Dir, bool in_bClickDetected)
{
	m_Org = in_Org;
	m_Dir = in_Dir;
	if(m_bPreloadComplete && !m_bComplete && !m_bMoving) 
	{
		bool Moving = false;
		if(in_bClickDetected ) 
		{
			for( int i=0 ; i < m_iSeqLength ; i++ ) 
			{
				bool M = MovePiece(m_iDir);
				if(i>0)
				{
					assert(M==Moving);
				}
				Moving = Moving || M;
			}
		}
		if(Moving)
		{
			m_StartMoveTime = m_LastUpdateTime;
			m_bMoving = true;
		}
	}
	m_bNeedUpdateHighlight = true;
	return false;
}

bool
CGameDevice::MovePiece(int where ) 
{
	switch (where ) 
	{
		case DOWN:
			{
				if (m_yHole == 0 ) 
				{
					// no more fields above return
					return false;
				}
				long unitId = m_Field[m_xHole][m_yHole-1];
				m_Field[m_xHole][m_yHole-1] = 0;
				m_Field[m_xHole][m_yHole] = unitId;
				unitId --;
				if(m_Units[unitId])
				{
					m_Units[unitId]->MoveToPosition(m_xHole,m_yHole,UNIT_MOVE_MILISECONDS);
				}
				m_yHole = m_yHole -1;
				m_pMoveCounter->Increment();
				
			}
			break;
		case UP:
			{
				if (m_yHole == 3 ) 
				{
					// no more fields above return
					return false;
				}
				long unitId = m_Field[m_xHole][m_yHole+1];
				m_Field[m_xHole][m_yHole+1] = 0;
				m_Field[m_xHole][m_yHole] = unitId;
				unitId --;
				if(m_Units[unitId])
				{
					m_Units[unitId]->MoveToPosition(m_xHole,m_yHole,UNIT_MOVE_MILISECONDS);
				}
				m_yHole = m_yHole +1;
				m_pMoveCounter->Increment();
				
			}
			break;

		case RIGHT:
			{
				if (m_xHole == 0 ) 
				{
					// no more fields above return
					return false;
				}
				long unitId = m_Field[m_xHole-1][m_yHole];
				m_Field[m_xHole-1][m_yHole] = 0;
				m_Field[m_xHole][m_yHole] = unitId;
				unitId --;
				if(m_Units[unitId])
				{
					m_Units[unitId]->MoveToPosition(m_xHole,m_yHole,UNIT_MOVE_MILISECONDS);
				}
				m_xHole = m_xHole -1;
				
				m_pMoveCounter->Increment();
			}
			break;
		case LEFT:
			{
				if (m_xHole == 3 ) 
				{
					// no more fields above return
					return false;
				}
				long unitId = m_Field[m_xHole+1][m_yHole];
				m_Field[m_xHole+1][m_yHole] = 0;
				m_Field[m_xHole][m_yHole] = unitId;
				unitId --;
				if(m_Units[unitId])
				{
					m_Units[unitId]->MoveToPosition(m_xHole,m_yHole,UNIT_MOVE_MILISECONDS);
				}
				m_xHole = m_xHole +1;
				
				m_pMoveCounter->Increment();
			}
			break;
	}
	CheckComplete();
	return true;
}

void
CGameDevice::MovePieceForShuffle(int where)
{
	switch (where ) 
	{
			case DOWN:
				{
					if (m_yHole == 0 ) 
					{
						// no more fields above return
						return ;
					}
					long unitId = m_Field[m_xHole][m_yHole-1];
					m_Field[m_xHole][m_yHole-1] = 0;
					m_Field[m_xHole][m_yHole] = unitId;
					unitId --;
					if(m_Units[unitId])
					{
						m_Units[unitId]->SetPosition(m_xHole,m_yHole);
					}
					m_yHole = m_yHole -1;
				}
				break;
			case UP:
				{
					if (m_yHole == 3 ) 
					{
						// no more fields above return
						return ;
					}
					long unitId = m_Field[m_xHole][m_yHole+1];
					m_Field[m_xHole][m_yHole+1] = 0;
					m_Field[m_xHole][m_yHole] = unitId;
					unitId --;
					if(m_Units[unitId])
					{
						m_Units[unitId]->SetPosition(m_xHole,m_yHole);
					}
					m_yHole = m_yHole +1;
				}
				break;

			case RIGHT:
				{
					if (m_xHole == 0 ) 
					{
						// no more fields above return
						return ;
					}
					long unitId = m_Field[m_xHole-1][m_yHole];
					m_Field[m_xHole-1][m_yHole] = 0;
					m_Field[m_xHole][m_yHole] = unitId;
					unitId --;
					if(m_Units[unitId])
					{
						m_Units[unitId]->SetPosition(m_xHole,m_yHole);
					}
					m_xHole = m_xHole -1;
				}
				break;
			case LEFT:
				{
					if (m_xHole == 3 ) 
					{
						// no more fields above return
						return ;
					}
					long unitId = m_Field[m_xHole+1][m_yHole];
					m_Field[m_xHole+1][m_yHole] = 0;
					m_Field[m_xHole][m_yHole] = unitId;
					unitId --;
					if(m_Units[unitId])
					{
						m_Units[unitId]->SetPosition(m_xHole,m_yHole);
					}
					m_xHole = m_xHole +1;
				}
				break;
	}
	CheckComplete();
}


bool CGameDevice::vAcceptKey(unsigned int scanCode ) 
{
	switch (scanCode ) 
	{
			case F2:
				{
					Shuffle();
					UpdateHighlight();
					m_bMoving = false;
					return false;
				}
			case UP:
			case DOWN:
			case LEFT:
			case RIGHT:
				{
					if(!m_bMoving && !m_bComplete)
					{
						if( MovePiece(scanCode) )
						{
							m_bMoving = true;
							m_StartMoveTime = m_LastUpdateTime;
							m_bNeedUpdateHighlight = true;
						}
					}
				}
				break;
	}
	return false;
}






















