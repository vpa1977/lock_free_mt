#include "algebra.h"

CVector::CVector() : x(0.0f), y(0.0f), z(0.0f) {}
CVector::CVector(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
CVector::CVector(const CVector& V) : x(V.x), y(V.y), z(V.z) {}

CMatrix::CMatrix( const CVector& Org, const CVector& Dir, const CVector& Up)
{
	*reinterpret_cast<CVector*>(&m[12]) = Org;
	CVector LeftN( (Up*Dir).Normalize() );
	CVector DirN( Dir ); DirN.Normalize();
	CVector UpN( DirN*LeftN );
	*reinterpret_cast<CVector*>(&m[0]) = LeftN;
	*reinterpret_cast<CVector*>(&m[4]) = UpN;
	*reinterpret_cast<CVector*>(&m[8]) = DirN;
	m[3] = m[7] = m[11] = 0.0f;
	m[15] = 1.0f;
}

CMatrix& CMatrix::ConstructUnit()
{
	m[0] =  1.0f; m[1] =  0.0f; m[2] =  0.0f; m[3] =  0.0f;
	m[4] =  0.0f; m[5] =  1.0f; m[6] =  0.0f; m[7] =  0.0f;
	m[8] =  0.0f; m[9] =  0.0f; m[10] = 1.0f; m[11] = 0.0f;
	m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
	return *this;
}

CMatrix& CMatrix::ConstructRotationX( float in_Angle )
{
	float S = sin(in_Angle);
	float C = cos(in_Angle);
	m[0] =  1.0f; m[1] =  0.0f; m[2] =  0.0f; m[3] =  0.0f;
	m[4] =  0.0f; m[5] =     C; m[6] =     S; m[7] =  0.0f;
	m[8] =  0.0f; m[9] =    -S; m[10] =    C; m[11] = 0.0f;
	m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
	return *this;
}

CMatrix& CMatrix::ConstructRotationY( float in_Angle )
{
	float S = sin(in_Angle);
	float C = cos(in_Angle);
	m[0] =     C; m[1] =  0.0f; m[2] =    -S; m[3] =  0.0f;
	m[4] =  0.0f; m[5] =  1.0f; m[6] =  0.0f; m[7] =  0.0f;
	m[8] =     S; m[9] =  0.0f; m[10] =    C; m[11] = 0.0f;
	m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
	return *this;
}

CMatrix& CMatrix::ConstructRotationZ( float in_Angle )
{
	float S = sin(in_Angle);
	float C = cos(in_Angle);
	m[0] =     C; m[1] =     S; m[2] =  0.0f; m[3] =  0.0f;
	m[4] =    -S; m[5] =     C; m[6] =  0.0f; m[7] =  0.0f;
	m[8] =  0.0f; m[9] =  0.0f; m[10] = 1.0f; m[11] = 0.0f;
	m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
	return *this;
}

CMatrix& CMatrix::ConstructRotation( const CVector& in_Axis, float in_Angle )
{
	CVector v( in_Axis );
	v.Normalize();

	float c = 1.0f - cos(in_Angle);
	float s = sin(in_Angle);

	m[0]	= ( v.x*v.x - 1.0f ) * c + 1.0f;
	m[5]	= ( v.y*v.y - 1.0f ) * c + 1.0f;
	m[10]	= ( v.z*v.z - 1.0f ) * c + 1.0f;

	float a,b;

	a=v.x*v.y*c;
	b=v.z*s;
	m[1]=a+b;
	m[4]=a-b;

	a=v.x*v.z*c;
	b=v.y*s;
	m[2]=a-b;
	m[8]=a+b;

	a=v.y*v.z*c; b=v.x*s;
	m[6]=a+b;
	m[9]=a-b;

	m[7]=m[3]=m[14]=m[13]=m[12]=m[11]=0.0f;
	m[15]=1.0f;

	return (*this);
}

CMatrix& CMatrix::ConstructTranslation( const CVector& in_Shift )
{
	m[0] =  1.0f;		m[1] =  0.0f;		m[2] =  0.0f;		m[3] =  0.0f;
	m[4] =  0.0f;		m[5] =  1.0f;		m[6] =  0.0f;		m[7] =  0.0f;
	m[8] =  0.0f;		m[9] =  0.0f;		m[10] = 1.0f;		m[11] = 0.0f;
	m[12] = in_Shift.x;	m[13] = in_Shift.y;	m[14] = in_Shift.z;	m[15] = 1.0f;
	return *this;
}

CMatrix& CMatrix::ConstructScaling( const CVector& in_Scale )
{
	m[0] =  in_Scale.x;	m[1] =  0.0f;		m[2] =  0.0f;		m[3] =  0.0f;
	m[4] =  0.0f;		m[5] =  in_Scale.y;	m[6] =  0.0f;		m[7] =  0.0f;
	m[8] =  0.0f;		m[9] =  0.0f;		m[10] = in_Scale.z;	m[11] = 0.0f;
	m[12] = 0.0f;		m[13] = 0.0f;		m[14] = 0.0f;		m[15] = 1.0f;
	return *this;
}

CMatrix& CMatrix::Transpose()
{
	float f;
	f=m[1]; m[1]=m[4]; m[4]=f;
	f=m[2]; m[2]=m[8]; m[8]=f;
	f=m[3]; m[3]=m[12]; m[12]=f;
	f=m[6]; m[6]=m[9]; m[9]=f;
	f=m[7]; m[7]=m[13]; m[13]=f;
	f=m[11]; m[11]=m[14]; m[14]=f;
	return *this;
}

CMatrix Transpose( const CMatrix& M )
{
	return CMatrix(
		M.m[0], M.m[4], M.m[8], M.m[12],
		M.m[1], M.m[5], M.m[9], M.m[13],
		M.m[2], M.m[6], M.m[10], M.m[14],
		M.m[3], M.m[7], M.m[11], M.m[15]
	);
}

CMatrix& CMatrix::Invert()
{
	float tmp[12]; /* temp array for pairs */
	float src[16]; /* array of transpose source matrix */
	float det; /* determinant */
	/* transpose matrix */
	for(long i=0; i<4; ++i)
	{
		src[i] = m[i*4];
		src[i + 4] = m[i*4 + 1];
		src[i + 8] = m[i*4 + 2];
		src[i + 12] = m[i*4 + 3];
	}
	/* calculate pairs for first 8 elements (cofactors) */
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];
	/* calculate first 8 elements (cofactors) */
	m[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
	m[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
	m[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
	m[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
	m[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
	m[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
	m[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
	m[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
	m[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
	m[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
	m[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
	m[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
	m[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
	m[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
	m[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
	m[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
	/* calculate pairs for second 8 elements (cofactors) */
	tmp[0] = src[2]*src[7];
	tmp[1] = src[3]*src[6];
	tmp[2] = src[1]*src[7];
	tmp[3] = src[3]*src[5];
	tmp[4] = src[1]*src[6];
	tmp[5] = src[2]*src[5];
	tmp[6] = src[0]*src[7];
	tmp[7] = src[3]*src[4];
	tmp[8] = src[0]*src[6];
	tmp[9] = src[2]*src[4];
	tmp[10] = src[0]*src[5];
	tmp[11] = src[1]*src[4];
	/* calculate second 8 elements (cofactors) */
	m[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
	m[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
	m[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
	m[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
	m[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
	m[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
	m[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
	m[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
	m[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
	m[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
	m[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
	m[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
	m[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
	m[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
	m[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
	m[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
	/* calculate determinant */
	det=src[0]*m[0]+src[1]*m[1]+src[2]*m[2]+src[3]*m[3];
	/* calculate matrix inverse */
	det = 1.0f/det;
	for(long j=0; j<16; ++j)
		m[j]*=det;
	return *this;
}

CMatrix Invert( const CMatrix& M )
{
	CMatrix Result;
	float tmp[12]; /* temp array for pairs */
	float src[16]; /* array of transpose source matrix */
	float det; /* determinant */
	/* transpose matrix */
	for(long i=0; i<4; ++i)
	{
		src[i] = M.m[i*4];
		src[i + 4] = M.m[i*4 + 1];
		src[i + 8] = M.m[i*4 + 2];
		src[i + 12] = M.m[i*4 + 3];
	}
	/* calculate pairs for first 8 elements (cofactors) */
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];
	/* calculate first 8 elements (cofactors) */
	Result.m[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
	Result.m[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
	Result.m[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
	Result.m[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
	Result.m[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
	Result.m[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
	Result.m[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
	Result.m[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
	Result.m[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
	Result.m[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
	Result.m[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
	Result.m[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
	Result.m[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
	Result.m[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
	Result.m[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
	Result.m[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
	/* calculate pairs for second 8 elements (cofactors) */
	tmp[0] = src[2]*src[7];
	tmp[1] = src[3]*src[6];
	tmp[2] = src[1]*src[7];
	tmp[3] = src[3]*src[5];
	tmp[4] = src[1]*src[6];
	tmp[5] = src[2]*src[5];
	tmp[6] = src[0]*src[7];
	tmp[7] = src[3]*src[4];
	tmp[8] = src[0]*src[6];
	tmp[9] = src[2]*src[4];
	tmp[10] = src[0]*src[5];
	tmp[11] = src[1]*src[4];
	/* calculate second 8 elements (cofactors) */
	Result.m[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
	Result.m[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
	Result.m[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
	Result.m[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
	Result.m[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
	Result.m[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
	Result.m[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
	Result.m[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
	Result.m[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
	Result.m[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
	Result.m[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
	Result.m[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
	Result.m[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
	Result.m[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
	Result.m[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
	Result.m[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
	/* calculate determinant */
	det=src[0]*Result.m[0]+src[1]*Result.m[1]+src[2]*Result.m[2]+src[3]*Result.m[3];
	/* calculate matrix inverse */
	det = 1.0f/det;
	for(long j=0; j<16; ++j)
		Result.m[j]*=det;
	return Result;
}

CMatrix operator * (const CMatrix& M1, const CMatrix& M2)
{
	return CMatrix(
		M1.m[0]*M2.m[0] + M1.m[1]*M2.m[4] + M1.m[2]*M2.m[8] + M1.m[3]*M2.m[12],
		M1.m[0]*M2.m[1] + M1.m[1]*M2.m[5] + M1.m[2]*M2.m[9] + M1.m[3]*M2.m[13],
		M1.m[0]*M2.m[2] + M1.m[1]*M2.m[6] + M1.m[2]*M2.m[10] + M1.m[3]*M2.m[14],
		M1.m[0]*M2.m[3] + M1.m[1]*M2.m[7] + M1.m[2]*M2.m[11] + M1.m[3]*M2.m[15],

		M1.m[4]*M2.m[0] + M1.m[5]*M2.m[4] + M1.m[6]*M2.m[8] + M1.m[7]*M2.m[12],
		M1.m[4]*M2.m[1] + M1.m[5]*M2.m[5] + M1.m[6]*M2.m[9] + M1.m[7]*M2.m[13],
		M1.m[4]*M2.m[2] + M1.m[5]*M2.m[6] + M1.m[6]*M2.m[10] + M1.m[7]*M2.m[14],
		M1.m[4]*M2.m[3] + M1.m[5]*M2.m[7] + M1.m[6]*M2.m[11] + M1.m[7]*M2.m[15],

		M1.m[8]*M2.m[0] + M1.m[9]*M2.m[4] + M1.m[10]*M2.m[8] + M1.m[11]*M2.m[12],
		M1.m[8]*M2.m[1] + M1.m[9]*M2.m[5] + M1.m[10]*M2.m[9] + M1.m[11]*M2.m[13],
		M1.m[8]*M2.m[2] + M1.m[9]*M2.m[6] + M1.m[10]*M2.m[10] + M1.m[11]*M2.m[14],
		M1.m[8]*M2.m[3] + M1.m[9]*M2.m[7] + M1.m[10]*M2.m[11] + M1.m[11]*M2.m[15],

		M1.m[12]*M2.m[0] + M1.m[13]*M2.m[4] + M1.m[14]*M2.m[8] + M1.m[15]*M2.m[12],
		M1.m[12]*M2.m[1] + M1.m[13]*M2.m[5] + M1.m[14]*M2.m[9] + M1.m[15]*M2.m[13],
		M1.m[12]*M2.m[2] + M1.m[13]*M2.m[6] + M1.m[14]*M2.m[10] + M1.m[15]*M2.m[14],
		M1.m[12]*M2.m[3] + M1.m[13]*M2.m[7] + M1.m[14]*M2.m[11] + M1.m[15]*M2.m[15]
	);
}

CMatrix& CMatrix::operator *= (const CMatrix& M)
{
	*this = CMatrix(
		m[0]*M.m[0] + m[1]*M.m[4] + m[2]*M.m[8] + m[3]*M.m[12],
		m[0]*M.m[1] + m[1]*M.m[5] + m[2]*M.m[9] + m[3]*M.m[13],
		m[0]*M.m[2] + m[1]*M.m[6] + m[2]*M.m[10] + m[3]*M.m[14],
		m[0]*M.m[3] + m[1]*M.m[7] + m[2]*M.m[11] + m[3]*M.m[15],

		m[4]*M.m[0] + m[5]*M.m[4] + m[6]*M.m[8] + m[7]*M.m[12],
		m[4]*M.m[1] + m[5]*M.m[5] + m[6]*M.m[9] + m[7]*M.m[13],
		m[4]*M.m[2] + m[5]*M.m[6] + m[6]*M.m[10] + m[7]*M.m[14],
		m[4]*M.m[3] + m[5]*M.m[7] + m[6]*M.m[11] + m[7]*M.m[15],

		m[8]*M.m[0] + m[9]*M.m[4] + m[10]*M.m[8] + m[11]*M.m[12],
		m[8]*M.m[1] + m[9]*M.m[5] + m[10]*M.m[9] + m[11]*M.m[13],
		m[8]*M.m[2] + m[9]*M.m[6] + m[10]*M.m[10] + m[11]*M.m[14],
		m[8]*M.m[3] + m[9]*M.m[7] + m[10]*M.m[11] + m[11]*M.m[15],

		m[12]*M.m[0] + m[13]*M.m[4] + m[14]*M.m[8] + m[15]*M.m[12],
		m[12]*M.m[1] + m[13]*M.m[5] + m[14]*M.m[9] + m[15]*M.m[13],
		m[12]*M.m[2] + m[13]*M.m[6] + m[14]*M.m[10] + m[15]*M.m[14],
		m[12]*M.m[3] + m[13]*M.m[7] + m[14]*M.m[11] + m[15]*M.m[15]
	);
	return *this;
}

// =============================================================================================







