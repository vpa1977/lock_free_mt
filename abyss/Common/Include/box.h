#ifndef BOX_HPP_INCLUDED
#define BOX_HPP_INCLUDED

#include "../../Render_DX8/Include/algebra.h"

template< typename T >
const T& Max( const T& t1, const T& t2 ) {
	if( t1 > t2 )
		return t1;
	return t2;
}

template< typename T >
T& Max( T& t1, T& t2 ) {
	if( t1 > t2 )
		return t1;
	return t2;
}

template< typename T >
const T& Min( const T& t1, const T& t2 ) {
	if( t1 < t2 )
		return t1;
	return t2;
}

template< typename T >
T& Min( T& t1, T& t2 ) {
	if( t1 < t2 )
		return t1;
	return t2;
}

struct CBox
{
	CBox() {}
	CBox( const CVector& in_Pos, const CVector& in_Neg ) : m_Pos(in_Pos), m_Neg(in_Neg) {}

	CVector m_Pos;
	CVector m_Neg;

	CBox& operator = ( const CVector& v ) { m_Pos = v; m_Neg = v; }

	CBox& operator += ( const CVector& v ) {
		m_Pos.x = Max( m_Pos.x, v.x );
		m_Pos.y = Max( m_Pos.y, v.y );
		m_Pos.z = Max( m_Pos.z, v.z );
		m_Neg.x = Min( m_Neg.x, v.x );
		m_Neg.y = Min( m_Neg.y, v.y );
		m_Neg.z = Min( m_Neg.z, v.z );
		return *this;
	}
	CBox& operator += ( const CBox& b ) {
		m_Pos.x = Max( m_Pos.x, b.m_Pos.x );
		m_Pos.y = Max( m_Pos.y, b.m_Pos.y );
		m_Pos.z = Max( m_Pos.z, b.m_Pos.z );
		m_Neg.x = Min( m_Neg.x, b.m_Neg.x );
		m_Neg.y = Min( m_Neg.y, b.m_Neg.y );
		m_Neg.z = Min( m_Neg.z, b.m_Neg.z );
		return *this;
	}

	CBox& operator *= ( float f ) {
		m_Pos *= f;
		m_Neg *= f;
		return *this;
	}
	CBox& operator *= ( const CMatrix& m ) {
		// transform all 8 corners of the box
		CVector v0( m_Pos.x, m_Pos.y, m_Pos.z ); v0*=m;
		CVector v1( m_Pos.x, m_Pos.y, m_Neg.z ); v1*=m;
		CVector v2( m_Pos.x, m_Neg.y, m_Pos.z ); v2*=m;
		CVector v3( m_Pos.x, m_Neg.y, m_Neg.z ); v3*=m;
		CVector v4( m_Neg.x, m_Pos.y, m_Pos.z ); v4*=m;
		CVector v5( m_Neg.x, m_Pos.y, m_Neg.z ); v5*=m;
		CVector v6( m_Neg.x, m_Neg.y, m_Pos.z ); v6*=m;
		CVector v7( m_Neg.x, m_Neg.y, m_Neg.z ); v7*=m;
		// construct new box
		m_Pos.x = Max(	Max( Max( v0.x, v1.x ), Max( v2.x, v3.x ) ),
						Max( Max( v4.x, v5.x ), Max( v6.x, v7.x ) )	);
		m_Pos.y = Max(	Max( Max( v0.y, v1.y ), Max( v2.y, v3.y ) ),
						Max( Max( v4.y, v5.y ), Max( v6.y, v7.y ) )	);
		m_Pos.z = Max(	Max( Max( v0.z, v1.z ), Max( v2.z, v3.z ) ),
						Max( Max( v4.z, v5.z ), Max( v6.z, v7.z ) )	);
		m_Neg.x = Min(	Min( Min( v0.x, v1.x ), Min( v2.x, v3.x ) ),
						Min( Min( v4.x, v5.x ), Min( v6.x, v7.x ) )	);
		m_Neg.y = Min(	Min( Min( v0.y, v1.y ), Min( v2.y, v3.y ) ),
						Min( Min( v4.y, v5.y ), Min( v6.y, v7.y ) )	);
		m_Neg.z = Min(	Min( Min( v0.z, v1.z ), Min( v2.z, v3.z ) ),
						Min( Min( v4.z, v5.z ), Min( v6.z, v7.z ) )	);
		return *this;
	}
};

inline CBox operator + ( const CBox& b1, const CBox& b2 ) {
	return CBox(	
					CVector( Max(b1.m_Pos.x,b2.m_Pos.x), Max(b1.m_Pos.y,b2.m_Pos.y), Max(b1.m_Pos.z,b2.m_Pos.z) ),
					CVector( Min(b1.m_Neg.x,b2.m_Neg.x), Min(b1.m_Neg.y,b2.m_Neg.y), Min(b1.m_Neg.z,b2.m_Neg.z) )
				);
}

inline CBox operator * ( const CBox& b, const CMatrix& m ) {
	// transform all 8 corners of the box
	CVector v0( b.m_Pos.x, b.m_Pos.y, b.m_Pos.z ); v0*=m;
	CVector v1( b.m_Pos.x, b.m_Pos.y, b.m_Neg.z ); v1*=m;
	CVector v2( b.m_Pos.x, b.m_Neg.y, b.m_Pos.z ); v2*=m;
	CVector v3( b.m_Pos.x, b.m_Neg.y, b.m_Neg.z ); v3*=m;
	CVector v4( b.m_Neg.x, b.m_Pos.y, b.m_Pos.z ); v4*=m;
	CVector v5( b.m_Neg.x, b.m_Pos.y, b.m_Neg.z ); v5*=m;
	CVector v6( b.m_Neg.x, b.m_Neg.y, b.m_Pos.z ); v6*=m;
	CVector v7( b.m_Neg.x, b.m_Neg.y, b.m_Neg.z ); v7*=m;
	// construct new box
	return CBox(
		CVector(
			Max(
				Max( Max( v0.x, v1.x ), Max( v2.x, v3.x ) ),
				Max( Max( v4.x, v5.x ), Max( v6.x, v7.x ) )
			),
			Max(
				Max( Max( v0.y, v1.y ), Max( v2.y, v3.y ) ),
				Max( Max( v4.y, v5.y ), Max( v6.y, v7.y ) )
			),
			Max(
				Max( Max( v0.z, v1.z ), Max( v2.z, v3.z ) ),
				Max( Max( v4.z, v5.z ), Max( v6.z, v7.z ) )
			)
		),
		CVector(
			Min(
				Min( Min( v0.x, v1.x ), Min( v2.x, v3.x ) ),
				Min( Min( v4.x, v5.x ), Min( v6.x, v7.x ) )
			),
			Min(
				Min( Min( v0.y, v1.y ), Min( v2.y, v3.y ) ),
				Min( Min( v4.y, v5.y ), Min( v6.y, v7.y ) )
			),
			Min(
				Min( Min( v0.z, v1.z ), Min( v2.z, v3.z ) ),
				Min( Min( v4.z, v5.z ), Min( v6.z, v7.z ) )
			)
		)
	);
}

#endif




































