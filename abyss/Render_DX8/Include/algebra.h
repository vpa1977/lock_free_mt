#ifndef ALGEBRA_HPP_INCLUDED
#define ALGEBRA_HPP_INCLUDED

#include "algebra_constants.h"
#include <math.h>
#include <memory.h>
#include <assert.h>

// =====================================================================================================

class CMatrix;
class CQuaternion;

// =====================================================================================================

#pragma pack(1)
class CVector
{
public:
	float x,y,z;

	//c-tors
	CVector();
	CVector(float X, float Y, float Z);
	CVector(const CVector& V);

	// normas
	float Square() const { return x*x+y*y+z*z; }  
	float Length() const { return sqrt(x*x+y*y+z*z); }
	CVector& Normalize() { float OOL=1.0f/Length(); x*=OOL; y*=OOL; z*=OOL; return *this; }
	friend CVector Normalize( const CVector& V )
	{
		float OOL=1.0f/V.Length();
		return CVector( OOL*V.x, OOL*V.y, OOL*V.z );
	}

	// multiplication
	friend CVector operator * (const CVector& V, float f);
	friend CVector operator * (float f, const CVector& V);

	// vector multiplication
	friend CVector operator * (const CVector& V1, const CVector& V2);

	// vec matr multiplication
	friend CVector operator * (const CVector& V, const CMatrix& M); // vector is a ROW here
	CVector& operator *= (const CMatrix& M);

	// rotate vector by quaternion
	friend CVector operator * (const CVector& V, const CQuaternion& Q);
	CVector& operator *= (const CQuaternion& Q);

	// unary minus
	CVector operator - () const { return CVector(-x,-y,-z); }

	// subtraction
	friend CVector operator - (const CVector& V1, const CVector& V2);

	// addition
	friend CVector operator + (const CVector& V1, const CVector& V2);

	// scalar multiplication
	friend float operator ^ (const CVector& V1, const CVector& V2);

	// assignment addition, subtraction, multiplication
	CVector& operator += (const CVector& V) { x+=V.x; y+=V.y; z+=V.z; return *this; }
	CVector& operator -= (const CVector& V) { x-=V.x; y-=V.y; z-=V.z; return *this; }
	CVector& operator *= (const CVector& V) { float X = y*V.z-z*V.y; float Y = z*V.x-x*V.z; float Z = x*V.y-y*V.x; x = X; y = Y; z = Z; return *this; }
	CVector& operator *= (float f) { x*=f; y*=f; z*=f; return *this; }
	CVector& operator /= (float f) { float oof=1.0f/f; x*=oof; y*=oof; z*=oof; return *this; }

	// stuff
	friend CVector Lerp( const CVector& Q1, const CVector& Q2, float t );
};
#pragma pack()

// =====================================================================================================


#pragma pack(8)
class CMatrix
{
public:
	float m[16]; // First element is a upper left, last one is a lower right.

	// c-tors
	CMatrix()
	{
		m[0] =  1.0f; m[1] =  0.0f; m[2] =  0.0f; m[3] =  0.0f;
		m[4] =  0.0f; m[5] =  1.0f; m[6] =  0.0f; m[7] =  0.0f;
		m[8] =  0.0f; m[9] =  0.0f; m[10] = 1.0f; m[11] = 0.0f;
		m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
	}
	CMatrix(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	)
	{
		m[0] =  m00; m[1] =  m01; m[2] =  m02; m[3] =  m03;
		m[4] =  m10; m[5] =  m11; m[6] =  m12; m[7] =  m13;
		m[8] =  m20; m[9] =  m21; m[10] = m22; m[11] = m23;
		m[12] = m30; m[13] = m31; m[14] = m32; m[15] = m33;
	}
	CMatrix( const CVector& v0, const CVector& v1, const CVector& v2, const CVector& v3 )
	{
		m[0] = v0.x; m[1] = v0.y; m[2] = v0.z; m[3] = 0.0f;
		m[4] = v1.x; m[5] = v1.y; m[6] = v1.z; m[7] = 0.0f;
		m[8] = v2.x; m[9] = v2.y; m[10] = v2.z; m[11] = 0.0f;
		m[12] = v3.x; m[13] = v3.y; m[14] = v3.z; m[15] = 1.0f;
	}
	CMatrix( const CVector& Org, const CVector& Dir, const CVector& Up);
	explicit CMatrix( const float* in_pRawData )
	{
		memcpy(m,in_pRawData,sizeof(m));
	}

	// builders
	CMatrix& ConstructUnit();
	CMatrix& ConstructRotationX( float in_Angle );
	CMatrix& ConstructRotationY( float in_Angle );
	CMatrix& ConstructRotationZ( float in_Angle );
	CMatrix& ConstructRotation( const CVector& in_Axis, float in_Angle );
	CMatrix& ConstructTranslation( const CVector& in_Shift );
	CMatrix& ConstructScaling( const CVector& in_Scale );


	float* operator[](int index) {
		index  = index * 4;
		return &m[index];

	};
	// specific
	CMatrix& Transpose();
	friend CMatrix Transpose( const CMatrix& M );
	CMatrix& Invert();
	friend CMatrix Invert( const CMatrix& M );

	// vec matr multiplication
	friend CVector operator * (const CVector& V, const CMatrix& M); // vector is a ROW here

	// addition, subtraction, multiplication
	friend CMatrix operator * (const CMatrix& M1, const CMatrix& M2);
	friend CMatrix operator + (const CMatrix& M1, const CMatrix& M2);
	friend CMatrix operator - (const CMatrix& M1, const CMatrix& M2);

	// assignment addition, subtraction, multiplication
	CMatrix& operator *= (const CMatrix& M);
	CMatrix& operator += (const CMatrix& M);
	CMatrix& operator -= (const CMatrix& M);

	// conversion
	CQuaternion ToQuaternion() const;
};
#pragma pack()

// =====================================================================================================

class CQuaternion {
public:
	float x,y,z,w;

	CQuaternion( float x_=0.0f, float y_=0.0f, float z_=0.0f, float w_=1.0f ) : x(x_), y(y_), z(z_), w(w_) {}
	CQuaternion( const CQuaternion& in_Q ) : x(in_Q.x), y(in_Q.y), z(in_Q.z), w(in_Q.w) {}
	CQuaternion( const CVector in_V, float in_Angle ) {
		CVector V(in_V);
		V.Normalize();
		float HalfAngle = in_Angle*0.5f;
		float S = sinf( HalfAngle );
		x = V.x*S;
		y = V.y*S;
		z = V.z*S;
		w = cosf( HalfAngle );
	}

	// Norma
	float Norm() const { return x*x + y*y + z*z + w*w; }
	float Length() const { return sqrt( x*x + y*y + z*z + w*w ); }
	CQuaternion& Normalize() { float L=Length(); x/=L; y/=L; z/=L; w/=L; return *this;  }

	// conjugate
	CQuaternion operator ! () const { return CQuaternion(-x,-y,-z,w); }

	// inverse
	CQuaternion operator - () const { 
		float N = Length();
		return CQuaternion( -x/N, -y/N, -z/N, w/N );
	}


	// equals
	bool equals_near(const CQuaternion& Q2, float range)
	{
		float thisPhi = acosf(w)*2;
		float q2Phi = acosf(Q2.w)*2;
		if (fabs( thisPhi - q2Phi) < range)
		{
			if (sinf(thisPhi/2) == 0 ) return false;
			if (sinf(q2Phi/2) == 0 ) return false;

			CVector v1(x/sinf(thisPhi/2), y/sinf(thisPhi/2) , z/ sinf(thisPhi/2));
			CVector v2(Q2.x/sinf(q2Phi/2), Q2.y/sinf(q2Phi/2),  Q2.z/ sinf(q2Phi/2));
			double vecAngle = acos( (v1.x* v2.x + v1.y*v2.y + v1.z* v2.z)/( sqrt( v1.x*v1.x + v1.y * v1.y + v1.z*v1.z) * sqrt( v2.x*v2.x + v2.y * v2.y + v2.z*v2.z)));
			if (fabs(vecAngle) < range ) 
			{
				return true;
			}
		}
		return false;

	}

	// scalar multiplication
	friend float operator ^ ( const CQuaternion& Q1, const CQuaternion& Q2 ) { return Q1.x*Q2.x + Q1.y*Q2.y + Q1.z*Q2.z + Q1.w*Q2.w; }

	// addition, subtraction
	friend CQuaternion operator + ( const CQuaternion& Q1, const CQuaternion& Q2 ) { return CQuaternion(Q1.x+Q2.x, Q1.y+Q2.y, Q1.z+Q2.z, Q1.w+Q2.w); }
	friend CQuaternion operator - ( const CQuaternion& Q1, const CQuaternion& Q2 ) { return CQuaternion(Q1.x-Q2.x, Q1.y-Q2.y, Q1.z-Q2.z, Q1.w-Q2.w); }
	CQuaternion& operator += (const CQuaternion& Q) { x+=Q.x; y+=Q.y; z+=Q.z; w+=Q.w; return *this; }
	CQuaternion& operator -= (const CQuaternion& Q) { x-=Q.x; y-=Q.y; z-=Q.z; w-=Q.w; return *this; }

	// multiplication
	friend CQuaternion operator * ( const CQuaternion& Q1, const CQuaternion& Q2 );
	CQuaternion& operator *= (const CQuaternion& Q);

	friend CVector operator * (const CVector& V, const CQuaternion& Q);

	// conversion
	CMatrix ToMatrix() const;
	inline void ToAxisAngle( CVector& out_V, float& out_Angle ) const;
};

inline bool equals_near(const CQuaternion& Q1,const CQuaternion& Q2, float range)
{
		float thisPhi = acos(Q1.w)*2;
		float q2Phi = acos(Q2.w)*2;
		if (fabs( thisPhi - q2Phi) < range)
		{
			if (sin(thisPhi/2) == 0 ) return false;
			if (sin(q2Phi/2) == 0 ) return false;

			CVector v1(Q1.x/sin(thisPhi/2), Q1.y/sin(thisPhi/2) , Q1.z/ sin(thisPhi/2));
			CVector v2(Q2.x/sin(q2Phi/2), Q2.y/sin(q2Phi/2),  Q2.z/ sin(q2Phi/2));
			double vecAngle = acos( (v1.x* v2.x + v1.y*v2.y + v1.z* v2.z)/( sqrt( v1.x*v1.x + v1.y * v1.y + v1.z*v1.z) * sqrt( v2.x*v2.x + v2.y * v2.y + v2.z*v2.z)));
			if (fabs(vecAngle) < range ) 
			{
				return true;
			}
		}
		return false;
}

inline CVector RotateVectorByQuaternion( const CVector& V, const CQuaternion& Q );
inline CQuaternion CreateUnitQuaternionFromRotationMatrix( const CMatrix& M );
inline CQuaternion CreateNonUnitQuaternionFromRotationMatrix( const CMatrix& M );
inline CQuaternion Lerp( const CQuaternion& Q1, const CQuaternion& Q2, float t );
inline CQuaternion SLerp( const CQuaternion& Q1, const CQuaternion& Q2, float t );

// =====================================================================================================

inline CVector operator * (const CVector& V, float f)
{
	return CVector(V.x*f,V.y*f,V.z*f);
}

inline CVector operator * (float f, const CVector& V)
{
	return CVector(V.x*f,V.y*f,V.z*f);
}

inline CVector operator * (const CVector& V1, const CVector& V2)
{
	return CVector(  V1.y*V2.z-V1.z*V2.y, V1.z*V2.x-V1.x*V2.z, V1.x*V2.y-V1.y*V2.x );
}

inline CVector operator * (const CVector& V, const CMatrix& M) // vector is a ROW here
{
	float OOW = 1.0f/(V.x*M.m[3]+V.y*M.m[7]+V.z*M.m[11]+M.m[15]);
	return CVector(
		OOW*(V.x*M.m[0]+V.y*M.m[4]+V.z*M.m[8]+M.m[12]),
		OOW*(V.x*M.m[1]+V.y*M.m[5]+V.z*M.m[9]+M.m[13]),
		OOW*(V.x*M.m[2]+V.y*M.m[6]+V.z*M.m[10]+M.m[14])
	);
}

inline CVector& CVector::operator *= (const CMatrix& M)
{
	float OOW = 1.0f/(x*M.m[3]+y*M.m[7]+z*M.m[11]+M.m[15]);
	float xx = OOW*(x*M.m[0]+y*M.m[4]+z*M.m[8]+M.m[12]);
	float yy = OOW*(x*M.m[1]+y*M.m[5]+z*M.m[9]+M.m[13]);
	float zz = OOW*(x*M.m[2]+y*M.m[6]+z*M.m[10]+M.m[14]);
	x = xx;
	y = yy;
	z = zz;
	return *this;
}


inline CVector operator - (const CVector& V1, const CVector& V2)
{
	return CVector( V1.x-V2.x, V1.y-V2.y, V1.z-V2.z );
}

inline CVector operator + (const CVector& V1, const CVector& V2)
{
	return CVector( V1.x+V2.x, V1.y+V2.y, V1.z+V2.z );
}

inline float operator ^ (const CVector& V1, const CVector& V2)
{
	return V1.x*V2.x + V1.y*V2.y + V1.z*V2.z;
}

inline CVector Lerp( const CVector& V1, const CVector& V2, float t ) {
	assert(0.0f<=t && t<=1.0f);
	return V1 + t*(V2-V1);
}

inline CMatrix operator + (const CMatrix& M1, const CMatrix& M2)
{
	return CMatrix(
		M1.m[0]+M2.m[0], M1.m[1]+M2.m[1], M1.m[2]+M2.m[2], M1.m[3]+M2.m[3],
		M1.m[4]+M2.m[4], M1.m[5]+M2.m[5], M1.m[6]+M2.m[6], M1.m[7]+M2.m[7],
		M1.m[8]+M2.m[8], M1.m[9]+M2.m[9], M1.m[10]+M2.m[10], M1.m[11]+M2.m[11],
		M1.m[12]+M2.m[12], M1.m[13]+M2.m[13], M1.m[14]+M2.m[14], M1.m[15]+M2.m[15]
	);
}

inline CMatrix operator - (const CMatrix& M1, const CMatrix& M2)
{
	return CMatrix(
		M1.m[0]-M2.m[0], M1.m[1]-M2.m[1], M1.m[2]-M2.m[2], M1.m[3]-M2.m[3],
		M1.m[4]-M2.m[4], M1.m[5]-M2.m[5], M1.m[6]-M2.m[6], M1.m[7]-M2.m[7],
		M1.m[8]-M2.m[8], M1.m[9]-M2.m[9], M1.m[10]-M2.m[10], M1.m[11]-M2.m[11],
		M1.m[12]-M2.m[12], M1.m[13]-M2.m[13], M1.m[14]-M2.m[14], M1.m[15]-M2.m[15]
	);
}

inline CMatrix& CMatrix::operator += (const CMatrix& M)
{
	m[0]+=M.m[0];	m[1]+=M.m[1];	m[2]+=M.m[2];	m[3]+=M.m[3];
	m[4]+=M.m[4];	m[5]+=M.m[5];	m[6]+=M.m[6];	m[7]+=M.m[7];
	m[8]+=M.m[8];	m[9]+=M.m[9];	m[10]+=M.m[10];	m[11]+=M.m[11];
	m[12]+=M.m[12];	m[13]+=M.m[13];	m[14]+=M.m[14];	m[15]+=M.m[15];
	return *this;
}

inline CMatrix& CMatrix::operator -= (const CMatrix& M)
{
	m[0]-=M.m[0];	m[1]-=M.m[1];	m[2]-=M.m[2];	m[3]-=M.m[3];
	m[4]-=M.m[4];	m[5]-=M.m[5];	m[6]-=M.m[6];	m[7]-=M.m[7];
	m[8]-=M.m[8];	m[9]-=M.m[9];	m[10]-=M.m[10];	m[11]-=M.m[11];
	m[12]-=M.m[12];	m[13]-=M.m[13];	m[14]-=M.m[14];	m[15]-=M.m[15];
	return *this;
}

inline CQuaternion CMatrix::ToQuaternion() const
{
	//
	// TODO: check that matrix contains rotation ONLY
	//
	float tr = m[0*4+0] + m[1*4+1] + m[2*4+2]; // trace of martix
	if (tr > 0.0f) {
		// if trace positive than "w" is biggest component
		return CQuaternion( m[1*4+2]-m[2*4+1], m[2*4+0]-m[0*4+2], m[0*4+1]-m[1*4+0], tr + 1.0f );
	} else {
		// Some of vector components is bigger
		if( (m[0*4+0] > m[1*4+1] ) && ( m[0*4+0] > m[2*4+2]) ) {
			return CQuaternion( 1.0f + m[0*4+0] - m[1*4+1] - m[2*4+2], m[1*4+0] + m[0*4+1], m[2*4+0] + m[0*4+2], m[1*4+2] - m[2*4+1] );
		} else {
			if ( m[1*4+1] > m[2*4+2] ) {
				return CQuaternion( m[1*4+0] + m[0*4+1], 1.0f + m[1*4+1] - m[0*4+0] - m[2*4+2], m[2*4+1] + m[1*4+2], m[2*4+0] - m[0*4+2] );
			} else {
				return CQuaternion( m[2*4+0] + m[0*4+2], m[2*4+1] + m[1*4+2], 1.0f + m[2*4+2] - m[0*4+0] - m[1*4+1], m[0*4+1] - m[1*4+0] );
			}
		}
	}
}

inline CQuaternion operator * ( const CQuaternion& Q1, const CQuaternion& Q2 )
{
	CVector V1( Q1.x, Q1.y, Q1.z );
	CVector V2( Q2.x, Q2.y, Q2.z );
	CVector V( V1*V2 + Q1.w*V2 + Q2.w*V1 );
	return CQuaternion( V.x, V.y, V.z, Q1.w*Q2.w - (V1^V2) );
}

inline CQuaternion& CQuaternion::operator *= (const CQuaternion& Q)
{
	CVector V1( x, y, z );
	CVector V2( Q.x, Q.y, Q.z );
	CVector V( V1*V2 + w*V2 + Q.w*V1 );
	x = V.x;
	y = V.y;
	z = V.z;
	w = w*Q.w - (V1^V2);
	return *this;
}

inline CMatrix CQuaternion::ToMatrix() const
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
	x2 = 2.0f*x;
	y2 = 2.0f*y;
	z2 = 2.0f*z;
	xx = x * x2;   xy = x * y2;   xz = x * z2;
	yy = y * y2;   yz = y * z2;   zz = z * z2;
	wx = w * x2;   wy = w * y2;   wz = w * z2;

	return CMatrix(
		1.0f-(yy+zz),	xy+wz,			xz-wy,			0.0f,
		xy-wz,			1.0f-(xx+zz),	yz+wx,			0.0f,
		xz+wy,			yz-wx,			1.0f-(xx+yy),	0.0f,
		0.0f,			0.0f,			0.0f,			1.0f
	);
}

inline void CQuaternion::ToAxisAngle( CVector& out_V, float& out_Angle ) const
{
	float vl = (float)sqrt( x*x + y*y + z*z );
	if( vl > 0.0001f ) {
		float ivl = 1.0f/vl;
		out_V.x = x*ivl;
		out_V.y = y*ivl;
		out_V.z = z*ivl;
		if( w < 0 )
			out_Angle = 2.0f*(float)atan2(-vl, -w); //-PI,0 
		else
			out_Angle = 2.0f*(float)atan2( vl,  w); //0,PI 
	} else {
		out_V.x = 0.0f;
		out_V.y = 0.0f;
		out_V.z = 0.0f;
		out_Angle = 0.0f;
	}
}

inline CVector RotateVectorByQuaternion( const CVector& V, const CQuaternion& Q ) {
	CQuaternion Result( Q * CQuaternion(V.x,V.y,V.z,0.0f) * (-Q) );
	return CVector( Result.x, Result.y, Result.z );
}

inline CVector operator * (const CVector& V, const CQuaternion& Q) {
	CQuaternion Result( Q * CQuaternion(V.x,V.y,V.z,0.0f) * (-Q) );
	return CVector( Result.x, Result.y, Result.z );
}

inline CVector& CVector::operator *= (const CQuaternion& Q) {
	CQuaternion Result( Q * CQuaternion(x,y,z,0.0f) * (-Q) );
	x = Result.x;
	y = Result.y;
	z = Result.z;
	return *this;
}

inline CQuaternion CreateUnitQuaternionFromRotationMatrix( const CMatrix& M )
{
	float tr = M.m[0*4+0] + M.m[1*4+1] + M.m[2*4+2]; // trace of martix
	if( tr > 0.0f ) {     // if trace positive than "w" is biggest component
		return CQuaternion( M.m[1*4+2] - M.m[2*4+1], M.m[2*4+0] - M.m[0*4+2], M.m[0*4+1] - M.m[1*4+0], tr+1.0f );
		//scale( 0.5f/(float)sqrt( w ) );     // "w" contain the "norm * 4"
	} else {                 // Some of vector components is bigger
		if( (M.m[0*4+0] > M.m[1*4+1] ) && ( M.m[0*4+0] > M.m[2*4+2]) ) {
			return CQuaternion( 1.0f + M.m[0*4+0] - M.m[1*4+1] - M.m[2*4+2], M.m[1*4+0] + M.m[0*4+1], M.m[2*4+0] + M.m[0*4+2], M.m[1*4+2] - M.m[2*4+1] );
			//scale( 0.5f/(float)sqrt( x ) );
		} else {
			if ( M.m[1*4+1] > M.m[2*4+2] ){
				return CQuaternion( M.m[1*4+0] + M.m[0*4+1], 1.0f + M.m[1*4+1] - M.m[0*4+0] - M.m[2*4+2], M.m[2*4+1] + M.m[1*4+2], M.m[2*4+0] - M.m[0*4+2] ); 
				//scale( 0.5f/(float)sqrt( y ) );
			} else {
				return CQuaternion( M.m[2*4+0] + M.m[0*4+2], M.m[2*4+1] + M.m[1*4+2], 1.0f + M.m[2*4+2] - M.m[0*4+0] - M.m[1*4+1], M.m[0*4+1] - M.m[1*4+0] );
				//scale( 0.5f/(float)sqrt( z ) );
			}
		}
	}
}

inline CQuaternion CreateNonUnitQuaternionFromRotationMatrix( const CMatrix& M )
{
	float tr = M.m[0*4+0] + M.m[1*4+1] + M.m[2*4+2]; // trace of martix
	if (tr > 0.0f) {
		// if trace positive than "w" is biggest component
		return CQuaternion( M.m[1*4+2]-M.m[2*4+1], M.m[2*4+0]-M.m[0*4+2], M.m[0*4+1]-M.m[1*4+0], tr + 1.0f );
	} else {
		// Some of vector components is bigger
		if( (M.m[0*4+0] > M.m[1*4+1] ) && ( M.m[0*4+0] > M.m[2*4+2]) ) {
			return CQuaternion( 1.0f + M.m[0*4+0] - M.m[1*4+1] - M.m[2*4+2], M.m[1*4+0] + M.m[0*4+1], M.m[2*4+0] + M.m[0*4+2], M.m[1*4+2] - M.m[2*4+1] );
		} else {
			if ( M.m[1*4+1] > M.m[2*4+2] ) {
				return CQuaternion( M.m[1*4+0] + M.m[0*4+1], 1.0f + M.m[1*4+1] - M.m[0*4+0] - M.m[2*4+2], M.m[2*4+1] + M.m[1*4+2], M.m[2*4+0] - M.m[0*4+2] );
			} else {
				return CQuaternion( M.m[2*4+0] + M.m[0*4+2], M.m[2*4+1] + M.m[1*4+2], 1.0f + M.m[2*4+2] - M.m[0*4+0] - M.m[1*4+1], M.m[0*4+1] - M.m[1*4+0] );
			}
		}
	}
}

inline CQuaternion Lerp( const CQuaternion& Q1, const CQuaternion& Q2, float t )
{
	return CQuaternion(	 Q1.x*(1.0f-t)+Q2.x*t
						,Q1.y*(1.0f-t)+Q2.y*t
						,Q1.z*(1.0f-t)+Q2.z*t
						,Q1.w*(1.0f-t)+Q2.w*t );
}

inline CQuaternion SLerp( const CQuaternion& Q1, const CQuaternion& Q2, float t )
{
	float CosOmega = Q1^Q2;
	if( fabs(CosOmega) > 0.95f ) {
		return Lerp(Q1,Q2,t);
	} else {
		float Omega = acosf( CosOmega );
		float SinOmega = sinf( Omega );
		float F1 = sinf( (1.0f-t)*Omega ) / SinOmega;
		float F2 = sinf( t*Omega ) / SinOmega;
		return CQuaternion(	 Q1.x*F1 + Q2.x*F2
							,Q1.y*F1 + Q2.y*F2
							,Q1.z*F1 + Q2.z*F2
							,Q1.w*F1 + Q2.w*F2	);
	}
}


#endif











