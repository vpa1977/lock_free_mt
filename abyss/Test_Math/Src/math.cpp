#include "../Include/algebra.h"
#include <stdio.h>

void DumpVector( const CVector& V )
{
	printf("%f %f %f\n",V.x,V.y,V.z);
}

void DumpMatrix( const CMatrix& M )
{
	printf("%f %f %f %f\n",M.m[0],M.m[1],M.m[2],M.m[3]);
	printf("%f %f %f %f\n",M.m[4],M.m[5],M.m[6],M.m[7]);
	printf("%f %f %f %f\n",M.m[8],M.m[9],M.m[10],M.m[11]);
	printf("%f %f %f %f\n",M.m[12],M.m[13],M.m[14],M.m[15]);
}

#define M_PI 3.1415926535897

int
main(int argc, char* argv[])
{
	{
		CMatrix M1;
		M1.ConstructRotation( CVector(13,1,-7), 0.2f );
		M1 *= CMatrix().ConstructTranslation( CVector(10,-2,6) );
		DumpMatrix(M1);
printf("----------------------------------------\n");
		CMatrix M2(M1);
		M2.Invert();
		DumpMatrix(M2);
printf("----------------------------------------\n");
		CMatrix M3( Invert(M1) );
		DumpMatrix(M3);
printf("----------------------------------------\n");
		DumpMatrix(M1*M2);
printf("----------------------------------------\n");
		DumpMatrix(M1*M3);
}
	return 0;
}






















