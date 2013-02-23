#include "../Include/m3_shufflereminder.h"
#include "../Include/render_proxy.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

void
CShuffleReminder::vStartPreload()
{
	PreloadComplete(true);
}

void
CShuffleReminder::vUpdateBeforeChildren( unsigned long in_Time )
{
	CRenderProxy::getInstance().RenderText( 130, 470, "Press F2 to shuffle position.", 2, 0xFFC00000);
}
