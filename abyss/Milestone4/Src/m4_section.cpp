#include "../Include/m4_section.h"
#include "../../MT_Core/Include/section_registrator.h"
#include "../../MT_Core/Include/log.h"

//#include "../../Render_DX8/Include/es_static_mesh.h"

// TEST {
#include "../../Render_DX8/Include/render_shadow_creator.h"
// } TEST

static CTSectionRegistrator<M4Section_CLSID,CM4Section>
g_DemoSectionRegistrator( SINGLE_INSTANCE_ONLY, STARTUP_SECTION );

extern void Touch_CEGUI();

CM4Section::CM4Section( long in_SectionID ) :
	CGameBaseSection( in_SectionID )
	,m_NThreadsToRun(0)
	,m_pShadowCreator(NULL)
	,m_pE(NULL)
	,m_pO(NULL)
	,m_pController(NULL)
	,m_NFramesToRender(2)
	,m_FirstTimeFrame(0)
	,m_LastTimeFrame(0)
	,m_NFrames(0) 
	,m_NShadowsSentToRender(0)
	,m_fAngleX(0)
	,m_fAngleY(0)
	,m_fScale(0.01f)
	,m_pLoadedObject(NULL)
{
	Touch_CEGUI();
	// system
//	RegisterReaction( GetID((CCommand_SetTimer_Response*)NULL),	CTConform<CM3Section,CCommand_SetTimer_Response>::Conform );
//	RegisterReaction( GetID((CCommand_Timer_Event*)NULL),	CTConform<CM3Section,CCommand_Timer_Event>::Conform );
	// vfs
//	RegisterReaction( GetID((CVFSSection_MountResponse*)NULL),	CTConform<CM3Section,CVFSSection_MountResponse>::Conform );
//	RegisterReaction( GetID((CVFSSection_UMountResponse*)NULL),	CTConform<CM3Section,CVFSSection_UMountResponse>::Conform );
//	RegisterReaction( GetID((CVFSSection_ReadResponse*)NULL),	CTConform<CM3Section,CVFSSection_ReadResponse>::Conform );
//	RegisterReaction( GetID((CVFSSection_CloseResponse*)NULL),	CTConform<CM3Section,CVFSSection_CloseResponse>::Conform );
//	RegisterReaction( GetID((CVFSSection_OpenResponse*)NULL),	CTConform<CM3Section,CVFSSection_OpenResponse>::Conform );
	// vfs errors
//	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_MountResponse>*)NULL),	CTConform<CM3Section,CVFSSection_Error<CVFSSection_MountResponse> >::Conform );
//	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_UMountResponse>*)NULL),	CTConform<CM3Section,CVFSSection_Error<CVFSSection_UMountResponse> >::Conform );
//	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_ReadResponse>*)NULL),	CTConform<CM3Section,CVFSSection_Error<CVFSSection_ReadResponse> >::Conform );
//	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_OpenResponse>*)NULL),	CTConform<CM3Section,CVFSSection_Error<CVFSSection_OpenResponse> >::Conform );
//	RegisterReaction( GetID((CVFSSection_Error<CVFSSection_CloseResponse>*)NULL),	CTConform<CM3Section,CVFSSection_Error<CVFSSection_CloseResponse> >::Conform );
	// preloader
//	RegisterReaction( GetID((CPreloaderSection_DecodeError*)NULL),	CTConform<CM3Section,CPreloaderSection_DecodeError>::Conform );
//	RegisterReaction( GetID((CPreloaderSection_DecodeResponse*)NULL),	CTConform<CM3Section,CPreloaderSection_DecodeResponse>::Conform );
	// window
	// render
	REGISTER_REACTION(CM4Section,CRenderSection_NewFrame);
	REGISTER_REACTION(CM4Section,CRenderSection_RenderObject_Response);
	REGISTER_REACTION(CM4Section, CVFSSection_MountResponse);
	REGISTER_REACTION(CM4Section , CPreloaderSection_ModelLoadResponse );
	REGISTER_REACTION(CM4Section , CPreloaderSection_MatrixLoadResponse );
	REGISTER_REACTION(CM4Section  , CInputConsoleSection_MouseUpdate );
	REGISTER_REACTION(CM4Section , CPreloaderSection_DecodeError);
	REGISTER_REACTION(CM4Section , CEGUI_Render_InitResponse );
	REGISTER_REACTION(CM4Section, CEGUI_UpdateResponse);
	
	
	m_pManager =boost::shared_ptr<CGuiManager>( new CGuiManager());



//	RegisterReaction( GetID((CRenderSection_CreateVertexBuffer_Response*)NULL),	CTConform<CM3Section,CRenderSection_CreateVertexBuffer_Response>::Conform );
//	RegisterReaction( GetID((CRenderSection_UpdateVertexBuffer_Response*)NULL),	CTConform<CM3Section,CRenderSection_UpdateVertexBuffer_Response>::Conform );
//	RegisterReaction( GetID((CRenderSection_CreateTexture_Response*)NULL),	CTConform<CM3Section,CRenderSection_CreateTexture_Response>::Conform );
//	RegisterReaction( GetID((CRenderSection_CreatePixelShader_Response*)NULL),	CTConform<CM3Section,CRenderSection_CreatePixelShader_Response>::Conform );
//	RegisterReaction( GetID((CRenderSection_CreateFont_Response*)NULL),	CTConform<CM3Section,CRenderSection_CreateFont_Response>::Conform );
	// input
//	RegisterReaction( GetID((CInputConsoleSection_KeyboardUpdate*)NULL),	CTConform<CM3Section,CInputConsoleSection_KeyboardUpdate>::Conform );
//	RegisterReaction( GetID((CInputConsoleSection_MouseUpdate*)NULL),	CTConform<CM3Section,CInputConsoleSection_MouseUpdate>::Conform );

	/*
	class Test : public CElementOwner<long>
	{
	public:
		Test() : m_Element(0)
		{
			CLog::Print("Test::Test()\n");
		}
		~Test()
		{
			CLog::Print("Test::~Test()\n");
		}
		long Check() { return m_Element; }
	private:
		long& GetReference() { return m_Element; }
		void ElementAccessBegin( CElementAccessor<long>* in_pAccessor )
		{
			CLog::Print("Test::ElementAccessBegin( %p )\n",in_pAccessor);
		}
		void ElementAccessEnd( CElementAccessor<long>* in_pAccessor )
		{
			CLog::Print("Test::ElementAccessEnd( %p )\n",in_pAccessor);
		}
		long m_Element;
	};
	{
		Test T;
		CLog::Print("1: %ld\n",T.Check());
		{
			CLog::Print("2: %ld\n",T.Check());
			CElementAccessor<long> Handle = T.Get();
			CLog::Print("3: %ld\n",T.Check());
			long& rValue = Handle;
			CLog::Print("4: %ld\n",T.Check());
			rValue = 666;
			CLog::Print("5: %ld\n",T.Check());
		}
		CLog::Print("6: %ld\n",T.Check());
	}
	*/

	//class C { public: virtual ~C() {} };
	//class B { public: virtual ~B() {} };
	//class A : public B, public C {};
	//A* pA = new A;
	//B* pB = pA;
	//C* pC = dynamic_cast<C*>(pB);
	//assert(pC);
	//delete pA;

	/*
	{
		CLog::Print("\n");
		CLog::Print("----CTestOwner----\n");
		class CTestOwner : public CElementOwner
		{
		public:
			CTestOwner() {
				m_Data.Init(this);
			}
			CElementWrapper<long,2> m_Data;
		};
		CTestOwner* pOwner = new CTestOwner;

		CLog::Print("\n");
		CLog::Print("----CTestShadow----\n");
		CElementShadow<long>* pShadow = new CElementShadow<long>(666);

		CLog::Print("\n");
		CLog::Print("----connect owner to target----\n");
		pOwner->m_Data.ConnectLink( pShadow ); // wrapper connects to shadow

		CLog::Print("\n");
		CLog::Print("----connect target to owner----\n");
		pShadow->ConnectLink( &(pOwner->m_Data) ); // shadow connects to wrapper

		CLog::Print("\n");
		CLog::Print("----delete shadow----\n");
		delete pShadow;

		CLog::Print("\n");
		CLog::Print("----delete owner----\n");
		delete pOwner;

		CLog::Print("\n");
		CLog::Print("--------------\n");
	}
	*/

	/*
	{
		CLog::Print("\n");
		CLog::Print("----create----\n");
		CEntity_StaticMesh* pE = new CEntity_StaticMesh( std::vector< const CRenderable_Abstract* >() );
		CObject* pO = new CObject( "Test", true, CMatrix(), std::vector<CObject*>(), pE );
		CEntityShadow_StaticMesh* pS = new CEntityShadow_StaticMesh;

		CLog::Print("\n");
		CLog::Print("----link----\n");
		pS->ConnectLink( pE );
		pS->ConnectLink( pO );
		pO->m_Position.ConnectLink( &(pS->m_Position) );
		pS->m_Position.ConnectLink( &(pO->m_Position) );

		CLog::Print("\n");
		CLog::Print("----change----\n");
		{
			CElementHandle<CMatrix> Handle = pO->m_Position.Get();
			CMatrix& rMatrix( Handle );
			rMatrix.ConstructUnit();
		}

		CLog::Print("\n");
		CLog::Print("----delete shadow----\n");
		delete pS;

		CLog::Print("\n");
		CLog::Print("----delete entity----\n");
		delete pE;

		CLog::Print("\n");
		CLog::Print("----delete object----\n");
		delete pO;

		CLog::Print("\n");
		CLog::Print("--------------\n");
	}
	*/

	/*
	{
		CRenderDX8_ShadowCreator Creator;

		CLog::Print("\n");
		CLog::Print("----create----\n");
		CEntity_StaticMesh* pE = new CEntity_StaticMesh( std::vector< CRenderable_Abstract >() );
		CObject* pChild = new CObject( "Child", true, CMatrix(), std::vector<CObject*>(), pE );
		std::vector<CObject*> Children;
		Children.push_back( pChild );
		CObject* pO = new CObject( "Parent", true, CMatrix(), Children, NULL );

		CLog::Print("\n");
		CLog::Print("----create shadow 1----\n");
		CShadow* pS1 = Creator.CreateShadow( pO );
		assert(pS1);

		CLog::Print("\n");
		CLog::Print("----create shadow 2----\n");
		CShadow* pS2 = Creator.CreateShadow( pO );
		assert(pS2);

		CLog::Print("\n");
		CLog::Print("----reclaim shadow 1----\n");
		Creator.ReclaimShadow( pS1 );

		CLog::Print("\n");
		CLog::Print("----reclaim shadow 2----\n");
		Creator.ReclaimShadow( pS2 );

		CLog::Print("\n");
		CLog::Print("----delete all shadows----\n");
		Creator.DeleteAllShadowsForObject(pO);

		CLog::Print("\n");
		CLog::Print("----delete entity----\n");
		delete pE;

		CLog::Print("\n");
		CLog::Print("----delete objects----\n");
		delete pChild;
		delete pO;

		CLog::Print("\n");
		CLog::Print("--------------\n");
	}
	*/
/*	
	{
		CLog::Print("\n");
		CLog::Print("----create----\n");
	CEntity_StaticMesh* pE = new CEntity_StaticMesh( std::vector< CRenderable_Abstract >() );
		CObject* pO = new CObject( "Parent", true, CMatrix(), std::vector<CObject*>(), pE );

		CLog::Print("\n");
		CLog::Print("----create controller----\n");
		class CTestController : public CController< CObject > {
		public:
			CTestController() { CLog::Print("CTestController::CTestController()\n"); }
			~CTestController() { CLog::Print("CTestController::~CTestController()\n"); }
			void Added( CObject& in_rHost ) { CLog::Print("CTestController::Added()\n"); }
			void Removed( CObject& in_rHost ) { CLog::Print("CTestController::Removed()\n"); }
			bool Process( CObject& in_rHost ) { CLog::Print("CTestController::Process()\n"); return true; }
			const std::string& GetName() const {
				static std::string Name("TestController");
				return Name;
			}
		};
		CTestController* pC = new CTestController();
		bool Added = pO->AddController( pC );
		assert( Added );

		std::set< CController<CObject>* > CC = pO->FindControllers( "TestController" );
		CLog::Print("%lu\n",CC.size());

		CLog::Print("\n");
		CLog::Print("----process----\n");
		pO->ProcessControllers();

		CLog::Print("%lu\n",pO->RemoveControllers("TestController"));

		//bool Removed = pO->RemoveController( pC );
		//assert( Removed );

		CLog::Print("\n");
		CLog::Print("----delete----\n");
		delete pE;
		delete pO;

		CLog::Print("\n");
		CLog::Print("--------------\n");
	}
*/
	m_pShadowCreator = new CRenderDX8_ShadowCreator;
}

CM4Section::~CM4Section()
{
	delete m_pShadowCreator;
}





















