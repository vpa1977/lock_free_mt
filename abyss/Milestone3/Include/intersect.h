#ifndef INTERSECT_HPP_INCLUDED
#define INTERSECT_HPP_INCLUDED

#include "../../Render_DX8/Include/algebra.h"
#include "../../Common/Include/primitive_types.h"
#include <vector>
bool
IntersectLineTriangle(
	const CVector& in_rLineOrg,
	const CVector& in_rLineDir,
	const CVector& in_rV0,
	const CVector& in_rV1,
	const CVector& in_rV2
);

bool
IntersectLineTriangleMesh(
	const CVector& in_rLineOrg,
	const CVector& in_rLineDir,
	const std::vector<CVector>& in_pVertices,
	unsigned long in_PrimitiveType
);

#endif






















