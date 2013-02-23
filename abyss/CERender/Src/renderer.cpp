#include "../Include/renderer.h"
#include "../Include/texture.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"

using namespace CEGUI;



// add's a quad to the list to be rendered
void	CObjectRendererProxy::addQuad(const Rect& dest_rect, float zt, const Texture* tex, const Rect& texture_rect, const ColourRect& colours, QuadSplitMode quad_split_mode)
{
/*	if (!m_Quads.get() ) 
	{
		m_Quads = boost::shared_ptr<  std::vector<CEGUI_Render_AddQuad> >( new std::vector<CEGUI_Render_AddQuad>() );

	}
	CEGUI_Render_AddQuad addQuad;
	if (!isQueueingEnabled()){
		addQuad.m_bFirst = true;
	}
	addQuad.m_Colours = colours;
	addQuad.m_DestRect = dest_rect;
	addQuad.m_QuadSplitMode = quad_split_mode;
	addQuad.m_TextureID = ((CObjectTexture*)tex)->GetID();
	addQuad.m_TextureRect = texture_rect;
	//CTCommandSender<CEGUI_Render_AddQuad>::SendCommand(GetProxyID(), addQuad);
	m_Quads->push_back(addQuad);*/


	//z = 1-z;
	float z = zt;
	
	QuadVertex v[6];
	Rect position = dest_rect;
	position.offset(Point(-0.5f, -0.5f));
	unsigned long topLeftCol  = colours.d_top_left.getARGB();
	unsigned long topRightCol = colours.d_top_right.getARGB();
	unsigned long bottomLeftCol =colours.d_bottom_left.getARGB();
	unsigned long bottomRightCol = colours.d_bottom_right.getARGB();



	v[0].x = position.d_left;
	v[0].y = position.d_top;
	v[0].z = z;
	v[0].rhw = 1.0f;
	v[0].diffuse = topLeftCol;
	v[0].tu1 = texture_rect.d_left;
	v[0].tv1 = texture_rect.d_top;


        // top-left to bottom-right diagonal
	if (quad_split_mode == TopLeftToBottomRight)
    {
       v[1].x = position.d_right;
       v[1].y = position.d_bottom;
       v[1].z = z;
       v[1].rhw = 1.0f;
       v[1].diffuse = bottomRightCol;
       v[1].tu1 = texture_rect.d_right;
       v[1].tv1 = texture_rect.d_bottom;
    }
    // bottom-left to top-right diagonal
    else
    {
        v[1].x = position.d_right;
        v[1].y = position.d_top;
        v[1].z = z;
        v[1].rhw = 1.0f;
        v[1].diffuse = topRightCol;
        v[1].tu1 = texture_rect.d_right;
        v[1].tv1 = texture_rect.d_top;
    }

			// setup Vertex 3...
	v[2].x = position.d_left;
	v[2].y = position.d_bottom;
	v[2].z = z;
	v[2].rhw = 1.0f;
	v[2].diffuse = bottomLeftCol;
	v[2].tu1 = texture_rect.d_left;
	v[2].tv1 =texture_rect.d_bottom;


	v[3].x = position.d_right;
	v[3].y = position.d_top;
	v[3].z = z;
	v[3].rhw = 1.0f;
	v[3].diffuse = topRightCol;
	v[3].tu1 = texture_rect.d_right;
	v[3].tv1 = texture_rect.d_top;


			// setup Vertex 5...
		v[4].x = position.d_right;
		v[4].y = position.d_bottom;
		v[4].z = z;
		v[4].rhw = 1.0f;
		v[4].diffuse = bottomRightCol;
		v[4].tu1 = texture_rect.d_right;
		v[4].tv1 = texture_rect.d_bottom;
		//++buffmem;

		// setup Vertex 6...

        // top-left to bottom-right diagonal
        if (quad_split_mode == TopLeftToBottomRight)
        {
           v[5].x = position.d_left;
           v[5].y = position.d_top;
           v[5].z = z;
           v[5].rhw = 1.0f;
           v[5].diffuse = topLeftCol;
           v[5].tu1 = texture_rect.d_left;
           v[5].tv1 = texture_rect.d_top;
        }
        // bottom-left to top-right diagonal
        else
        {
            v[5].x = position.d_left;
            v[5].y = position.d_bottom;
            v[5].z = z;
            v[5].rhw = 1.0f;
            v[5].diffuse = bottomLeftCol;
            v[5].tu1 =texture_rect.d_left;
            v[5].tv1 = texture_rect.d_bottom;
        }


		if (isQueueingEnabled() && true ) 
		{
			
			render_queue::iterator it = m_NormalQueue.find( ((CObjectTexture*)tex)->GetID() );
			if (it  == m_NormalQueue.end() ) 
			{
				m_NormalQueue[ ((CObjectTexture*)tex)->GetID() ] = quads();
			}
			it = m_NormalQueue.find(((CObjectTexture*)tex)->GetID() );
			for (int i = 0 ; i < 6 ; i ++ ) 
			{
				it->second.push_back(v[i]);
			}
		}
		else
		{
			render_queue::iterator it = m_OneFrameQueue.find( ((CObjectTexture*)tex)->GetID() );
			if (it  == m_OneFrameQueue.end() ) 
			{
				m_OneFrameQueue[ ((CObjectTexture*)tex)->GetID() ] = quads();
			}
			it = m_OneFrameQueue.find( ((CObjectTexture*)tex)->GetID() );
			for (int i = 0 ; i < 6 ; i ++ ) 
			{
				it->second.push_back(v[i]);
			}


		}

}

// perform final rendering for all queued renderable quads.
// dont render anything ..... 
void	CObjectRendererProxy::doRender(void)
{

}

// clear the queue
void	CObjectRendererProxy::clearRenderList(void)
{
	m_bClearRenderList = true;
	m_NormalQueue.clear();
}





// create an empty texture
Texture*	CObjectRendererProxy::createTexture(void)
{
	CObjectTexture* ptr = new CObjectTexture(this);
	m_Textures.push_back(ptr);
	return ptr ;

}

// create a texture and load it with the specified file.
Texture*	CObjectRendererProxy::createTexture(const String& filename, const String& resourceGroup)
{
	CObjectTexture* ptr = new CObjectTexture(this);
	ptr->loadFromFile(filename, resourceGroup);
	m_Textures.push_back(ptr);
	return ptr ;
}

// create a texture and set it to the specified size
Texture*	CObjectRendererProxy::createTexture(float size)
{
	CObjectTexture* ptr = new CObjectTexture(this);
	m_Textures.push_back(ptr);
	return ptr ;

}

// destroy the given texture
void		CObjectRendererProxy::destroyTexture(Texture* texture)
{
	if( !texture) 
	{
		return;
	}

	std::vector<CObjectTexture*>::iterator it = m_Textures.begin();
	while (it !=  m_Textures.end())
	{
		if ((*it)->GetID() == ((CObjectTexture*)texture)->GetID())
		{
			m_DestroyTexturesList.push_back( ((CObjectTexture*)texture)->GetID());
			m_Textures.erase(it);
		//	delete texture;
			return;
		}
		it++;
	}
	
}

// destroy all textures still active
 void		CObjectRendererProxy::destroyAllTextures(void)
 {
	 m_bDestroyAllTextures = true;
	 m_Textures.clear();
 }
