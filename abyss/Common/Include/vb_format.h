#ifndef VB_FORMAT_FLAGS_HPP_INCLUDED
#define VB_FORMAT_FLAGS_HPP_INCLUDED

struct CVBFormat
{
	enum TEXCOORDSET_THICKNESS {
		// DO NOT CHANGE NUMERIC VALUES !!!
		TEXCOORDSET_NONE			= 0, // this coordset is not used
		TEXCOORDSET_ONEDIMENSIONAL	= 1, // 1 float
		TEXCOORDSET_FLAT			= 2, // 2 floats
		TEXCOORDSET_CUBIC			= 3  // 3 floats
	};
	CVBFormat() :
		m_XYZ(false),
		m_XYZRHW(false),
		m_NBonesPerVertex(0),
		m_Normal(false),
		m_Diffuse(false),
		m_Specular(false),
		m_Tex0Dimension(TEXCOORDSET_NONE),
		m_Tex1Dimension(TEXCOORDSET_NONE),
		m_Tex2Dimension(TEXCOORDSET_NONE),
		m_Tex3Dimension(TEXCOORDSET_NONE)
	{}
	bool m_XYZ;			// 3 floats, {X,Y,Z}
	bool m_XYZRHW;

	// Up to 4 bones per vertex possible.
	// Sum of all USED bone weights must be equal to 1.0
	//
	// Actual data layout is:
	//
	// float BoneWeight0; (if any)
	// float BoneWeight1; (if any)
	// float BoneWeight2; (if any)
	// unsigned long Indices; - each byte contains index
	//
	// for ONE bone per vertex:
	// indices ONLY
	// 
	// for TWO bones per vertex
	// weight0, indices
	// 
	// for THREE bones per vertex
	// weight0, weight1, indices
	// 
	// for FOUR bones per vertex
	// weight0, weight1, weight2, indices
	long m_NBonesPerVertex;	// 0,1,2,3,4

	bool m_Normal;		// 3 floats, {Nx,Ny,Nz}
	bool m_Diffuse;		// unsigned long {Color}
	bool m_Specular;	// unsigned long {Specular}
	TEXCOORDSET_THICKNESS m_Tex0Dimension;
	TEXCOORDSET_THICKNESS m_Tex1Dimension;
	TEXCOORDSET_THICKNESS m_Tex2Dimension;
	TEXCOORDSET_THICKNESS m_Tex3Dimension;

};

#endif









