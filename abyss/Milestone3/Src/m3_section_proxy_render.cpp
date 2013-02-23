#include "../Include/m3_section.h"
#include "../../MT_Core/Include/log.h"
#include "../../MT_Core/Include/rtassert.h"


void
CM3Section::SetAmbient(unsigned long ambient)
{
	CTCommandSender<CRenderSection_SetAmbient>::SendCommand(
		GetRenderSectionID(),
		CRenderSection_SetAmbient( ambient )
	);
}

void
CM3Section::SetDirectionalLight( const CVector& in_Dir, float in_R, float in_G, float in_B )
{
	CTCommandSender<CRenderSection_SetDirectionalLight>::SendCommand(
		GetRenderSectionID(),
		CRenderSection_SetDirectionalLight(in_Dir,in_R,in_G,in_B)
	);
}

void
CM3Section::SetCamera( const CRenderSection_SetCamera& command)
{
	CTCommandSender<CRenderSection_SetCamera>::SendCommand(
		GetRenderSectionID(),
		command
	);
}


void
CM3Section::CreateVB( const CRenderSection_CreateVertexBuffer_Request& in_req, CRenderUser* user )
{
	// TODO
	CRenderSection_CreateVertexBuffer_Request req(in_req);
	long renderSectionID = GetRenderSectionID();
	unsigned long userID = (unsigned long) user;
	req.m_Magic = userID;
	CTCommandSender<CRenderSection_CreateVertexBuffer_Request>::SendCommand(
			renderSectionID, req);
	CRenderProxy::RegisterProxiedAction(userID , user);
}

void
CM3Section::UpdateVB( const CRenderSection_UpdateVertexBuffer_Request& req, CRenderUser* user)
{
	// TODO
	long renderSectionID = GetRenderSectionID();
	unsigned long userID = req.m_VBHandle;
	CTCommandSender<CRenderSection_UpdateVertexBuffer_Request>::SendCommand(
			renderSectionID, req);
	CRenderProxy::RegisterProxiedAction(userID , user);
}

void
CM3Section::CreateTexture( const CRenderSection_CreateTexture_Request& in_req, CRenderUser* user)
{
	// TODO
	CRenderSection_CreateTexture_Request req(in_req);
	long renderSectionID = GetRenderSectionID();
	unsigned long userID = (unsigned long) user;
	req.m_Magic = userID;
	CTCommandSender<CRenderSection_CreateTexture_Request>::SendCommand(
			renderSectionID, req);
	CRenderProxy::RegisterProxiedAction(userID , user);

}

void
CM3Section::CreatePS( const CRenderSection_CreatePixelShader_Request& in_req, CRenderUser* user)
{
	CRenderSection_CreatePixelShader_Request req(in_req);
	// TODO
	long renderSectionID = GetRenderSectionID();
	unsigned long userID = (unsigned long) user;
	req.m_Magic = userID;
	CTCommandSender<CRenderSection_CreatePixelShader_Request>::SendCommand(
			renderSectionID, req);
	CRenderProxy::RegisterProxiedAction(userID , user);

}

void 
CM3Section::CreateFont(const CFontDescription& in_rFontDescription, CRenderUser* user ) 
{
	// TODO
	
	
	long renderSectionID = GetRenderSectionID();
	
	CRenderSection_CreateFont_Request req;
	req.m_FontDescription = in_rFontDescription;
	
	unsigned long userID = (unsigned long) user;
	req.m_Magic = userID;
	CTCommandSender<CRenderSection_CreateFont_Request>::SendCommand(
			renderSectionID, req);
	CRenderProxy::RegisterProxiedAction(userID , user);
}

void
CM3Section::RenderVB( const CRenderSection_RenderVB& req /*, CRenderUser* user */)
{
	// TODO
	long renderSectionID = GetRenderSectionID();
	unsigned long userID = req.m_VBHandle;
	CTCommandSender<CRenderSection_RenderVB>::SendCommand(
			renderSectionID, req);
	//CRenderProxy::getInstance().RegisterProxiedAction(userID , user);

}

void
CM3Section::RenderText(	long in_X, long in_Y,
						const std::string& in_rString,
						unsigned long in_FontHandle,
						unsigned long in_Color/*,
						CRenderUser* in_pUser*/	)
{
	// TODO
	CRenderSection_RenderString req(in_X, in_Y, in_rString,in_FontHandle, in_Color);
	long renderSectionID = GetRenderSectionID();
	
	CTCommandSender<CRenderSection_RenderString>::SendCommand(
			renderSectionID, req);

}

void
CM3Section::Present( unsigned long in_ClearColor )
{
	CTCommandSender<CRenderSection_PresentAndClear>::SendCommand(
		GetRenderSectionID()
		,CRenderSection_PresentAndClear( in_ClearColor )
	);
}



void
CM3Section::Reaction(long in_SrcSectionID , const CRenderSection_CreateVertexBuffer_Response&  resp)
{
		long userID = resp.m_Magic;
		CRenderUser* user = CRenderProxy::getInstance().PopProxiedAction(userID);
		if (!user ) 
		{
			RTASSERT(0);
			// no user found - error;
			return;
		}
		user->CreateVBResult(resp.m_VBHandle);
}

void
CM3Section::Reaction(long in_SrcSectionID , const CRenderSection_UpdateVertexBuffer_Response&   resp)
{
		long userID = resp.m_VBHandle;
		CRenderUser* user = CRenderProxy::getInstance().PopProxiedAction(userID);
		if (!user ) 
		{
			RTASSERT(0);
			// no user found - error;
			return;
		}
		user->UpdateVBResult(resp.m_bSuccess);

}

void
CM3Section::Reaction(long in_SrcSectionID , const CRenderSection_CreateTexture_Response&   resp)
{
		long userID = resp.m_Magic;
		CRenderUser* user = CRenderProxy::getInstance().PopProxiedAction(userID);
		if (!user ) 
		{
			RTASSERT(0);
			// no user found - error;
			return;
		}
		user->CreateTextureResult(resp.m_TexHandle);

}

void
CM3Section::Reaction(long in_SrcSectionID , const CRenderSection_CreatePixelShader_Response&   resp)
{
		long userID = resp.m_Magic;
		CRenderUser* user = CRenderProxy::getInstance().PopProxiedAction(userID);
		if (!user ) 
		{
			RTASSERT(0);
			// no user found - error;
			return;
		}
		user->CreatePSResult(resp.m_ShaderHandle);


}

void
CM3Section::Reaction(long in_SrcSectionID , const CRenderSection_CreateFont_Response&   resp)
{
		long userID = resp.m_Magic;
		CRenderUser* user = CRenderProxy::getInstance().PopProxiedAction(userID);
		if (!user ) 
		{
			RTASSERT(0);
			// no user found - error;
			return;
		}
		user->CreateFontResult( resp.m_FontHandle);
}
