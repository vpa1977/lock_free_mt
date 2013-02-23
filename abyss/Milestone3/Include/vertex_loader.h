#ifndef CVERTEX_LOADER_HPP

#define CVERTEX_LOADER_HPP

#include "../../MT_Core/Include/section.h"
#include "../../MT_Core/Include/system_commands.h"
#include "../../MT_Core/Include/command_sender.h"
#include "../../MT_Core/Include/command_sender.h"

#include "../../Game_Base/Include/gamebase_section.h"


class CVertexLoader
{
public:
	bool Load(const std::vector<unsigned char>& in_rData );

	std::vector<unsigned char>& GetData(){
		return m_Data;
	}
	std::vector<CVector>& GetIntersectData(){
		return m_IntersectData;
	}
	unsigned long GetNVertices(){
		return m_NVertices;
	}
	unsigned long GetNPrimitives(){
		return m_NPrimitives;
	}
	unsigned long GetPrimitiveType(){
		return m_PrimitiveType;
	}
	const CVBFormat& GetVBFormat(){
		return m_VBFormat;
	}

private:
	unsigned long m_NVertices;
	unsigned long m_NPrimitives;
	unsigned long m_PrimitiveType;
	CVBFormat m_VBFormat;
	std::vector<unsigned char> m_Data;
	std::vector<CVector> m_IntersectData;

	void DecodeVBFormatString( const char* in_pszBuffer, CVBFormat& out_rVBFormat );
	void DecodePrimitiveTypeString( const char* in_pszBuffer, unsigned long& out_rPrimitiveType );
	bool DecodeVertexString( const char* in_pszBuffer, const CVBFormat& in_rVBFormat, std::vector<unsigned char>& out_rData, std::vector<CVector>& out_rIntersectData );
	unsigned long CalcNPrimitives(unsigned long in_PrimitiveType, unsigned long in_NVertices);
};


#endif