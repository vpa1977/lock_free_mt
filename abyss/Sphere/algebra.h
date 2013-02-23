#ifndef ALGEBRA_HPP_INCLUDED
#define ALGEBRA_HPP_INCLUDED
 
#include <math.h>
#include <memory.h>
#include <assert.h>
 
// =====================================================================================================
 
class CMatrix;
 
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
 
 // stuff
 friend CVector Interpolate(float in_Fraction, const CVector& V1, const CVector& V2);
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
};
#pragma pack()
 
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
 
inline CVector Interpolate(float in_Fraction, const CVector& V1, const CVector& V2)
{
 assert(0.0f<=in_Fraction && in_Fraction<=1.0f);
 return V1 + in_Fraction*(V2-V1);
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
 m[0]+=M.m[0]; m[1]+=M.m[1]; m[2]+=M.m[2]; m[3]+=M.m[3];
 m[4]+=M.m[4]; m[5]+=M.m[5]; m[6]+=M.m[6]; m[7]+=M.m[7];
 m[8]+=M.m[8]; m[9]+=M.m[9]; m[10]+=M.m[10]; m[11]+=M.m[11];
 m[12]+=M.m[12]; m[13]+=M.m[13]; m[14]+=M.m[14]; m[15]+=M.m[15];
 return *this;
}
 
inline CMatrix& CMatrix::operator -= (const CMatrix& M)
{
 m[0]-=M.m[0]; m[1]-=M.m[1]; m[2]-=M.m[2]; m[3]-=M.m[3];
 m[4]-=M.m[4]; m[5]-=M.m[5]; m[6]-=M.m[6]; m[7]-=M.m[7];
 m[8]-=M.m[8]; m[9]-=M.m[9]; m[10]-=M.m[10]; m[11]-=M.m[11];
 m[12]-=M.m[12]; m[13]-=M.m[13]; m[14]-=M.m[14]; m[15]-=M.m[15];
 return *this;
}
 
#endif
