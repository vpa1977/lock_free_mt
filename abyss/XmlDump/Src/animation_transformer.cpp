#include "../Include/animation_transformer.h"
#include "../Include/export_scene_util.h"
#include <stdio.h>

std::vector<CKeyFrame> CAnimationTransformer::GetFrames( std::vector<float> m_seq)
{
	std::vector<CKeyFrame> m_Frames;

	size_t AnimSize = m_seq.size()/16;
	for (int i = 0 ; i < AnimSize; i ++ ) 
	{
		CMatrix m;
		for (int j = 0 ; j < 16 ; j++ )
			m.m[j] = m_seq[i*16+j];

		CMatrix mat1 = m;

		CQuaternion q = mat1.ToQuaternion();
		q.Normalize();
		CVector t( mat1.m[12], mat1.m[13], mat1.m[14] );
		CKeyFrame f;
		f.m_FrameID = i;
		f.m_Position = t;
		f.m_Quaternion = q;
        m_Frames.push_back(f);
	}
	
	long sizeBefore = m_Frames.size();
	bool bOptimized = false;

	//if (true) return m_Frames;
	do {
		std::vector<CKeyFrame> optimized;
		if (m_Frames.size() > 0 ) 
			optimized.push_back(m_Frames[0]);
		bOptimized =false;
		 ;
		for (int i = 2 ;i <= m_Frames.size() ;  i +=2) 
		{
			if (i == m_Frames.size())
			{
				CLog::Print("Adding FID %d \n" ,m_Frames[i-1].m_FrameID);
				optimized.push_back( m_Frames[i-1]);
				continue;
			}
			CKeyFrame& begin = m_Frames[i-2];
			CKeyFrame& inter   = m_Frames[i-1];
			CKeyFrame& end = m_Frames[i];
			float f = ( (float)(inter.m_FrameID - begin.m_FrameID) ) / ( (float)(end.m_FrameID - begin.m_FrameID) );
			CQuaternion qInter = SLerp(begin.m_Quaternion , end.m_Quaternion , f);
			qInter.Normalize();
			
			//optimized.push_back(begin);
			CVector pos_begin = begin.m_Position;
			CVector pos_end = end.m_Position;
			
			pos_begin.x += (pos_end.x-pos_begin.x)/2;
			pos_begin.y += (pos_end.y-pos_begin.y)/2;
			pos_begin.z += (pos_end.z-pos_begin.z)/2;
			pos_begin.x -= inter.m_Position.x;
			pos_begin.y -= inter.m_Position.y;
			pos_begin.z -= inter.m_Position.z;


			if ( inter.m_Quaternion.equals_near(qInter, 0.01f) 
				&& pos_begin.Length() == 0  )
			{
				CLog::Print("Dropping FID %d\n" , inter.m_FrameID);
				bOptimized = true;
			}
			else
			{
				optimized.push_back(inter);
			}
			optimized.push_back(end);
			CLog::Print("Adding FID %d \n" , end.m_FrameID);
		}

		m_Frames = optimized;
		CLog::Print("----------------------------\n");

	} while (bOptimized);
	long sizeAfter = m_Frames.size();
	for (int i = 0 ; i < sizeAfter ; i ++ ) 
	{
		CLog::Print(" Frame %d: {%f,%f,%f}\n" , m_Frames[i].m_FrameID, m_Frames[i].m_Position.x, m_Frames[i].m_Position.y, m_Frames[i].m_Position.z);
	}
	
	return m_Frames;

}