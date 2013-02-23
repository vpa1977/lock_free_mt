#include "algebra.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define GOLDEN 1.618f

// ===================================================================================================

void
GenerateTriangle
(	FILE* f,
	const CVector& in_v1,
	const CVector& in_v2,
	const CVector& in_v3,
	float tu1, float tv1,
	float tu2, float tv2,
	float tu3, float tv3
)
{
	fprintf(f,"%f %f %f    %f %f %f    ffffff    %f %f\n", in_v1.x,in_v1.y,in_v1.z, in_v1.x,in_v1.y,in_v1.z, tu1,tv1 );
	fprintf(f,"%f %f %f    %f %f %f    ffffff    %f %f\n", in_v2.x,in_v2.y,in_v2.z, in_v2.x,in_v2.y,in_v2.z, tu2,tv2 );
	fprintf(f,"%f %f %f    %f %f %f    ffffff    %f %f\n", in_v3.x,in_v3.y,in_v3.z, in_v3.x,in_v3.y,in_v3.z, tu3,tv3 );
}

// ===================================================================================================

void
ProcessBigTriangle
(	FILE* f,
	long in_Segments,
	const CVector& in_v1,
	const CVector& in_v2,
	const CVector& in_v3,
	float tu1, float tv1,
	float tu2, float tv2,
	float tu3, float tv3
)
{
	assert(f);
	assert(in_Segments>0 && in_Segments<100);

	CVector v1(in_v1); v1.Normalize();
	CVector v2(in_v2); v2.Normalize();
	CVector v3(in_v3); v3.Normalize();

	CVector Tex1(tu1,tv1,0); 
	CVector Tex2(tu2,tv2,0); 
	CVector Tex3(tu3,tv3,0); 

	CVector BaseSmallBegin(v1);
	CVector BaseSmallEnd(v1);

	CVector TexSmallBegin(Tex1); 
	CVector TexSmallEnd(Tex1); 

	for( long i=0; i<in_Segments; ++i )
	{
printf("Segment=%ld\n",i);
		float Fraction = float(i+1) / float(in_Segments);
		CVector BaseBigBegin = Interpolate(Fraction,v1,v3);
		BaseBigBegin.Normalize();
		CVector BaseBigEnd   = Interpolate(Fraction,v1,v2);
		BaseBigEnd.Normalize();
		CVector TexBigBegin = Interpolate(Fraction,Tex1,Tex3);
		CVector TexBigEnd   = Interpolate(Fraction,Tex1,Tex2);
		//
		CVector PrevBig   = BaseBigBegin;
		CVector PrevSmall = BaseSmallBegin;
		CVector PrevTexBig = TexBigBegin;
		CVector PrevTexSmall = TexSmallBegin;
		for( long j=0; j<=i; ++j )
		{
printf("  j=%ld\n",j);
			CVector NextBig = Interpolate( float(j+1)/float(i+1), BaseBigBegin, BaseBigEnd );
			NextBig.Normalize();
			CVector NextTexBig = Interpolate( float(j+1)/float(i+1), TexBigBegin, TexBigEnd );
printf("    big\n");
			GenerateTriangle(
				f,
				PrevBig,
				PrevSmall,
				NextBig,
				PrevTexBig.x, PrevTexBig.y,
				PrevTexSmall.x, PrevTexSmall.y,
				NextTexBig.x, NextTexBig.y
			);
			if( j < i )
			{
				CVector NextSmall = Interpolate( float(j+1)/float(i), BaseSmallBegin, BaseSmallEnd );
				NextSmall.Normalize();
				CVector NextTexSmall = Interpolate( float(j+1)/float(i), TexSmallBegin, TexSmallEnd );
printf("    small\n");
				GenerateTriangle(
					f,
					NextBig,
					PrevSmall,
					NextSmall,
					NextTexBig.x, NextTexBig.y,
					PrevTexSmall.x, PrevTexSmall.y,
					NextTexSmall.x, NextTexSmall.y
				);
				PrevSmall = NextSmall;
				PrevTexSmall = NextTexSmall;
			}
			PrevBig = NextBig;
			PrevTexBig = NextTexBig;
		}
		BaseSmallBegin = BaseBigBegin;
		BaseSmallEnd   = BaseBigEnd;
		TexSmallBegin = TexBigBegin;
		TexSmallEnd   = TexBigEnd;
	}
}

// ===================================================================================================

void
GenerateSphere
(FILE* f, long in_Segments)
{
	assert(f);
	assert(in_Segments>0 && in_Segments<100);

	float RotateAngle = atan(1.0f/GOLDEN);
	CMatrix mr;
	mr.ConstructRotationZ(RotateAngle);

	CVector v00(	0.0f,	 1.0f,	 GOLDEN	);		v00=v00*mr;
	CVector v01(	0.0f,    1.0f,  -GOLDEN	);		v01=v01*mr;
	CVector v02(	0.0f,   -1.0f,   GOLDEN	);		v02=v02*mr;
	CVector v03(	0.0f,   -1.0f,  -GOLDEN	);		v03=v03*mr;

	CVector v04(	 1.0f,	 GOLDEN,	0.0f	);	v04=v04*mr;
	CVector v05(	 1.0f,	-GOLDEN,	0.0f	);	v05=v05*mr;
	CVector v06(	-1.0f,	 GOLDEN,	0.0f	);	v06=v06*mr;
	CVector v07(	-1.0f,	-GOLDEN,	0.0f	);	v07=v07*mr;

	CVector v08(	 GOLDEN,	0.0f,	 1.0f	);	v08=v08*mr;
	CVector v09(	 GOLDEN,	0.0f,	-1.0f	);	v09=v09*mr;
	CVector v10(	-GOLDEN,	0.0f,	 1.0f	);	v10=v10*mr;
	CVector v11(	-GOLDEN,	0.0f,	-1.0f	);	v11=v11*mr;

	// 5 triangles around vertex 04
	ProcessBigTriangle(f,in_Segments,v04,v00,v06,	0.0909, 0.0000, 0.1818, 0.3333, 0.0000, 0.3333 );
	ProcessBigTriangle(f,in_Segments,v04,v08,v00,	0.2727, 0.0000, 0.3636, 0.3333, 0.1818, 0.3333 );
	ProcessBigTriangle(f,in_Segments,v04,v09,v08,	0.4545, 0.0000, 0.5455, 0.3333, 0.3636, 0.3333 );
	ProcessBigTriangle(f,in_Segments,v04,v01,v09,	0.6364, 0.0000, 0.7273, 0.3333, 0.5455, 0.3333 );
	ProcessBigTriangle(f,in_Segments,v04,v06,v01,	0.8182, 0.0000, 0.9091, 0.3333, 0.7273, 0.3333 );

	// 10 triangles in belt
	ProcessBigTriangle(f,in_Segments,v10,v06,v00,	0.0909, 0.6667, 0.0000, 0.3333, 0.1818, 0.3333 );
	ProcessBigTriangle(f,in_Segments,v02,v00,v08,	0.2727, 0.6667, 0.1818, 0.3333, 0.3636, 0.3333 );
	ProcessBigTriangle(f,in_Segments,v05,v08,v09,	0.4545, 0.6667, 0.3636, 0.3333, 0.5455, 0.3333 );
	ProcessBigTriangle(f,in_Segments,v03,v09,v01,	0.6364, 0.6667, 0.5455, 0.3333, 0.7273, 0.3333 );
	ProcessBigTriangle(f,in_Segments,v11,v01,v06,	0.8182, 0.6667, 0.7273, 0.3333, 0.9091, 0.3333 );
	//
	ProcessBigTriangle(f,in_Segments,v00,v02,v10,	0.1818, 0.3333, 0.2727, 0.6667, 0.0909, 0.6667 );
	ProcessBigTriangle(f,in_Segments,v08,v05,v02,	0.3636, 0.3333, 0.4545, 0.6667, 0.2727, 0.6667 );
	ProcessBigTriangle(f,in_Segments,v09,v03,v05,	0.5455, 0.3333, 0.6364, 0.6667, 0.4545, 0.6667 );
	ProcessBigTriangle(f,in_Segments,v01,v11,v03,	0.7273, 0.3333, 0.8182, 0.6667, 0.6364, 0.6667 );
	ProcessBigTriangle(f,in_Segments,v06,v10,v11,	0.9091, 0.3333, 0.9999, 0.6667, 0.8182, 0.6667 );

	// 5 triangles around vertex 07
	ProcessBigTriangle(f,in_Segments,v07,v10,v02,	0.1818, 1.0000, 0.0909, 0.6667, 0.2727, 0.6667 );
	ProcessBigTriangle(f,in_Segments,v07,v02,v05,	0.3636, 1.0000, 0.2727, 0.6667, 0.4545, 0.6667 );
	ProcessBigTriangle(f,in_Segments,v07,v05,v03,	0.5455, 1.0000, 0.4545, 0.6667, 0.6364, 0.6667 );
	ProcessBigTriangle(f,in_Segments,v07,v03,v11,	0.7273, 1.0000, 0.6364, 0.6667, 0.8182, 0.6667 );
	ProcessBigTriangle(f,in_Segments,v07,v11,v10,	0.9091, 1.0000, 0.8182, 0.6667, 1.0000, 0.6667 );
}

// ===================================================================================================

int
main
(int argc, char* argv[])
{
	long nSegments = 5;
	assert(nSegments>0 && nSegments<100);

	FILE* f = fopen("object.txt","wt");
	assert(f);

	fprintf(f,"D3DFVF_XYZ D3DFVF_NORMAL D3DFVF_DIFFUSE D3DFVF_TEX0\nPRIM_TRIANGLE_LIST\n");
	GenerateSphere( f, nSegments );

	fclose(f);
	return 0;
}










