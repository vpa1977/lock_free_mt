#include "../Include/CVertexLoader.h"
#include "../../MT_Core/Include/log.h"


bool CVertexLoader::Load(const CVFSSection_ReadResponse& response )
{
		
	    char* buffer;
		buffer = (char*)malloc (response.m_Size+1);
		memset(buffer, 0, response.m_Size + 1 ) ;
		memcpy(buffer, &(response.m_pData[0]), response.m_Size);

		std::vector<std::string> vLines;
		
		char* nextLine = strtok(buffer, "\r\n");
		while (nextLine ) 
		{
			vLines.push_back(nextLine);
			nextLine = strtok(NULL, "\r\n" );
		}
		if (vLines.size() < 2 ) 
		{
			return false;
		}
		

	
		m_PrimitiveType = 0;


		char Buffer[1024];

		// get vertex format
		out_rVBFormat.m_Diffuse = false;
		out_rVBFormat.m_Normal = false;
		out_rVBFormat.m_Specular = false;
		out_rVBFormat.m_XYZ = false;
		out_rVBFormat.m_XYZRHW = false;
/*		out_rVBFormat.m_Tex0Dimension = 0;
		out_rVBFormat.m_Tex1Dimension = 0;
		out_rVBFormat.m_Tex2Dimension = 0;
		out_rVBFormat.m_Tex3Dimension = 0;*/
		

		DecodeVBFormatString( (char*) vLines[0].data(), out_rVBFormat );

		// get primitive type
		m_PrimitiveType = 0;
		DecodePrimitiveTypeString( (char*) vLines[1].data(), m_PrimitiveType );

		// get vertex data
		out_rData.clear();
		m_NVertices = 0;
		int i =2;
		while( i < vLines.size() )
		{

			if(DecodeVertexString((char*) vLines[i].data(), out_rVBFormat, out_rData ))
			{
				m_NVertices++;
			}
			i++;
		}
		

	CLog::Print("  m_PrimitiveType=%lu\n",m_PrimitiveType);
	CLog::Print("  m_NVertices=%lu\n",m_NVertices);
	m_NPrimitives = CalcNPrimitives(m_PrimitiveType,m_NVertices);

	return true;
}


////
void
CVertexLoader::DecodeVBFormatString( char* in_pszBuffer, CVBFormat& out_rVBFormat )
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
		else if( 0==strcmp(token,"D3DFVF_TEX0") )
		{
			out_rVBFormat.m_Tex0Dimension = CVBFormat::TEXCOORDSET_FLAT;
		}
		token = strtok(NULL," \t\n");
	}
}

void
CVertexLoader::DecodePrimitiveTypeString( char* in_pszBuffer, unsigned long& out_rPrimitiveType )
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
CVertexLoader::DecodeVertexString( char* in_pszBuffer, const CVBFormat& in_rVBFormat, std::vector<unsigned char>& out_rData )
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

		if(in_rVBFormat.m_Tex0Dimension)
		{
			assert(in_rVBFormat.m_Tex0Dimension==2);
			float u,v;
			//
			token = strtok(NULL," \t\n");
			assert(token);
			u = atof(token);
			*((float*)(Buffer+NextElement)) = u;
			NextElement += sizeof(float);
			//
			token = strtok(NULL," \t\n");
			assert(token);
			v = atof(token);
			*((float*)(Buffer+NextElement)) = v;
			NextElement += sizeof(float);
			//
CLog::Print("  texcoords0 = {%f,%f}\n",u,v);
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