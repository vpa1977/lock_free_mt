// a1.cpp : Defines the entry point for the console application.


#pragma once

#include <stdio.h>
#include <windows.h>

#include "../include/exportscene.h"
#include "../Include/export_scene_util.h"
#include "../../MT_Core/Include/Log.h"




// Macro that calls a COM method returning HRESULT value:
#define HRCALL(a, errmsg) \
do { \
    hr = (a); \
    if (FAILED(hr)) { \
        CLog::Print( "%s:%d  HRCALL Failed: %s\n  0x%.8x = %s\n", \
                __FILE__, __LINE__, errmsg, hr, #a ); \
        goto clean; \
    } \
} while (0)

// Helper function that put output in stdout and debug window
// in Visual Studio:

// Helper function to create a DOM instance:
IXMLDOMDocument * DomFromCOM()
{
   HRESULT hr;
   IXMLDOMDocument *pxmldoc = NULL;

    HRCALL( CoCreateInstance(__uuidof(DOMDocument),
                      NULL,
                      CLSCTX_INPROC_SERVER,
                      __uuidof(IXMLDOMDocument),
                      (void**)&pxmldoc),
            "Create a new DOMDocument");

    HRCALL( pxmldoc->put_async(VARIANT_FALSE),
            "should never fail");
    HRCALL( pxmldoc->put_validateOnParse(VARIANT_FALSE),
            "should never fail");
    HRCALL( pxmldoc->put_resolveExternals(VARIANT_FALSE),
            "should never fail");

   return pxmldoc;
clean:
   if (pxmldoc)
    {
      pxmldoc->Release();
    }
   return NULL;
}


int main(int argc, char* argv[])
{
	CLog::Init();
	CExportScene sc;
   IXMLDOMDocument *pXMLDom=NULL;
   IXMLDOMParseError *pXMLErr=NULL;
   BSTR bstr = NULL;
   VARIANT_BOOL status;
   VARIANT var;
   HRESULT hr;

   CoInitialize(NULL);

   char* model_name;
   char* output_path;
   std::string file_name;
   if (argc < 1 ) 
   {
	   CLog::Print("Model name is required\n" );
	   return -1;
   }
   
   model_name = argv[1];
   std::string file_str(model_name);
   if (argc > 2 ) 
	   output_path = argv[2];
   else
	   output_path = ".";

   if (argc > 3 ) 
       file_name = argv[3];
   else
   {
	
	   int start = file_str.find_last_of("\\");
	   int start2  = file_str.find_last_of("/");
	   start = max(start, start2);
	   if (start < 0 ) 
	   {
		   start = -1;
	   }
	   int end = file_str.find_last_of(".");
	   if (end < 0 ) 
	   {
		   end = file_str.length();
	   }
	  
	   
	   file_name = file_str.substr(start+1, (end-start-1));

   }
   pXMLDom = DomFromCOM();
   if (!pXMLDom) goto clean;


     // Convert to a wchar_t*
    size_t origsize = strlen(model_name) + 1;
    const size_t newsize = 255;
    size_t convertedChars = 0;
    wchar_t wcstring[newsize];
    mbstowcs(wcstring, model_name,origsize);

	
   VariantInit(&var);
   V_BSTR(&var) = SysAllocString(wcstring);
   //V_BSTR(&var) = SysAllocString(L"C:/monastery.DAE");
   V_VT(&var) = VT_BSTR;
   HRCALL(pXMLDom->load(var, &status), "");
   
   sc.Export(pXMLDom, output_path,file_name.c_str());
   //sc.ExportMeshes(pXMLDom, output_path,file_name);
   //sc.ExportAnimations(pXMLDom, output_path, file_name);
  

   if (status!=VARIANT_TRUE) {
      HRCALL(pXMLDom->get_parseError(&pXMLErr),"");
      HRCALL(pXMLErr->get_reason(&bstr),"");
      CLog::Print("Failed to load input file. %S\n",
               bstr);
      goto clean;
   }
   HRCALL(pXMLDom->get_xml(&bstr), "");
	


clean:
   if (bstr) SysFreeString(bstr);
   if (&var) VariantClear(&var);
   if (pXMLErr) pXMLErr->Release();
   if (pXMLDom) pXMLDom->Release();
   CLog::Close();	
   CoUninitialize();
   return 0;
}


