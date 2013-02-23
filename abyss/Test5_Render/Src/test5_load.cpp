#include "../Include/test5_section.h"
#include "../../MT_Core/Include/log.h"
#include <assert.h>
#include <stdio.h>

class CFile
{
public:
	CFile(const char* in_pszFileName) : m_Handle(NULL)
	{
		assert(in_pszFileName);
		m_Handle = fopen(in_pszFileName,"rt");
		assert(m_Handle);
	}
	~CFile()
	{
		fclose(m_Handle);
	}
	FILE* GetHandle() const { return m_Handle; }
private:
	FILE* m_Handle;
};

void
DecodeVBFormatString( char* in_pszBuffer, CVBFormat& out_rVBFormat )
{
	char* token = strtok(in_pszBuffer," \t\n");
	assert(token);
	while( token )
	{
		if( 0==strcmp(token,"D3DFVF_XYZ") )
		{
			out_rVBFormat.m_XYZ = true;
		}
		else if( 0==strcmp(token,"D3DFVF_NORMAL") )
		{
			out_rVBFormat.m_Normal = true;
		}
		else if( 0==strcmp(token,"D3DFVF_DIFFUSE") )
		{
			out_rVBFormat.m_Diffuse = true;
		}
		token = strtok(NULL," \t\n");
	}
}

void
DecodePrimitiveTypeString( char* in_pszBuffer, unsigned long& out_rPrimitiveType )
{
	char* token = strtok(in_pszBuffer," \t\n");
	assert(token);
	if( 0 == strcmp(token,"PRIM_POINT_LIST") )
		out_rPrimitiveType = PRIM_POINT_LIST;
	else if( 0 == strcmp(token,"PRIM_LINE_LIST") )
		out_rPrimitiveType = PRIM_LINE_LIST;
	else if( 0 == strcmp(token,"PRIM_LINE_STRIP") )
		out_rPrimitiveType = PRIM_LINE_STRIP;
	else if( 0 == strcmp(token,"PRIM_TRIANGLE_LIST") )
		out_rPrimitiveType = PRIM_TRIANGLE_LIST;
	else if( 0 == strcmp(token,"PRIM_TRIANGLE_STRIP") )
		out_rPrimitiveType = PRIM_TRIANGLE_STRIP;
	else if( 0 == strcmp(token,"PRIM_TRIANGLE_FAN") )
		out_rPrimitiveType = PRIM_TRIANGLE_FAN;
	assert(0!=out_rPrimitiveType);
}

bool
DecodeVertexString( char* in_pszBuffer, const CVBFormat& in_rVBFormat, std::vector<unsigned char>& out_rData )
{
	assert(in_rVBFormat.m_XYZ==true);
	assert(in_pszBuffer);
	if(in_pszBuffer[0]!='\0' && in_pszBuffer[0]!='#' && in_pszBuffer[0]!='\n' && in_pszBuffer[0]!='\r')
	{
		unsigned char Buffer[1024];
		unsigned long NextElement = 0;

		// read vertex position
		CVector V;
		char* token = strtok(in_pszBuffer," \t\n");
		assert(token);
		V.x = atof(token);
		token = strtok(NULL," \t\n");
		assert(token);
		V.y = atof(token);
		token = strtok(NULL," \t\n");
		assert(token);
		V.z = atof(token);
		*((CVector*)(Buffer+NextElement)) = V;
		NextElement += 3*sizeof(float);
CLog::Print("vertex = {%f,%f,%f}\n",V.x,V.y,V.z);

		if(in_rVBFormat.m_Normal)
		{
			// read normal
			CVector N;
			token = strtok(NULL," \t\n");
			assert(token);
			N.x = atof(token);
			token = strtok(NULL," \t\n");
			assert(token);
			N.y = atof(token);
			token = strtok(NULL," \t\n");
			assert(token);
			N.z = atof(token);
			*((CVector*)(Buffer+NextElement)) = N;
			NextElement += 3*sizeof(float);
CLog::Print("  normal = {%f,%f,%f}\n",N.x,N.y,N.z);
		}

		if(in_rVBFormat.m_Diffuse)
		{
			// read diffuse color
			unsigned long D=0;
			token = strtok(NULL," \t\n");
			assert(token);
			int n = sscanf(token,"%x",&D);
			assert(1==n);
			*((unsigned long*)(Buffer+NextElement)) = D;
			NextElement += sizeof(unsigned long);
CLog::Print("  diffuse = %06x\n",D);
		}

		unsigned long Size = out_rData.size();
		out_rData.resize( Size + NextElement );
		unsigned char* Dst = &(out_rData[Size]);
		memcpy(Dst,Buffer,NextElement);

		return true;
	}
	else
		return false;
}

bool
CTest5Section::LoadDataFromFile(
	const char* in_pszFileName,
	CVBFormat& out_rVBFormat,
	std::vector<unsigned char>& out_rData,
	unsigned long& out_rPrimitiveType,
	unsigned long& out_rNPrimitives
)
{
	bool Ok = false;
	do
	{
		// open file
		CFile File(in_pszFileName);

		char Buffer[1024];

		// get vertex format
		out_rVBFormat.m_Diffuse = false;
		out_rVBFormat.m_Normal = false;
		out_rVBFormat.m_Specular = false;
		out_rVBFormat.m_XYZ = false;
		out_rVBFormat.m_XYZRHW = false;
		out_rVBFormat.m_Tex0Dimension = 0;
		out_rVBFormat.m_Tex1Dimension = 0;
		out_rVBFormat.m_Tex2Dimension = 0;
		out_rVBFormat.m_Tex3Dimension = 0;
		if( NULL == fgets(Buffer,1023,File.GetHandle()) )
			break;
		DecodeVBFormatString( Buffer, out_rVBFormat );

		// get primitive type
		out_rPrimitiveType = 0;
		if( NULL == fgets(Buffer,1023,File.GetHandle()) )
			break;
		DecodePrimitiveTypeString( Buffer, out_rPrimitiveType );

		// get vertex data
		out_rData.clear();
		out_rNPrimitives = 0;
		while( NULL != fgets(Buffer,1023,File.GetHandle()) )
		{
			if(DecodeVertexString( Buffer, out_rVBFormat, out_rData ))
			{
				out_rNPrimitives++;
			}
		}
		assert( !out_rData.empty() );

		Ok = true;
	} while (false);
	return Ok;
}




























