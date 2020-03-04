#pragma once

//============================================================================
//
// Struct
//
struct Tool
{
	DWORD id;
	DWORD idCursor;
	DWORD idBitmap;
	HWND hWnd;
	HCURSOR hCursor;
	HBITMAP hBitmap;
	HPEN hPen;
	HBRUSH hBrush;
	POINT	ptBeg;
	POINT	ptEnd;
};

//============================================================================
//
// Error Message
//
enum 
{
	MSGBOX_UNFINISHED,

};
void MsgBox( int type, HWND hWnd, const TCHAR *szTitle = NULL, const TCHAR *szText = NULL );


//============================================================================
//
// WM_*,Window messages
//
LRESULT OnCreate( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT OnLButtonDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT OnLButtonUp( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT OnMouseMove( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT OnSize( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT OnCommand( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT OnPaint( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT OnDrawItem( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT OnDestroy( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

//============================================================================
//
// Command
//
void DealWithPencil( HDC hdc, HDC hdcMem, POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen );
void DealWithBrush( HDC hdc, HDC hdcMem, POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen );
void DealWithAirbrush( HDC hdc, HDC hdcMem, POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen );
void DealWithEraser( HDC hdc, HDC hdcMem, POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen );
void DealWithText( HDC hdc, HDC hdcMem, POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen );

//============================================================================
//
// Tool
//
HPEN CreateEraser( );
HPEN CreateAirbrush( );
HBITMAP CopyBitmap( HBITMAP hBitmapSrc );
HDC CopyHdcBitmapMem( HDC hdcMemSrc );


//============================================================================
//
// Menu
//
HDC MenuEditUndo( std::vector<HDC>& hdcMemUndoStack, std::vector<HDC>& hdcMemRedoStack );
HDC MenuEditRedo( std::vector<HDC>& hdcMemRedoStack, std::vector<HDC>& hdcMemUndoStack );
