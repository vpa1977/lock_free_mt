#include "../Include/intersect.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>

#define PRECISION 0.001f

bool
IntersectLineTriangle(
	const CVector& in_rLineOrg,
	const CVector& in_rLineDir,
	const CVector& in_rV0,
	const CVector& in_rV1,
	const CVector& in_rV2
)
{
	//CLog::Print("    IntersectLineTriangle()\n");
	//CLog::Print("      Org: {%f,%f,%f}\n",in_rLineOrg.x,in_rLineOrg.y,in_rLineOrg.z);
	//CLog::Print("      Dir: {%f,%f,%f}\n",in_rLineDir.x,in_rLineDir.y,in_rLineDir.z);
	//CLog::Print("      V0 : {%f,%f,%f}\n",in_rV0.x,in_rV0.y,in_rV0.z);
	//CLog::Print("      V1 : {%f,%f,%f}\n",in_rV1.x,in_rV1.y,in_rV1.z);
	//CLog::Print("      V2 : {%f,%f,%f}\n",in_rV2.x,in_rV2.y,in_rV2.z);
	CVector Normal( (in_rV2-in_rV0)*(in_rV1-in_rV0) ); Normal.Normalize();
	CVector Dir( in_rLineDir ); Dir.Normalize();
	float NU = Normal^Dir;
	if( fabs(NU) < PRECISION )
	{
		// dir and plane are parallel
		//CLog::Print("    return false;\n");
		return false;
	}
	else
	{
		// dir and plane intersect somewhere

		// step 1: find intersection point
		float S = -(Normal^(in_rLineOrg-in_rV0))/NU;
		CVector IntersectionPoint( in_rLineOrg+S*Dir );
		//CLog::Print("      intersection point: {%f,%f,%f}\n",IntersectionPoint.x,IntersectionPoint.y,IntersectionPoint.z);

		// step 2: check "clockiness" for all three subtriangles
		CVector SubTriangleDir = (IntersectionPoint-in_rV0)*(in_rV1-in_rV0);
		if( (SubTriangleDir^Normal) > 0 )
		{
			SubTriangleDir = (IntersectionPoint-in_rV1)*(in_rV2-in_rV1);
			if( (SubTriangleDir^Normal) > 0 )
			{
				SubTriangleDir = (IntersectionPoint-in_rV2)*(in_rV0-in_rV2);
				if( (SubTriangleDir^Normal) > 0 )
				{
					//CLog::Print("    return true;\n");
					return true;
				}
			}
		}
		//CLog::Print("    return false;\n");
		return false;
	}
}

bool
IntersectLineTriangleMesh(
	const CVector& in_rLineOrg,
	const CVector& in_rLineDir,
	const std::vector<CVector>& in_pVertices,
	unsigned long in_PrimitiveType
)
{
	//CLog::Print("  IntersectLineTriangleMesh()\n");
	if(in_PrimitiveType==PRIM_TRIANGLE_LIST)
	{
		size_t NVertices = in_pVertices.size();
		assert(NVertices>=3);
		for( size_t i=0; i<NVertices; i+=3 )
		{
			if( IntersectLineTriangle(in_rLineOrg,in_rLineDir,in_pVertices[i],in_pVertices[i+1],in_pVertices[i+2]) )
			{
				//CLog::Print("  return true;\n");
				return true;
			}
		}
		//CLog::Print("  return false;\n");
		return false;
	}
	else
	{
		assert(false);
		return false;
	}
}
















