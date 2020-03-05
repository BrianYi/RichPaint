#include "stdafx.h"
#include "Header.h"
#include "RichPaint.h"

extern HINSTANCE hInst;

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
	default:
		break;
	}
}