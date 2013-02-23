#include "../Include/m3_section.h"
#include "../../MT_Core/Include/log.h"

void
CM3Section::ReadFile( const char* in_pszFileName, CLoadUser* in_pUser )
{
	// TODO
}

void
CM3Section::DecodeImageFromFile( const char* in_pszFileName, CLoadUser* in_pUser )
{
	// TODO
}

void
CM3Section::DecodeRawImage( IMAGE_TYPE in_Type, const std::vector<unsigned char> in_rData )
{
	// TODO
}


void
CM3Section::CreateVB( const CRenderSection_CreateVertexBuffer_Request&, CRenderUser* )
{
	// TODO
}

void
CM3Section::UpdateVB( const CRenderSection_UpdateVertexBuffer_Request&, CRenderUser* )
{
	// TODO
}

void
CM3Section::CreateTexture( const CRenderSection_CreateTexture_Request&, CRenderUser* )
{
	// TODO
}

void
CM3Section::CreatePS( const CRenderSection_CreatePixelShader_Request&, CRenderUser* )
{
	// TODO
}

void
CM3Section::RenderVB( const CRenderSection_RenderVB&, CRenderUser* )
{
	// TODO
}

void
CM3Section::RenderText(	long in_X, long in_Y,
						const std::string& in_rString,
						unsigned long in_FontHandle,
						unsigned long in_Color,
						CRenderUser* in_pUser	)
{
	// TODO
}
