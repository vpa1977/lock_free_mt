#include "../Include/vertex_loader.h"
#include "../../MT_Core/Include/log.h"

#include <string.h>
#include <stdio.h>

std::string
SafeStrtok(const char* in_pszString, const char* in_pszSymbols, const char*& out_rNext)
{
	assert(in_pszString && in_pszSymbols);
	assert( strlen(in_pszSymbols) > 0 );
	size_t Len = strlen(in_pszString);
	if(Len==0)
	{
		out_rNext = NULL;
		return "";
	}
	else
	{
		size_t NSkip = strspn(in_pszString,in_pszSymbols);
		assert(NSkip<=Len);
		if(NSkip==Len)
		{
			out_rNext = NULL;
			return "";
		}
		else
		{
			const char* pBegin = in_pszString+NSkip;
			size_t SubstringLen = strcspn(pBegin,in_pszSymbols);
			assert(SubstringLen>0);
			out_rNext = pBegin+SubstringLen;
			return std::string(pBegin,SubstringLen);
		}
	}
}

struct CBufferHolder
{
	CBufferHolder(size_t in_Size)
	{
		m_Data = new char[in_Size];
	}
	~CBufferHolder()
	{
		assert(m_Data);
		delete [] m_Data;
	}
	char* m_Data;
};

bool
CVertexLoader::Load(const std::vector<unsigned char>& in_rData  )
{
	CBufferHolder Buffer(in_rData.size()+1);
	memcpy(Buffer.m_Data,&in_rData[0],in_rData.size());
	Buffer.m_Data[in_rData.size()] = '\0';

	std::vector<std::string> Lines;
	const char* pData = Buffer.m_Data;
	do
	{
		std::string NextLine = SafeStrtok(pData,"\r\n",pData);
		if(pData)
		{
			Lines.push_back( NextLine );
			//CLog::Print( "%s\n", NextLine.c_str() );
		}
	} while(pData!=NULL);
	if( Lines.size() < 2 ) 
	{
		return false;
	}

	// get vertex format
	m_VBFormat.m_Diffuse = false;
	m_VBFormat.m_Normal = false;
	m_VBFormat.m_Specular = false;
	m_VBFormat.m_XYZ = false;
	m_VBFormat.m_XYZRHW = false;
	m_VBFormat.m_Tex0Dimension = CVBFormat::TEXCOORDSET_NONE;
	m_VBFormat.m_Tex1Dimension = CVBFormat::TEXCOORDSET_NONE;
	m_VBFormat.m_Tex2Dimension = CVBFormat::TEXCOORDSET_NONE;
	m_VBFormat.m_Tex3Dimension = CVBFormat::TEXCOORDSET_NONE;
	DecodeVBFormatString( Lines[0].c_str(), m_VBFormat );

	// get primitive type
	DecodePrimitiveTypeString( Lines[1].c_str(), m_PrimitiveType );
//	CLog::Print("  m_PrimitiveType=%lu\n",m_PrimitiveType);

	// get vertex data
	m_Data.clear();
	m_IntersectData.clear();
	m_NVertices = 0;
	unsigned int i = 2;
	while( i < Lines.size() )
	{
		if( DecodeVertexString( Lines[i].c_str(), m_VBFormat, m_Data, m_IntersectData ) )
		{
			m_NVertices++;
		}
		i++;
	}
//	CLog::Print("  m_NVertices=%lu\n",m_NVertices);
	m_NPrimitives = CalcNPrimitives(m_PrimitiveType,m_NVertices);

	return true;
}

void
CVertexLoader::DecodeVBFormatString( const char* in_pszBuffer, CVBFormat& out_rVBFormat )
{
	assert(in_pszBuffer);
	do
	{
		std::string Token = SafeStrtok(in_pszBuffer," \t\n",in_pszBuffer);
		if(in_pszBuffer)
		{
			const char* pToken = Token.c_str();
			if( 0==strcmp(pToken,"D3DFVF_XYZ") )
			{
				out_rVBFormat.m_XYZ = true;
			}
			else if( 0==strcmp(pToken,"D3DFVF_NORMAL") )
			{
				out_rVBFormat.m_Normal = true;
			}
			else if( 0==strcmp(pToken,"D3DFVF_DIFFUSE") )
			{
				out_rVBFormat.m_Diffuse = true;
			}
			else if( 0==strcmp(pToken,"D3DFVF_TEX0") )
			{
				out_rVBFormat.m_Tex0Dimension = CVBFormat::TEXCOORDSET_FLAT;
			}
		}
	} while(in_pszBuffer);
}

void
CVertexLoader::DecodePrimitiveTypeString( const char* in_pszBuffer, unsigned long& out_rPrimitiveType )
{
	assert(in_pszBuffer);
	std::string Token = SafeStrtok(in_pszBuffer," \t\n",in_pszBuffer);
	assert(in_pszBuffer);
	const char* pToken = Token.c_str();
	out_rPrimitiveType = 0;
	if( 0 == strcmp(pToken,"R8PRIM_POINT_LIST") )
		out_rPrimitiveType = PRIM_POINT_LIST;
	else if( 0 == strcmp(pToken,"R8PRIM_LINE_LIST") )
		out_rPrimitiveType = PRIM_LINE_LIST;
	else if( 0 == strcmp(pToken,"R8PRIM_LINE_STRIP") )
		out_rPrimitiveType = PRIM_LINE_STRIP;
	else if( 0 == strcmp(pToken,"R8PRIM_TRIANGLE_LIST") )
		out_rPrimitiveType = PRIM_TRIANGLE_LIST;
	else if( 0 == strcmp(pToken,"R8PRIM_TRIANGLE_STRIP") )
		out_rPrimitiveType = PRIM_TRIANGLE_STRIP;
	else if( 0 == strcmp(pToken,"R8PRIM_TRIANGLE_FAN") )
		out_rPrimitiveType = PRIM_TRIANGLE_FAN;
	assert(0!=out_rPrimitiveType);
}

bool
CVertexLoader::DecodeVertexString
(
	const char* in_pszBuffer
	,const CVBFormat& in_rVBFormat
	,std::vector<unsigned char>& out_rData
	,std::vector<CVector>& out_rIntersectData
)
{
	assert(in_pszBuffer);
	assert(in_rVBFormat.m_XYZ==true);
	if(in_pszBuffer[0]!='\0' && in_pszBuffer[0]!='#' && in_pszBuffer[0]!='\n' && in_pszBuffer[0]!='\r')
	{
		unsigned char Buffer[1024];
		unsigned long NextElement = 0;

		// read vertex position
		CVector V;
		std::string Token = SafeStrtok(in_pszBuffer," \t\n",in_pszBuffer);
		assert(in_pszBuffer);
		V.x = (float)atof(Token.c_str());
		Token = SafeStrtok(in_pszBuffer," \t\n",in_pszBuffer);
		assert(in_pszBuffer);
		V.y = (float)atof(Token.c_str());
		Token = SafeStrtok(in_pszBuffer," \t\n",in_pszBuffer);
		assert(in_pszBuffer);
		V.z = (float)atof(Token.c_str());
		*((CVector*)(Buffer+NextElement)) = V;
		NextElement += 3*sizeof(float);
//CLog::Print("vertex = {%f,%f,%f}\n",V.x,V.y,V.z);

		assert( NextElement == sizeof(CVector) );
		size_t Size = out_rIntersectData.size();
		out_rIntersectData.resize( Size + 1 );
		out_rIntersectData[Size] = V;

		if(in_rVBFormat.m_Normal)
		{
			// read normal
			CVector N;
			Token = SafeStrtok(in_pszBuffer," \t\n",in_pszBuffer);
			assert(in_pszBuffer);
			N.x =(float) atof(Token.c_str());
			Token = SafeStrtok(in_pszBuffer," \t\n",in_pszBuffer);
			assert(in_pszBuffer);
			N.y = (float)atof(Token.c_str());
			Token = SafeStrtok(in_pszBuffer," \t\n",in_pszBuffer);
			assert(in_pszBuffer);
			N.z = (float)atof(Token.c_str());
			*((CVector*)(Buffer+NextElement)) = N;
			NextElement += 3*sizeof(float);
//CLog::Print("  normal = {%f,%f,%f}\n",N.x,N.y,N.z);
		}

		if(in_rVBFormat.m_Diffuse)
		{
			// read diffuse color
			unsigned long D=0;
			Token = SafeStrtok(in_pszBuffer," \t\n",in_pszBuffer);
			assert(in_pszBuffer);
			int n = sscanf(Token.c_str(),"%x",&D);
			assert(1==n);
			*((unsigned long*)(Buffer+NextElement)) = D;
			NextElement += sizeof(unsigned long);
//CLog::Print("  diffuse = %06x\n",D);
		}

		if(in_rVBFormat.m_Tex0Dimension)
		{
			assert(in_rVBFormat.m_Tex0Dimension==2);
			float u,v;
			//
			Token = SafeStrtok(in_pszBuffer," \t\n",in_pszBuffer);
			assert(in_pszBuffer);
			u =(float) atof(Token.c_str());
			*((float*)(Buffer+NextElement)) = u;
			NextElement += sizeof(float);
			//
			Token = SafeStrtok(in_pszBuffer," \t\n",in_pszBuffer);
			assert(in_pszBuffer);
			v = (float)atof(Token.c_str());
			*((float*)(Buffer+NextElement)) = v;
			NextElement += sizeof(float);
			//
//CLog::Print("  texcoords0 = {%f,%f}\n",u,v);
		}

		Size = out_rData.size();
		out_rData.resize( Size + NextElement );
		unsigned char* Dst = &(out_rData[Size]);
		memcpy(Dst,Buffer,NextElement);

		return true;
	}
	else
		return false;
}

unsigned long
CVertexLoader::CalcNPrimitives(unsigned long in_PrimitiveType, unsigned long in_NVertices)
{
	assert(in_PrimitiveType>=PRIM_POINT_LIST && in_PrimitiveType<=PRIM_TRIANGLE_FAN);
	switch(in_PrimitiveType)
	{
		case PRIM_POINT_LIST:
			return in_NVertices;
		case PRIM_LINE_LIST:
			return in_NVertices/2;
		case PRIM_LINE_STRIP:
			return in_NVertices-1;
		case PRIM_TRIANGLE_LIST:
			return in_NVertices/3;
		case PRIM_TRIANGLE_STRIP:
		case PRIM_TRIANGLE_FAN:
			return in_NVertices-2;
	}
	return 0;
}
