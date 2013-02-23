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
	bool Load(const CVFSSection_ReadResponse& response );
	
	std::vector<unsigned char>& GetData(){
		return out_rData;
	}

	unsigned long GetNVertices(){
		return m_NVertices;
	}
	unsigned long GetNPrimitives(){
		return  m_NPrimitives;
	}
	unsigned long GetPrimitiveType(){
		return m_PrimitiveType;
	}
	const CVBFormat& GetVBFormat(){
		return out_rVBFormat;
	}

private:
	unsigned long m_NVertices;
	unsigned long m_NPrimitives;
	unsigned long m_PrimitiveType;
	CVBFormat out_rVBFormat;
	std::vector<unsigned char> out_rData;
	

	void DecodeVBFormatString( char* in_pszBuffer, CVBFormat& out_rVBFormat );
	void DecodePrimitiveTypeString( char* in_pszBuffer, unsigned long& out_rPrimitiveType );
	bool DecodeVertexString( char* in_pszBuffer, const CVBFormat& in_rVBFormat, std::vector<unsigned char>& out_rData );
	unsigned long CalcNPrimitives(unsigned long in_PrimitiveType, unsigned long in_NVertices);
};


#endif