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
	HBRUSH hBrush;
	POINT	ptBeg;
	POINT	ptEnd;
};

//============================================================================
//
// Class related
//
ATOM                MyRegisterClass( HINSTANCE hInstance );
BOOL                InitInstance( HINSTANCE, int );

//============================================================================
//
// Wind Procedure
//
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK    About( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    TransparentWndProc( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK AnimationDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );


//============================================================================
//
// WM_*,Window messages
//
BOOL OnCreate( HWND hWnd, LPCREATESTRUCT lpCreateStruct );
BOOL OnLButtonDown( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags );
BOOL OnLButtonUp( HWND hWnd, int x, int y, UINT keyFlags );
BOOL OnMouseMove( HWND hWnd, int x, int y, UINT keyFlags );
BOOL OnSize( HWND hWnd, UINT state, int cx, int cy );
BOOL OnCommand( HWND hWnd, int id, HWND hwndCtl, UINT codeNotify );
BOOL OnPaint( HWND hWnd );
BOOL OnDrawItem( HWND hWnd, const DRAWITEMSTRUCT * lpDrawItem );
BOOL OnDestroy( HWND hwnd );


//============================================================================
//
// Dialog
//
BOOL DlgOnInitAnimation( HWND hWnd, HWND hWndFocus, LPARAM lParam );
BOOL DlgOnCommand( HWND hwnd, int id, HWND hwndCtl, UINT codeNotify );
BOOL DlgOnClose( HWND hWnd );

//============================================================================
//
// Command
//
void DealWithPencil( HDC hdc, HDC hdcMem, POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen );
void DealWithBrush( HDC hdc, HDC hdcMem, POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen );
void DealWithAirbrush( HDC hdc, HDC hdcMem, POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen );
void DealWithEraser( HDC hdc, HDC hdcMem, POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen );
void DealWithText( HDC hdc, HDC hdcMem, POINT ptMouseStart, POINT ptMouseEnd, HPEN hPen );
void DealInitCommonDlg( HWND hWnd );
void DealClearUndoStack( std::vector<HDC>& hdcMemUndoStack );
void DealClearRedoStack( std::vector<HDC>& hdcMemRedoStack );
void DealClearUndoRedoStack( std::vector<HDC>& hdcMemUndoStack, std::vector<HDC>& hdcMemRedoStack );
void DealInitAnimComCtl( HWND hWnd );

//============================================================================
//
// Tool
//
HPEN CreateEraser( );
HPEN CreateAirbrush( );
HBITMAP CopyBitmap( HBITMAP hBitmapSrc );
HDC CopyHdcBitmapMem( HDC hdcMemSrc );
HBITMAP CreateDIBSectionFromDIBFile( const TCHAR* pszFileName );
BOOL SaveDIBtoFile( HDC hdc, HBITMAP hBitmap, const TCHAR* pszFileName );


//============================================================================
//
// Menu
//
HDC MenuEditUndo( std::vector<HDC>& hdcMemUndoStack, std::vector<HDC>& hdcMemRedoStack );
HDC MenuEditRedo( std::vector<HDC>& hdcMemRedoStack, std::vector<HDC>& hdcMemUndoStack );


//============================================================================
//
// Debug tools
//
void DebugShowPosition( HDC hdc, HDC hdcMem, int x, int y, POINT pt );


//============================================================================
//
// Message Box
//
enum
{
	MSGBOX_UNFINISHED,
	MSGBOX_FILE_SAVE_SUCCESS,
	MSGBOX_FILE_SAVE_FAILED,
	MSBBOX_INIT_COMCTL32_FAILED,
};
void MsgBox( int type, HWND hWnd, const TCHAR *szTitle = NULL, const TCHAR *szText = NULL );