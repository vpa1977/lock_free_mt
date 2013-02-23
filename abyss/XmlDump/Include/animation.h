#ifndef ANIMATION_HPP 

#define ANIMATION_HPP 

#include "../../Render_DX8/Include/algebra.h"

#include <list>
#include <vector>

/**
	Basic bone class with animation
*/
struct CBoneSerialized
		{
			int m_pParent;
			//std::list<CBoneSerialized*> m_ChildrenPtr;
			CMatrix m_InitialMatrix;
			CMatrix m_InvBoneSkinMatrix;
			CMatrix m_FinalMatrix;
			CVector m_Pos;
			long m_Index;
			std::vector<unsigned char> m_ID_Copy;
			std::string m_ID;
			std::string m_Name;
			std::vector<CMatrix> m_AnimationMatrices;
			std::list<float> m_AnimationData;
			// vector - render call index, matrix offset 
			std::vector< std::pair<unsigned char, unsigned char> > m_MatrixMapping;

			CBoneSerialized()
			{
			}
			
			CBoneSerialized(const CMatrix& initMatrix, int parent, long index,
				std::string id)
				: m_InitialMatrix(initMatrix) , m_pParent(parent) , m_Index(index) , m_ID(id)
				
			{
			} 

			
			


}; 


#endif 