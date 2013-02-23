#include "../Render_DX8/Include/algebra.h"
#include <stdio.h>

void Dump( const CQuaternion& Q ) {
	printf("%f %f %f %f\n",Q.x,Q.y,Q.z,Q.w);
}

int main( int argc, char* argv[] ) {
	printf("Test quaternions.\n");

	{
		printf("\n=== 1 =============================================\n");
		CQuaternion Q1;
		Dump(Q1);
	}

	{
		printf("\n=== 2 =============================================\n");
		CQuaternion Q2( CVector(0.0f,0.0f,666.0f), (float)CONST_PI );
		Dump(Q2);
	}

	{
		printf("\n=== 3 =============================================\n");
		CQuaternion Q3( 1, 1, 1, 1 );
		Dump(Q3);
		printf("%f\n", Q3.Norm() );
		printf("%f\n", Q3.Length() );
		Q3.Normalize();
		Dump(Q3);
	}

	{
		printf("\n=== 4 =============================================\n");
		CQuaternion Q4( 1, 1, 1, 1 );
		Dump(Q4);
		CQuaternion Q41 = !Q4;
		Dump(Q41);
		CQuaternion Q42 = -Q4;
		Dump(Q42);
	}

	{
		printf("\n=== 5 =============================================\n");
		CQuaternion Q51( 1, 2, 3, 4 );
		Dump(Q51);
		CQuaternion Q52( 1, 2, 3, -4 );
		Dump(Q52);
		printf("%f\n", Q51^Q52 );
	}

	{
		printf("\n=== 6 =============================================\n");
		CQuaternion Q61( 1, 2, 3, 4 );
		Dump(Q61);
		CQuaternion Q62( 5, 6, 7, 8 );
		Dump(Q62);
		Dump( Q61 + Q62 );
		Dump( Q61 - Q62 );
		Dump( Q61 += Q62 );
		Dump( Q61 -= Q62 );
	}

	{
		printf("\n=== 7 =============================================\n");
		CQuaternion Q71( 1, 2, 3, 4 );
		Q71.Normalize();
		Dump(Q71);
		printf("%f\n",Q71.Length());

		//CQuaternion Q72( 5, 6, 7, 8 );
		//Q72.Normalize();
		//Dump(Q72);
		//Dump( Q71 * Q72 );

		CQuaternion Q73 = -Q71;
		Dump( Q73 );
		printf("%f\n",Q73.Length());

		CQuaternion Product = Q71 * Q73;
		Dump( Product );
		printf("%f\n",Product.Length());

		Dump( Q71*=Q73 );
	}

	{
		printf("\n=== 8 =============================================\n");
		CQuaternion Q8( CVector(0.0f,0.0f,1.0f), (float)CONST_PI_2 );
		//CQuaternion Q8( CVector(0,0,1), CONST_PI );
		Dump(Q8);
		CVector Src(1,0,0);
		CVector Dst = RotateVectorByQuaternion( Src, Q8 );
		printf("%f %f %f\n",Dst.x,Dst.y,Dst.z);
	}

	{
		printf("\n=== 9 =============================================\n");
		CQuaternion Q81( CVector(0,0,1), 0 );
		Dump(Q81);
		CQuaternion Q82( CVector(0,0,1), CONST_PI );
		Dump(Q82);
		for( float t=0.0f; t<1.0f; t+=0.1f ) {
			CQuaternion R = SLerp(Q81,Q82,t);
			Dump( R );
		}
	}

	{
		printf("\n=== 10 ============================================\n");
		CQuaternion Q10( CVector(0,0,1), CONST_PI_2 );
		//CQuaternion Q10( CVector(0,0,1), CONST_PI );
		Dump(Q10);

		CVector V;
		float A;
		Q10.ToAxisAngle( V, A );
		printf("%f %f %f\n",V.x,V.y,V.z);
		printf("%f\n",A);

		CVector Src(1,0,0);
		CVector Dst1 = RotateVectorByQuaternion( Src, Q10 );
		printf("%f %f %f\n",Dst1.x,Dst1.y,Dst1.z);
		CVector Dst11 = Src*Q10;
		printf("%f %f %f\n",Dst11.x,Dst11.y,Dst11.z);

		CMatrix M( Q10.ToMatrix() );
		CVector Dst2 = Src*M;
		printf("%f %f %f\n",Dst2.x,Dst2.y,Dst2.z);

		CMatrix M3;
		M3.ConstructRotation( CVector(0,0,1), CONST_PI_2 );
		CVector Dst3 = Src*M3;
		printf("%f %f %f\n",Dst3.x,Dst3.y,Dst3.z);

		CMatrix M4;
		M4.ConstructRotationZ( CONST_PI_2 );
		CVector Dst4 = Src*M4;
		printf("%f %f %f\n",Dst4.x,Dst4.y,Dst4.z);
	}

	{
		printf("\n=== 11 =============================================\n");
		CMatrix M11;
		M11.ConstructRotation( CVector(0.3f,-0.78f,-0.9f), 0.666 );
		CQuaternion Q11 = CreateNonUnitQuaternionFromRotationMatrix( M11 );
		Q11.Normalize();
		CQuaternion Q111 = CreateUnitQuaternionFromRotationMatrix( M11 );
		Q111.Normalize();

		CVector V11( 0.0f, 0.0f, 1.0f );

		CVector R1 = V11*M11;
		printf("%f %f %f\n",R1.x,R1.y,R1.z);

		CVector R2 = RotateVectorByQuaternion( V11, Q11 );
		printf("%f %f %f\n",R2.x,R2.y,R2.z);

		CVector R3 = RotateVectorByQuaternion( V11, Q111 );
		printf("%f %f %f\n",R3.x,R3.y,R3.z);
	}

	{
		printf("\n=== 12 =============================================\n");
		CQuaternion Q1( CVector(1.0f,1.0f,1.0f), (float)CONST_PI_3 );
		Q1.Normalize();
		CQuaternion Q2( CVector(1.0f,1.0f,1.0f), (float)CONST_PI_3 );
		Q2.Normalize();
		CQuaternion Q3 = SLerp(Q1,Q2,0.5f);
		Dump(Q3);
	}
}































