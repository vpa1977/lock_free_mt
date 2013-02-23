//---------------------------------------------------------------------------
//
//  DDFontMake - Copyright © 2000 Michael Fötsch.
//      Last Update: June 26, 2000
//
//      This program accompanies the tutorial "Fast Bitmap Fonts for
//      DirectDraw", which is available at www.mr-gamemaker.com.
//      DDFontMake creates bitmaps from TrueType, bitmap, and vector fonts.
//      These bitmaps are stored in the DDF (DirectDraw Font) format, which
//      is used by the DirectDraw font engine from that tutorial.
//      The tutorial along with a compiled version of this utility is available
//      also at www.geocities.com/foetsch.
//
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//---------------------------------------------------------------------------

#include <windows.h>
#pragma hdrstop
#include "Resfile.h"

#define WINDOWCLASSNAME "DDFontMakeWindow"
#define WINDOWCAPTION "DDFontMake - Copyright © 2000 Michael Fötsch"


POINT BlamGlyphs (HDC hdc, PTEXTMETRIC ptm, USHORT Start, USHORT End, PRECT prect, BOOL printing);

//-------------------------- Structures/Classes -----------------------------

// enum TPenTypes - Indices into arrays Pens and PenColors
enum TPenTypes {
    ptGrid,                         // used to draw grid lines
    ptBaseline,                     // used to draw the baseline of the font
    ptAbcA, ptAbcB, ptAbcC          // used to draw ABC widths of characters
};

// array PenColors - COLORREFs for the above pen types
COLORREF PenColors[5] = {
    // dark gray:       // red:         // green:       // blue:
    RGB(128, 128, 128), RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255),
    // magenta:
    RGB(255, 0, 255)
};

// array Pens - array that will store HPENs for the above pen types
HPEN Pens[5];


// CFontInfo
//      This class stores information about the currently loaded font.
//      This includes LOGFONT and CHOOSEFONT structures for use with the
//      ChooseFont dialog, as well as info about character dimensions.
//
class CFontInfo
{
public:
    CFontInfo() { hFont=NULL; }
    ~CFontInfo() { if (hFont) DeleteObject(hFont); }
    void InitChooseFont(HWND hwnd);

    HFONT hFont;                // HFONT used to draw with this font
    LOGFONT lf;                 // font description for CreateFontIndirect
    CHOOSEFONT cf;              // font description for ChooseFont dialog
    TEXTMETRIC tm;              // text metrics, e.g. character height
    ABC abc[224];               // character widths
};

// CFontInfo::InitChooseFont
//      Fill CHOOSEFONT cf so that it can be passed to ChooseFont
void CFontInfo::InitChooseFont(HWND hwnd)
{
    ZeroMemory(&lf, sizeof(lf));
    ZeroMemory(&cf, sizeof(cf));
    cf.lStructSize = sizeof(cf);
    cf.hwndOwner = hwnd;
    cf.lpLogFont = &lf;
    cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
}
//---------------------------------------------------------------------------

//-------------------------- Global Variables -------------------------------

HWND hWnd;
HINSTANCE hInst;
CFontInfo fi;                   // currently opened font
OPENFILENAME OpenFileName;      // used for GetSaveFileName
char SaveFileName[MAX_PATH];    // buffer for OpenFileName.lpstrFile

//---------------------------------------------------------------------------

// ErrMsgBox
//      Displays a MsgBox with the error message for GetLastError.
//      Uses FormatMessage to retrieve the message.
//      The code was taken from the FormatMessage help topic.
//
void ErrMsgBox()
{
    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL
    );

    // Display the string.
    MessageBox(NULL, (char*)lpMsgBuf, "GetLastError", MB_OK|MB_ICONINFORMATION);

    // Free the buffer.
    LocalFree( lpMsgBuf );
}
//---------------------------------------------------------------------------
POINT BlamGlyphs (HDC hdc, PTEXTMETRIC ptm, USHORT Start, USHORT End, PRECT prect, BOOL printing)
{
	SIZE theSize;
USHORT i;
POINT point;
WCHAR outChar;
BOOL odd;
long x, y;

odd = 0;
point.x = 0; 
point.y = 0;
 
  if (Start == 0xffff) return point;

  for (i = Start; i<=End; i++) {

	
    /* special case the non-spacing diacritic marks. U+0300 -> U+036F
     *  Write a space first, for them to 'modify.'
     */
    
    if ( (0x0300 <= i) && (i <= 0x036F) ) {
      outChar = (WCHAR) 0x0020;
      TextOutW (hdc, 0,0, &outChar, 1);
    }


    outChar = (WCHAR) i;
	char c= outChar;
	if (c == 34 )
	{
		outChar = 39;
		c = 39;
	}

	//if (!GetCharABCWidths(hdc, 32, 255, fi.abc))
	//{
		GetTextExtentPoint32(hdc,(LPCSTR) &outChar,1,&theSize);
//	}
	//else
	{
	//	theSize.cx = fi.abc->abcA +  fi.abc->abcB  + fi.abc->abcC; 
	}
	theSize.cx = abs(fi.abc[c-32].abcA) +  abs(fi.abc[c-32].abcB)  + abs(fi.abc[c-32].abcC); 



	//point.x = point.x - fi.abc[c - 32].abcA;

/*(	if (!odd ) 
	{
		int till  = fi.abc[c - 32].abcB+1 ;
		for (int k = fi.abc[c - 32].abcA; k < till; k ++ ) 
		{
			SetPixel(hdc,point.x +k, point.y, RGB(255,255,255));
		}
	}
	odd = !odd;
*/

	TextOutW (hdc, point.x -fi.abc[c-32].abcA ,point.y+1, &outChar, 1);
	

	point.x+= theSize.cx+5;
	
    /* Watch for overflow in x */
    /*if (point.x > (prect->right - ptm->tmAveCharWidth)) {
      point.x = 0;
      point.y += (ptm->tmHeight + ptm->tmExternalLeading);
      MoveToEx (hdc, point.x, point.y, NULL);
    }*/

    /* Watch for overflow in y */
   /* if (printing) {
      if (point.y > (prect->bottom-(ptm->tmHeight+ptm->tmExternalLeading))) {
        EndPage   (hdc);
        StartPage (hdc);
        point.x = point.y = 0;
        MoveToEx (hdc, point.x, point.y, NULL);
      }
    } else {
      if (point.y > prect->bottom) {
        return;
      }
    }*/
  } /* end for (i= Start; i<=End ... */
  outChar = '{';
  GetTextExtentPoint32(hdc,(LPCSTR) &outChar,1,&theSize);
  point.y = theSize.cy;
  return point;
}


// MeasureFont
//      Fills CFontInfo fi (global) with text metrics and char widths
//      -> hdc: HDC that the font is currently selected into
//
void MeasureFont(HDC hdc)
{
    GetTextMetrics(hdc, &fi.tm);
    // Measure TrueType fonts with GetCharABCWidths:
    if (!GetCharABCWidths(hdc, 32, 255, fi.abc))
    // If it's not a TT font, use GetTextExtentPoint32 to fill array abc:
    {
        ZeroMemory(fi.abc, 224*sizeof(ABC));    // set abcA and abcC to 0
                                                // (abcB will be overwritten)
        SIZE Size;
        // for all characters, beginning with " ":
        for (int i=32; i < 256; i++)
        {
            // get width of character...
            GetTextExtentPoint32(hdc, (char*)&i, 1, &Size);
            // ...and store it in abcB:
            fi.abc[i-32].abcB = Size.cx;
        }
    }
}
//---------------------------------------------------------------------------

// CMLoadFont
//      Menu item "File | LoadFont"
void CMLoadFont()
{
    // invoke ChooseFont common dialog:
    if (ChooseFont(&fi.cf))
    {
        // create an HFONT:
        if (fi.hFont) { DeleteObject(fi.hFont); fi.hFont = NULL; }
        fi.hFont = CreateFontIndirect(&fi.lf);

        // get HDC:
        HDC hdc = GetDC(hWnd);

        // select font:
        SelectObject(hdc, fi.hFont);

        // get text metrics and char widths:
        MeasureFont(hdc);

        // release HDC:
        ReleaseDC(hWnd, hdc);

        // redraw window:
        RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
    }
}


bool hasPixel(HDC hdc, int col, int height)
{
	for (int i = 0 ; i < height ; i ++ ) 
	{
		COLORREF ref = GetPixel(hdc,col,i);
		if (ref > 0 && ref !=CLR_INVALID ) 
		{
			return true;
		}
	}
	return false;
}
void freeSpace(HDC destination, int col, int height)
{
	for (int i = 1 ; i < height ; i ++ ) 
	{
		SetPixel(destination,col,i,RGB(0,0,0)) ;
	}

}
void copyPixels(HDC source, HDC destination, int col,int DestCol, int height)
{
	for (int i = 0 ; i < height ; i ++ ) 
	{
		COLORREF ref = GetPixel(source,col,i);
		SetPixel(destination,DestCol,i,ref) ;
	}

}

//---------------------------------------------------------------------------

    #define GetFilePointer(x) SetFilePointer(x, 0, 0, FILE_CURRENT)
    #define ERROR_BREAK(x) { ErrMsgBox(); throw (int)(x); }

// CMSaveFont
//      Menu item "File | Save Font"
void CMSaveFont()
{
    HDC OffscrDC=NULL;
	HDC CopyDC = NULL;
    HBITMAP OffscrBmp=NULL;
	HBITMAP COffscrBmp= NULL;
    LPBITMAPINFO lpbi=NULL;
    LPVOID lpvBits=NULL;
    HANDLE BmpFile=INVALID_HANDLE_VALUE;

try
{
    // Use fontname as filename by default:
    strcpy(OpenFileName.lpstrFile, fi.lf.lfFaceName);
    // Let user specify file to save to:
    if (!GetSaveFileName(&OpenFileName)) return;

    // Redraw window now so that the preview will not be erased later:
    RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
    UpdateWindow(hWnd);

    // Create an offscreen device context:
    OffscrDC = CreateCompatibleDC(0);
	CopyDC = CreateCompatibleDC(0);

    if (fi.hFont) SelectObject(OffscrDC, fi.hFont);

    // Create an offscreen bitmap:
    unsigned short width=4096;
    unsigned short height=256;
    HBITMAP OffscrBmp = CreateCompatibleBitmap(OffscrDC, width, height);
    if (!OffscrBmp) ERROR_BREAK(0);
    // Select bitmap into DC:
    HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
    if (!OldBmp) ERROR_BREAK(1);

    HBITMAP COffscrBmp = CreateCompatibleBitmap(CopyDC, width, height);
    if (!OffscrBmp) ERROR_BREAK(0);
    // Select bitmap into DC:
    HBITMAP COldBmp = (HBITMAP)SelectObject(CopyDC, COffscrBmp);
    if (!COldBmp) ERROR_BREAK(1);


    // Clear background to black:
    SelectObject(CopyDC, GetStockObject(BLACK_BRUSH));
    Rectangle(CopyDC, 0, 0, width, height);
    SetBkMode(CopyDC, TRANSPARENT);     // do not fill character background
    SetTextColor(CopyDC, RGB(255, 255, 255)); // text color white


    SelectObject(OffscrDC, GetStockObject(BLACK_BRUSH));
    Rectangle(OffscrDC, 0, 0, width, height);
    SetBkMode(OffscrDC, TRANSPARENT);     // do not fill character background
    SetTextColor(OffscrDC, RGB(255, 255, 255)); // text color white


    // Draw characters:
		  RECT rect;
		  TEXTMETRIC tm;
	      GetTextMetrics (OffscrDC, &tm);
		  MoveToEx (OffscrDC, 0,0, NULL);
          USHORT start, end;
          start = 32;//tm.tmFirstChar;
          end = 128;//tm.tmLastChar;
          POINT max = BlamGlyphs (OffscrDC, &tm, start, end, &rect, FALSE);
	width =  max.x+3;
	height = max.y+3;

	bool odd = true;
	bool inChar = false;
	int destCol = 0;
	for (int i = 0 ; i < 12 ; i ++ ) 
	{
		freeSpace(CopyDC,destCol,height);
		destCol++;

	}
for (int i  = 0 ; i < width; i ++ ) 
{
	//
	if (hasPixel(OffscrDC, i, height))
	{
		if (!inChar)
		{
			freeSpace(CopyDC,destCol,height);
			destCol++;
		}
		inChar = true;
		copyPixels(OffscrDC, CopyDC, i,destCol, height);
		if (odd)
		{
			SetPixel(CopyDC,destCol-1,0,RGB(255,255,255));
			SetPixel(CopyDC,destCol,0,RGB(255,255,255));
			SetPixel(CopyDC,destCol+1,0,RGB(255,255,255));
		}
		destCol ++;
	}
	else
	{
		if (inChar)
		{
			freeSpace(CopyDC,destCol,height);
			odd = !odd;
			destCol++;
		}
		
		
		inChar = false;

	}
}

width = destCol;


		  
DWORD Written;	

    // Create a file to save the DIB to:
	
    if ((BmpFile = CreateFile(OpenFileName.lpstrFile,
                              GENERIC_WRITE,
                              0, NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL)) == INVALID_HANDLE_VALUE) ERROR_BREAK(8);
	
	unsigned char header[18];
	memset(header,0,18);
	
	header[1]= 0;
	header[2] = 2;
	header[16] = 32;
	

	memcpy(header+12 , &width , sizeof(width));
	memcpy(header+14, &height , sizeof(height));

	if (!WriteFile(BmpFile, header, 18, &Written, NULL))
        ERROR_BREAK(9);




for (int j =0 ; j < height ; j ++ ) 
		{
	for (int i = 0 ; i < width ; i ++  ) 
	{

			char zero = 0;
			COLORREF colorRef = GetPixel(CopyDC,i,height - j-1);
			if (colorRef > 0 )
			{
				if (height - j-1 > 2) 
				{
					colorRef = colorRef | 0xFF000000;
					
				}
				else
				{
				colorRef  = 0xFF0000;
				}
			}

			if (!colorRef)
			{
					colorRef = 0xFF;
				
			}
			
			
			WriteFile(BmpFile, &colorRef, sizeof(COLORREF), &Written, NULL);		
		}
	}




    // Blt preview to screen:
    HDC hdc = GetDC(hWnd);
    SelectObject(OffscrDC, OffscrBmp);
    BitBlt(hdc, 0, 0, width, height, OffscrDC, 0, 0, SRCCOPY);
    ReleaseDC(hWnd, hdc);
}
catch (int &errorcode)
{
    char Buf[100];
    wsprintf(Buf, "Screenshot error #%i", errorcode);
    OutputDebugString(Buf);
}
catch (...)
{
    OutputDebugString("Screenshot error");
}
	if (CopyDC) DeleteDC(CopyDC);
    if (OffscrDC) DeleteDC(OffscrDC);
    if (OffscrBmp) DeleteObject(OffscrBmp);
	if (COffscrBmp) DeleteObject(COffscrBmp);
    if (lpbi) delete[] lpbi;
    if (lpvBits) delete[] lpvBits;
    if (BmpFile != INVALID_HANDLE_VALUE) CloseHandle(BmpFile);
}
//---------------------------------------------------------------------------

    #undef ERROR_BREAK

// WMPaint
//      WM_PAINT handler
//      Arranges the characters in a 16x14 grid. Draws grid lines as well as
//      the baseline of the font and the ABC widths of each character.
//
void WMPaint()
{
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    int y;
    if (fi.hFont) SelectObject(ps.hdc, fi.hFont);

    SetBkMode(ps.hdc, TRANSPARENT);     // do not fill character background

    SelectObject(ps.hdc, Pens[ptGrid]);         // use dark gray pen for grid
    int width=16*fi.tm.tmMaxCharWidth;
    int height=14*fi.tm.tmHeight;
/*    for (y=0; y < 15; y++)                      // draw horizontal lines
    {
        MoveToEx(ps.hdc, 0, y*fi.tm.tmHeight, NULL);
        LineTo(ps.hdc, width, y*fi.tm.tmHeight);
    }
*/

  /*  for (int x=0; x < 17; x++)                  // draw vertical lines
    {
        MoveToEx(ps.hdc, x*fi.tm.tmMaxCharWidth, 0, NULL);
        LineTo(ps.hdc, x*fi.tm.tmMaxCharWidth, height);
    }*/

    unsigned char c;
    int left, top, bottom;

    // arrange characters in 14 rows:
    for (y=0; y < 14; y++)
    {
        top = y*fi.tm.tmHeight;
        bottom = top + fi.tm.tmHeight;

        // draw baseline with red pen:
        SelectObject(ps.hdc, Pens[ptBaseline]);
        MoveToEx(ps.hdc, 0, bottom-fi.tm.tmDescent, NULL);
        LineTo(ps.hdc, width, bottom-fi.tm.tmDescent);

        // arrange characters in 16 columns per row:
        for (int x=0; x < 16; x++)
        {
            left = x*fi.tm.tmMaxCharWidth;
            c = (unsigned char)(y*16+x+32);
            // draw char
            TextOut(ps.hdc, left-fi.abc[c-32].abcA, top,(const char*)&c, 1);
                // *) abcA is subtracted because we want our character to start
                // at the left edge of its cell regardless of the value of abcA.
                // Otherwise, when abcA was negative, part of the character
                // would be visible in the preceding cell. A similar problem
                // could occur at the right edge if abcA was too big.

            // draw A width (see docs, "ABC" and "GetCharABCWidth") using green:
            SelectObject(ps.hdc, Pens[ptAbcA]);
            MoveToEx(ps.hdc, left+fi.abc[c-32].abcA, top, NULL);
            LineTo(ps.hdc, left+fi.abc[c-32].abcA, bottom);
            // draw B width using blue:
            left += fi.abc[c-32].abcB; SelectObject(ps.hdc, Pens[ptAbcB]);
            MoveToEx(ps.hdc, left, top, NULL); LineTo(ps.hdc, left, bottom);
            // draw C width using magenta:
            left += fi.abc[c-32].abcC; SelectObject(ps.hdc, Pens[ptAbcC]);
            MoveToEx(ps.hdc, left, top, NULL); LineTo(ps.hdc, left, bottom);
        }
    }

    EndPaint(hWnd, &ps);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//-------------------------- Window Procedure -------------------------------
//---------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(
    HWND hwnd,	// handle of window
    UINT uMsg,	// message identifier
    WPARAM wParam,	// first message parameter
    LPARAM lParam 	// second message parameter
)
{
    LRESULT Result=0;
    int i;
    HDC hdc;    // used in case WM_CREATE

    switch (uMsg)
    {
    case WM_COMMAND:            // Main menu
        switch (wParam)
        {
        case CM_LOADFONT: CMLoadFont(); break;
        case CM_SAVEFONT: CMSaveFont(); break;
        case CM_EXIT: DestroyWindow(hWnd); break;
        }
        break;

    case WM_CREATE:
        // Initialize CFontInfo fi:
        fi.InitChooseFont(hWnd);
        hdc = GetDC(hWnd);
        GetObject(GetCurrentObject(hdc, OBJ_FONT), sizeof(LOGFONT), &fi.lf);
        MeasureFont(hdc);
        ReleaseDC(hWnd, hdc);
        // create HPENs for the constants in TPenTypes:
        for (i=0; i < 5; i++)
            Pens[i] = CreatePen(PS_DOT, 1, PenColors[i]);
        // prepare OPENFILENAME structure for GetSaveFileName:
        ZeroMemory(&OpenFileName, sizeof(OpenFileName));
        OpenFileName.lStructSize = 76;   // = 76
            // (See remark at the end of this file!)
        OpenFileName.hwndOwner = hWnd;
        OpenFileName.lpstrFilter = "DirectDraw Font files (*.TGA)\0"
            "*.TGA\0";
        OpenFileName.nFilterIndex = 1;
        OpenFileName.lpstrFile = SaveFileName;
        OpenFileName.nMaxFile = MAX_PATH;
        OpenFileName.lpstrTitle = "Save as DirectDraw Font";
        OpenFileName.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
        OpenFileName.lpstrDefExt = "TGA";
        break;

    case WM_DESTROY:
        // destroy the HPENs in array Pens:
        for (i=0; i < 5; i++)
            DeleteObject(Pens[i]);
        PostQuitMessage(true); break;

    case WM_PAINT:
        WMPaint(); break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return Result;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//-------------------------- WinMain ----------------------------------------
//---------------------------------------------------------------------------

#pragma argsused
WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    // Save params to global vars:
    hInst = hInstance;

    // Register window class:
    WNDCLASS WndClass;
    ZeroMemory(&WndClass, sizeof(WndClass));
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = WindowProc;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    WndClass.lpszMenuName = "MAINMENU";
    WndClass.lpszClassName = WINDOWCLASSNAME;
    if (!RegisterClass(&WndClass)) {ErrMsgBox(); return false;}

    // Create window:
    hWnd = CreateWindow(WINDOWCLASSNAME,
                        WINDOWCAPTION,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        (HWND)NULL,
                        (HMENU)NULL,
                        hInstance,
                        NULL);
    if (!hWnd) { ErrMsgBox(); return false; }

    // Show the window immediately:
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Message loop:
    MSG Msg;
    while (GetMessage(&Msg, (HWND)NULL, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return Msg.wParam;
}
//---------------------------------------------------------------------------


/*

OPENFILENAME_SIZE_VERSION_400 Remark:
"With Windows 2000, the OPENFILENAME structure has increased to include some
additional members. However, this causes problems for applications on
previous operation systems. To use the current header files for applications
on Windows 95/98 and Windows NT4.0, either use the
#define "/D_WIN32_WINNT=0x0400" or use OPENFILENAME_SIZE_VERSION_400 for the
lStructSize member of OPENFILENAME."
(Taken from http://msdn.microsoft.com/library/default.asp?URL=/library/psdk/winui/commdlg3_7zqd.htm)

(BTW, the #define should be "#define _WIN32_WINNT 0x0400" in source code...)

In case the "File | Save" dialog does not work, try different values for
OpenFileName.lStructSize (in the WM_CREATE handler):
    OpenFileName.lStructSize = sizeof(OPENFILENAME);
    or
    OpenFileName.lStructSize = OPENFILENAME_VERSION_400;    // = 76
*/
