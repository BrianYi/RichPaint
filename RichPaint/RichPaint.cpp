#include "stdafx.h"
#include "Header.h"
#include "RichPaint.h"

extern HINSTANCE hInst;

BOOL DlgOnInitAnimation( HWND hWnd, HWND hWndFocus, LPARAM lParam )
{
	// Load and register animation control class.
// 	INITCOMMONCONTROLSEX iccx;
// 	iccx.dwSize = sizeof( INITCOMMONCONTROLSEX );
// 	iccx.dwICC = ICC_ANIMATE_CLASS;
// 	if ( !InitCommonControlsEx( &iccx ) )
// 	{
// 		MsgBox( MSBBOX_INIT_COMCTL32_FAILED, hWnd,
// 				TEXT( "Initialization" ), TEXT( "Init ICC_ANIMATE_CLASS failed!" ) );
// 	}

	// Create the animation control
	RECT rc = { 20,20,280,60 };
	HWND hAnimate = Animate_Create( hWnd, ID_ANIMATION, WS_CHILD | WS_VISIBLE |
									ACS_AUTOPLAY | ACS_TIMER | ACS_TRANSPARENT, hInst );
	SetWindowPos( hAnimate, 0, rc.left, rc.top,
				  rc.right, rc.bottom,
				  SWP_NOZORDER | SWP_DRAWFRAME );

	Animate_Open( hAnimate, MAKEINTRESOURCE( IDR_UPLOAD_AVI ) );

	return TRUE;
}

BOOL DlgOnCommand( HWND hwnd, int id, HWND hwndCtl, UINT codeNotify )
{
	switch ( id )
	{
	case IDOK:
	case IDCANCEL:
		EndDialog( hwnd, id );
		break;
	default:
		break;
	}
	return TRUE;
}

BOOL DlgOnClose( HWND hWnd )
{
	EndDialog( hWnd, 0 );
	return TRUE;
}

void DealWithPencil( HDC hdc, HDC hdcMem, 
					 POINT ptMouseStart, POINT ptMouseEnd,
					 HPEN hPen)
{
	MoveToEx( hdc, ptMouseStart.x, ptMouseStart.y, NULL );
	MoveToEx( hdcMem, ptMouseStart.x, ptMouseStart.y, NULL );

	DeleteObject( SelectObject( hdc, hPen ) );
	DeleteObject( SelectObject( hdcMem, hPen ) );

	LineTo( hdc, ptMouseEnd.x, ptMouseEnd.y );
	LineTo( hdcMem, ptMouseEnd.x, ptMouseEnd.y );
}

void DealWithBrush( HDC hdc, HDC hdcMem, 
					POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen )
{
	MoveToEx( hdc, ptMouseStart.x, ptMouseStart.y, NULL );
	MoveToEx( hdcMem, ptMouseStart.x, ptMouseStart.y, NULL );

	DeleteObject( SelectObject( hdc, hPen ) );
	DeleteObject( SelectObject( hdcMem, hPen ) );

	LineTo( hdc, ptMouseEnd.x, ptMouseEnd.y );
	LineTo( hdcMem, ptMouseEnd.x, ptMouseEnd.y );
}

void DealWithAirbrush( HDC hdc, HDC hdcMem, 
					   POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen )
{
	MoveToEx( hdc, ptMouseStart.x, ptMouseStart.y, NULL );
	MoveToEx( hdcMem, ptMouseStart.x, ptMouseStart.y, NULL );

	DeleteObject( SelectObject( hdc, hPen ) );
	DeleteObject( SelectObject( hdcMem, hPen ) );

	LineTo( hdc, ptMouseEnd.x, ptMouseEnd.y );
	LineTo( hdcMem, ptMouseEnd.x, ptMouseEnd.y );
}

void DealWithEraser( HDC hdc, HDC hdcMem, 
					 POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen )
{
	MoveToEx( hdc, ptMouseStart.x, ptMouseStart.y, NULL );
	MoveToEx( hdcMem, ptMouseStart.x, ptMouseStart.y, NULL );

	DeleteObject( SelectObject( hdc, hPen ) );
	DeleteObject( SelectObject( hdcMem, hPen ) );

	LineTo( hdc, ptMouseEnd.x, ptMouseEnd.y );
	LineTo( hdcMem, ptMouseEnd.x, ptMouseEnd.y );
}

void DealWithText( HDC hdc, HDC hdcMem, 
				   POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen )
{
	HCURSOR hCursor = LoadCursor( NULL, IDC_CROSS );
	SetCursor( hCursor );


}

extern OPENFILENAME ofn;
extern TCHAR szFilter[ ];
extern TCHAR szFileTitle[ MAX_PATH ];
extern TCHAR szFileName[ MAX_PATH ];
void DealInitCommonDlg(HWND hWnd )
{
	// OPENFILENAME
	ZeroMemory( szFileTitle, MAX_PATH * sizeof TCHAR );
	ZeroMemory( szFileName, MAX_PATH * sizeof TCHAR );
	ZeroMemory( &ofn, sizeof OPENFILENAME );
	ofn.lStructSize = sizeof OPENFILENAME;
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = szFilter;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrDefExt = TEXT( "bmp" );
	ofn.lpstrFileTitle = szFileTitle;
	ofn.lpstrFile = szFileName;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
}

void DealClearUndoStack( std::vector<HDC>& hdcMemUndoStack )
{
	while ( hdcMemUndoStack.size( ) > 1 )
	{
		DeleteDC( hdcMemUndoStack.back( ) );
		hdcMemUndoStack.pop_back( );
	}
}

void DealClearRedoStack( std::vector<HDC>& hdcMemRedoStack )
{
	while ( !hdcMemRedoStack.empty( ) )
	{
		DeleteDC( hdcMemRedoStack.back( ) );
		hdcMemRedoStack.pop_back( );
	}
}

void DealClearUndoRedoStack( std::vector<HDC>& hdcMemUndoStack, 
							 std::vector<HDC>& hdcMemRedoStack )
{
	DealClearUndoStack( hdcMemUndoStack );
	DealClearRedoStack( hdcMemRedoStack );
}

HPEN CreateEraser( )
{
	LOGBRUSH lgbrsh;
	lgbrsh.lbStyle = BS_SOLID;
	lgbrsh.lbColor = RGB( 255, 255, 255 );
	lgbrsh.lbHatch = 0;
	return ExtCreatePen( PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_ROUND,
						 15, &lgbrsh, 0, NULL );
}

HPEN CreateAirbrush( )
{
	LOGBRUSH lgbrsh;
	lgbrsh.lbStyle = BS_PATTERN;
	lgbrsh.lbColor = 0;
	lgbrsh.lbHatch = ( ULONG_PTR ) LoadBitmap( hInst,
											   MAKEINTRESOURCE( IDB_AIRBRUSH_EFFECT ) );
	return ExtCreatePen( PS_GEOMETRIC,
						 24, &lgbrsh, 0, NULL );
}

HBITMAP CopyBitmap( HBITMAP hBitmapSrc )
{
	BITMAP bitmapSrc;
	HBITMAP hBitmapDst;
	HDC hdcSrc, hdcDst;

	GetObject( hBitmapSrc, sizeof BITMAP, &bitmapSrc );
	hBitmapDst = CreateBitmapIndirect( &bitmapSrc );

	hdcSrc = CreateCompatibleDC( NULL );
	hdcDst = CreateCompatibleDC( NULL );

	SelectObject( hdcSrc, hBitmapSrc );
	SelectObject( hdcDst, hBitmapDst );
	PatBlt( hdcDst, 0, 0, GetSystemMetrics( SM_CXSCREEN ), GetSystemMetrics( SM_CYSCREEN ), WHITENESS );

	BitBlt( hdcDst, 0, 0, bitmapSrc.bmWidth, bitmapSrc.bmHeight,
			hdcSrc, 0, 0, SRCCOPY );

	DeleteDC( hdcSrc );
	DeleteDC( hdcDst );
	return hBitmapDst;
}

HDC CopyHdcBitmapMem( HDC hdcMemSrc )
{
	if ( hdcMemSrc == NULL ) return NULL;

	int cx = GetSystemMetrics( SM_CXSCREEN );
	int cy = GetSystemMetrics( SM_CYSCREEN );
	HDC hdcMemDst = CreateCompatibleDC( NULL );
	HBITMAP hBitmapDst = CreateCompatibleBitmap( hdcMemSrc, cx, cy );
	SelectObject( hdcMemDst, hBitmapDst );
	BitBlt( hdcMemDst, 0, 0, cx, cy,
			hdcMemSrc, 0, 0, SRCCOPY );
	return hdcMemDst;
}

HBITMAP CreateDIBSectionFromDIBFile( const TCHAR* pszFileName )
{
	HANDLE hFile = CreateFile( pszFileName, GENERIC_READ, FILE_SHARE_READ,
							   NULL, OPEN_EXISTING, 0, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
		return NULL;

	BITMAPFILEHEADER bmfh;
	BITMAPINFO *pbmi;
	DWORD dwBytesRead, dwInfoSize;

	BOOL bSuccess = ReadFile( hFile, &bmfh, sizeof BITMAPFILEHEADER, &dwBytesRead, NULL );
	if ( !bSuccess || dwBytesRead != sizeof BITMAPFILEHEADER ||
		 bmfh.bfType != *( WORD* )"BM" )
	{
		CloseHandle( hFile );
		return NULL;
	}

	dwInfoSize = bmfh.bfOffBits - sizeof BITMAPFILEHEADER;

	pbmi = ( BITMAPINFO * ) malloc( dwInfoSize );
	bSuccess = ReadFile( hFile, pbmi, dwInfoSize, &dwBytesRead, NULL );
	if ( !bSuccess || dwBytesRead != dwInfoSize )
	{
		DWORD dwError = GetLastError( );

		free( pbmi );
		CloseHandle( hFile );
		return NULL;
	}

	BYTE *pBits;
	HBITMAP hBitmap = CreateDIBSection( NULL, pbmi, DIB_RGB_COLORS,
										(void **)&pBits, NULL, 0 );
	if ( hBitmap == NULL )
	{
		free( pbmi );
		CloseHandle( hFile );
		return NULL;
	}

	ReadFile( hFile, pBits, bmfh.bfSize - bmfh.bfOffBits, &dwBytesRead, NULL );

	free( pbmi );
	CloseHandle( hFile );

	return hBitmap;
}

BOOL SaveDIBtoFile( HDC hdc, HBITMAP hBitmap, const TCHAR* pszFileName )
{
	//============================================================================
	//
	// build a DIB file structure(BITMAPFILEHEADER + BITMAPINFO + Color BITS)
	// packed DIB is BITMAPINFO + Color BITS
	// 
	BITMAP bm;
	GetObject( hBitmap, sizeof BITMAP, &bm );

	BITMAPINFO bmi;
	ZeroMemory( &bmi, sizeof BITMAPINFO );
	BITMAPINFOHEADER &bmih = bmi.bmiHeader;
	bmih.biSize = sizeof BITMAPINFOHEADER;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;
	bmih.biWidth = bm.bmWidth;
	bmih.biHeight = bm.bmHeight;
	bmih.biPlanes = 1;
	
	DWORD dwBitsBytes = ( bm.bmWidth * bm.bmBitsPixel + 31 ) / 32 * 4 * bm.bmHeight;
	BYTE *pBytes = ( BYTE * ) malloc( dwBitsBytes * sizeof BYTE );
	GetDIBits( hdc, hBitmap,
			   0, bm.bmHeight,
			   pBytes,
			   &bmi, DIB_RGB_COLORS );

	BITMAPFILEHEADER bmfh;
	ZeroMemory( &bmfh, sizeof BITMAPFILEHEADER );
	bmfh.bfOffBits = sizeof BITMAPFILEHEADER + sizeof BITMAPINFO;
	bmfh.bfType = *( WORD * )"BM";
	bmfh.bfSize = bmfh.bfOffBits + dwBitsBytes;
	

	//============================================================================
	//
	// write to file
	//
	HANDLE hFile = CreateFile( szFileName, GENERIC_WRITE, 0, NULL,
							   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	DWORD dwBytesWritten;
	DWORD dwBytesOfDIBFile = 0;
	
	BOOL bSuccess = WriteFile( hFile, &bmfh, sizeof BITMAPFILEHEADER, &dwBytesWritten, NULL );
	dwBytesOfDIBFile += dwBytesWritten;
	if ( !bSuccess ) goto fail;

	bSuccess = WriteFile( hFile, &bmi, sizeof BITMAPINFO, &dwBytesWritten, NULL );
	dwBytesOfDIBFile += dwBytesWritten;
	if ( !bSuccess ) goto fail;
	
	bSuccess = WriteFile( hFile, pBytes, dwBitsBytes, &dwBytesWritten, NULL );
	dwBytesOfDIBFile += dwBytesWritten;
	if ( !bSuccess || dwBytesOfDIBFile != bmfh.bfSize) goto fail;

fail:
	if (!bSuccess )
	{
		CloseHandle( hFile );
		free( pBytes );
		return FALSE;
	}

	CloseHandle( hFile );
	free( pBytes );
	return TRUE;
}

HDC MenuEditUndo( std::vector<HDC>& hdcMemUndoStack, std::vector<HDC>& hdcMemRedoStack )
{
	if ( hdcMemUndoStack.size() == 0 )
	{
		MessageBox( NULL, TEXT( "ERROR:MenuEditUndo hdcMemUndoStack.size() == 0" ),
					TEXT( "ERROR" ), MB_ICONERROR );
		return NULL;
	}

	if ( hdcMemUndoStack.size( ) > 1 )
	{
		hdcMemRedoStack.push_back( hdcMemUndoStack.back( ) );
		hdcMemUndoStack.pop_back( );
	}
	return CopyHdcBitmapMem( hdcMemUndoStack.back( ) );
}

HDC MenuEditRedo( std::vector<HDC>& hdcMemRedoStack, std::vector<HDC>& hdcMemUndoStack )
{

	if ( hdcMemUndoStack.size( ) == 0 )
	{
		MessageBox( NULL, TEXT( "ERROR:MenuEditRedo hdcMemUndoStack.size() == 0" ),
					TEXT( "ERROR" ), MB_ICONERROR );
		return NULL;
	}

	if ( !hdcMemRedoStack.empty( ) )
	{
		hdcMemUndoStack.push_back( hdcMemRedoStack.back( ) );
		hdcMemRedoStack.pop_back( );
	}
	return CopyHdcBitmapMem( hdcMemUndoStack.back( ) );
}

void DebugShowPosition( HDC hdc, HDC hdcMem, int x, int y, POINT pt )
{
	TCHAR szBuffer[ 128 ];
	wsprintf( szBuffer, TEXT( "(%-4d, %-4d)" ), pt.x, pt.y );
	TextOut( hdc, x, y, szBuffer, lstrlen( szBuffer ) );
	TextOut( hdcMem, x, y, szBuffer, lstrlen( szBuffer ) );
}

void _MsgBox( int uID, TCHAR *szBuffer, const TCHAR *szStr = NULL )
{
	if ( szStr )
		lstrcpy( szBuffer, szStr );
	else
		LoadString( hInst, uID, szBuffer, MAX_LOADSTRING );
}

void MsgBox( int type, HWND hWnd, const TCHAR *szTitle/* = NULL*/, const TCHAR *szText/* = NULL*/ )
{
	TCHAR szCaption[ MAX_LOADSTRING ];
	TCHAR szContent[ MAX_LOADSTRING ];

	switch ( type )
	{
	case MSGBOX_UNFINISHED:
		_MsgBox( IDS_MSGBOX_UNFINISHED_TITLE, szCaption, szTitle );
		_MsgBox( IDS_MSGBOX_UNFINISHED_TEXT, szContent, szText );
		MessageBox( hWnd, szContent, szCaption, MB_ICONINFORMATION );
		break;
	case MSGBOX_FILE_SAVE_SUCCESS:
		MessageBox( hWnd, szText, szTitle, MB_ICONINFORMATION );
		break;
	case MSGBOX_FILE_SAVE_FAILED:
		MessageBox( hWnd, szText, szTitle, MB_ICONWARNING );
		break;
	case MSBBOX_INIT_COMCTL32_FAILED:
		MessageBox( hWnd, szText, szTitle, MB_ICONERROR );
		break;
	default:
		break;
	}
}