#ifndef RENDER_PROXY_HPP_INCLUDED
#define RENDER_PROXY_HPP_INCLUDED

#include "../../Render_DX8/Include/render_section_interface.h"
#include "proxy_action.h"

class CRenderUser
{
public:
	virtual void CreateVBResult( unsigned long in_VBHandle ) = 0; // in_VBHandle=0 - error, VB was not created.
	virtual void UpdateVBResult( bool in_bSuccess ) = 0;
	virtual void CreateTextureResult( unsigned long in_TexHandle ) = 0; // in_TexHandle=0 - error, Texture was not created.
	virtual void CreatePSResult( unsigned long in_ShaderHandle ) = 0; // in_ShaderHandle=0 - error, Shader was not created.
	virtual void CreateFontResult(unsigned long in_FontHandle) = 0; //  in_FontHandle=0 - error, Font was not created.
};

class CRenderProxy  : protected ProxyAction<unsigned long,CRenderUser*>
{
public:
	virtual void CreateVB( const CRenderSection_CreateVertexBuffer_Request&, CRenderUser* ) = 0;
	virtual void UpdateVB( const CRenderSection_UpdateVertexBuffer_Request&, CRenderUser* ) = 0;
	virtual void CreateTexture( const CRenderSection_CreateTexture_Request&, CRenderUser* ) = 0;
	virtual void CreatePS( const CRenderSection_CreatePixelShader_Request&, CRenderUser* ) = 0;
	virtual void CreateFont(const CFontDescription & , CRenderUser * ) = 0;
	virtual void RenderVB( const CRenderSection_RenderVB& /*, CRenderUser* */ ) = 0;
	virtual void SetAmbient (unsigned long ambient) =0;
	virtual void SetDirectionalLight( const CVector& in_Dir, float in_R, float in_G, float in_B ) =0;
	virtual void SetCamera( const CRenderSection_SetCamera& ) =0;
	virtual void RenderText(	long in_X, long in_Y,
								const std::string& in_rString,
								unsigned long in_FontHandle,
								unsigned long in_Color
								/* ,CRenderUser* in_pUser */	) = 0;
	virtual void Present( unsigned long in_ClearColor ) = 0;

protected:
	void setInstance(CRenderProxy* instance)
	{
		m_pInstance = instance;
	}

public:
	static CRenderProxy& getInstance()
	{
		return (CRenderProxy&) (*m_pInstance);
	}

	static CRenderProxy* m_pInstance;



};


#endif





















