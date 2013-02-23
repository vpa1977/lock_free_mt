#ifndef SHADER_RESOURCES_HPP_INCLUDED
#define SHADER_RESOURCES_HPP_INCLUDED

struct CShaderResources
{
public:
	CShaderResources() {
		m_Required_VWorld = false;
		m_Required_VCamSpace = false;
		m_Required_NObject = false;
		m_Required_NCamSpace = false;
		m_Required_NReflection = false;
	}
	bool m_Required_VWorld;
	bool m_Required_VCamSpace;
	bool m_Required_NObject;
	bool m_Required_NCamSpace;
	bool m_Required_NReflection; // !!! If you set this one, set m_Required_VCamSpace flag also !!! 
};

#endif



































