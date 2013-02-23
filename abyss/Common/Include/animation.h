#ifndef ANIMATION_HPP_INCLUDED
#define ANIMATION_HPP_INCLUDED

#include <vector>

template< typename T >
struct CBoneStep
{
	float	m_Time; // normaized, i.e.: 0 <= m_Time <= 1
	T		m_Position;
};

template< typename T >
struct CBoneTransformations
{
	std::vector< CBoneStep< T > > m_Steps;
};

template< typename T >
struct CAnimationData
{
	std::vector< CBoneTransformations< T > > m_Transformations;
};

template< typename T >
struct CAnimationDescription
{
	std::string			m_Name;
	float				m_Duration;
	CAnimationData< T >	m_Data;
};

typedef CAnimationDescription< CMatrix > CAnimationDescription_Matrix;
typedef CAnimationDescription< std::pair< CQuaternion, CVector > > CAnimationDescription_Quaternion;

#endif































