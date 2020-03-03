// RichPaint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "RichPaint.h"

Tool tools[ ] =
{
	{ID_PENCIL,			IDC_PENCIL,			IDB_PENCIL,			NULL,NULL,							NULL,CreatePen( PS_SOLID, 1, 0 ),	NULL,NULL,NULL},
	{ID_BRUSH,			IDC_BRUSH,			IDB_BRUSH,			NULL,NULL,							NULL,CreatePen( PS_SOLID, 10, 0 ),	NULL,NULL,NULL},
	{ID_AIRBRUSH,		IDC_AIRBRUSH,		IDB_AIRBRUSH,		NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},
	{ID_ERASER,			IDC_ERASER,			IDB_ERASER,			NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},

	{ID_TEXT,			IDC_TEXT,			IDB_TEXT,			NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},
	{ID_FILLWITHCOLOR,	IDC_FILLWITHCOLOR,	IDB_FILLWITHCOLOR,	NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},
	{ID_COLORPICKER,	IDC_COLORPICKER,	IDB_COLORPICKER,	NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},

	{ID_MAGNIFIER,		IDC_MAGNIFIER,		IDB_MAGNIFIER,		NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},
	{ID_ZOOMIN,			IDC_ZOOMIN,			IDB_ZOOMIN,			NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},
	{ID_ZOOMOUT,		IDC_ZOOMOUT,		IDB_ZOOMOUT,		NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},

	{ID_COPY,			IDC_COPY,			IDB_COPY,			NULL,LoadCursor( NULL,IDC_ARROW ),	NULL,NULL,							NULL,NULL,NULL},
	{ID_PASTE,			IDC_PASTE,			IDB_PASTE,			NULL,LoadCursor( NULL,IDC_ARROW ),	NULL,NULL,							NULL,NULL,NULL},
	{ID_CUT,			IDC_CUT,			IDB_CUT,			NULL,LoadCursor( NULL,IDC_ARROW ),	NULL,NULL,							NULL,NULL,NULL},
	{ID_LINE,			IDC_LINE,			IDB_LINE,			NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},
	{ID_CURVE,			IDC_CURVE,			IDB_CURVE,			NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},
	{ID_ROUNDEDRECT,	IDC_ROUNDEDRECT,	IDB_ROUNDEDRECT,	NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},
	{ID_RECT,			IDC_RECT,			IDB_RECT,			NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},
	{ID_OVAL,			IDC_OVAL,			IDB_OVAL,			NULL,NULL,							NULL,NULL,							NULL,NULL,NULL},
};

DWORD dwBasicColors[ ] = { 0,64,128,255 };
const DWORD cBasicColor = sizeof dwBasicColors / sizeof dwBasicColors[ 0 ];
HWND hWndBasicColor[ cBasicColor * cBasicColor * cBasicColor ];
const DWORD cTools = sizeof tools / sizeof tools[ 0 ];
HWND hWndEditColor;
HWND hWndCurColor;
HWND hWndUndo, hWndRedo;

DWORD dwCurToolIdx = 0;
DWORD cColor = cBasicColor * cBasicColor * cBasicColor;
BOOL bLButtonDown, bDrawing;
POINT ptMouse;
RECT canvasRect;
std::vector<HDC> hdcMemCanvasUndoStack;
std::vector<HDC> hdcMemCanvasRedoStack;
HDC hdcMemCanvas;
HBITMAP hBitmapCanvas;


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[ MAX_LOADSTRING ];                  // The title bar text
WCHAR szWindowClass[ MAX_LOADSTRING ];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass( HINSTANCE hInstance );
BOOL                InitInstance( HINSTANCE, int );
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK    About( HWND, UINT, WPARAM, LPARAM );

int APIENTRY wWinMain( _In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPWSTR    lpCmdLine,
					   _In_ int       nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
	LoadStringW( hInstance, IDC_RICHPAINT, szWindowClass, MAX_LOADSTRING );
	MyRegisterClass( hInstance );

	// Perform application initialization:
	if ( !InitInstance( hInstance, nCmdShow ) )
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_RICHPAINT ) );

	MSG msg;

	// Main message loop:
	while ( GetMessage( &msg, nullptr, 0, 0 ) )
	{
		if ( !TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	return ( int ) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof( WNDCLASSEX );

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_RICHPAINT ) );
	wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 2 );
	wcex.lpszMenuName = MAKEINTRESOURCEW( IDC_RICHPAINT );
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon( wcex.hInstance, MAKEINTRESOURCE( IDI_RICHPAINT ) );

	return RegisterClassExW( &wcex );
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW( szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
							   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr );

	if ( !hWnd )
	{
		return FALSE;
	}

	ShowWindow( hWnd, nCmdShow );
	UpdateWindow( hWnd );

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
// 0,32,64,128,255
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_CREATE:
	{
		return OnCreate( hWnd, message, wParam, lParam );
	}
	case WM_LBUTTONDOWN:
	{
		return OnLButtonDown( hWnd, message, wParam, lParam );
	}
	case WM_LBUTTONUP:
	{
		return OnLButtonUp( hWnd, message, wParam, lParam );
	}
	case WM_MOUSEMOVE:
	{
		return OnMouseMove( hWnd, message, wParam, lParam );
	}
	case WM_SIZE:
	{
		return OnSize( hWnd, message, wParam, lParam );
	}
	case WM_COMMAND:
	{
		return OnCommand( hWnd, message, wParam, lParam );
	}
	case WM_PAINT:
	{
		return OnPaint( hWnd, message, wParam, lParam );
	}
	case WM_DRAWITEM:
	{
		return OnDrawItem( hWnd, message, wParam, lParam );
	}
	case WM_DESTROY:
	{
		return OnDestroy( hWnd, message, wParam, lParam );
	}
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

LRESULT OnCreate( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	DWORD dwColor, dwId;
	DWORD i, x, y, z;
	for ( i = 0; i < cColor; ++i )
	{
		x = i % cBasicColor;
		y = ( i / cBasicColor ) % cBasicColor;
		z = ( i / cBasicColor / cBasicColor ) % cBasicColor;

		dwColor = RGB( dwBasicColors[ x ],
					   dwBasicColors[ y ],
					   dwBasicColors[ z ] );

		dwId = x * cBasicColor * cBasicColor + y * cBasicColor + z;

		hWndBasicColor[ dwId ] = CreateWindow( TEXT( "button" ), NULL,
											   WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
											   0, 0, 0, 0,
											   hWnd, ( HMENU ) ( COLORID + dwId ), hInst, NULL );
		SetWindowLongPtr( hWndBasicColor[ dwId ], GWLP_USERDATA, dwColor );
	}
	hWndCurColor = CreateWindow( TEXT( "button" ), NULL,
								 WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
								 0, 0, 0, 0,
								 hWnd, ( HMENU ) ( ID_CURCOLOR ), hInst, NULL );

	hWndEditColor = CreateWindow( TEXT( "button" ), NULL,
								  WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
								  0, 0, 0, 0,
								  hWnd, ( HMENU ) ( ID_EDITCOLOR ), hInst, NULL );
	SetWindowLongPtr( hWndBasicColor[ dwId ], GWLP_USERDATA, RGB( 0, 0, 0 ) );
	
	hWndUndo = CreateWindow( TEXT( "button" ), NULL,
							 WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
							 0, 0, 0, 0,
							 hWnd, ( HMENU ) ( ID_UNDO ), hInst, NULL );
	
	hWndRedo = CreateWindow( TEXT( "button" ), NULL,
							 WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
							 0, 0, 0, 0,
							 hWnd, ( HMENU ) ( ID_REDO ), hInst, NULL );

	for ( i = 0; i < cTools; ++i )
	{
		tools[ i ].hWnd = CreateWindow( TEXT( "button" ), NULL,
										WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
										0, 0, 0, 0,
										hWnd, ( HMENU ) tools[ i ].id, hInst, NULL );
		tools[ i ].hBitmap = LoadBitmap( hInst, MAKEINTRESOURCE( tools[ i ].idBitmap ) );

		if ( !tools[ i ].hCursor )
			tools[ i ].hCursor = LoadCursor( hInst, MAKEINTRESOURCE( tools[ i ].idCursor ) );
	}


	tools[ ID_ERASER - TOOLID ].hPen = CreateEraser( );
	tools[ ID_AIRBRUSH - TOOLID ].hPen = CreateAirbrush( LoadBitmap( hInst, 
																	 MAKEINTRESOURCE( IDB_AIRBRUSH_EFFECT ) ) );

	// global cursor
	dwCurToolIdx = 0;
	SET_GLOBAL_CURSOR( tools[ dwCurToolIdx ].hCursor );

	// canvas bitmap
	HDC hdc = GetDC( hWnd );
	hdcMemCanvas = CreateCompatibleDC( hdc );
	hBitmapCanvas = CreateCompatibleBitmap( hdc, GetSystemMetrics( SM_CXSCREEN ),
											GetSystemMetrics( SM_CYSCREEN ) );
	SelectObject( hdcMemCanvas, hBitmapCanvas );
	PatBlt( hdcMemCanvas, 0, 0,
			GetSystemMetrics( SM_CXSCREEN ),
			GetSystemMetrics( SM_CYSCREEN ),
			WHITENESS );
	hdcMemCanvasUndoStack.push_back( hdcMemCanvas );	// 放张白纸垫底
	hdcMemCanvas = CopyHdcBitmapMem( hdcMemCanvas );
	ReleaseDC( hWnd, hdc );

	return DefWindowProc( hWnd, message, wParam, lParam );
}

LRESULT OnLButtonDown( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	POINT pt;
	pt.x = ( short ) LOWORD( lParam );
	pt.y = ( short ) HIWORD( lParam );

	if ( !PtInRect( &canvasRect, pt ) ) return 0;

	bLButtonDown = TRUE;
	ptMouse = pt;
	SetCapture( hWnd );	// 忘记具体用途了
	return 0;
}

LRESULT OnLButtonUp( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( bLButtonDown )
	{
		// add one to undo stack
		bLButtonDown = FALSE;
		if ( bDrawing )
		{
			bDrawing = FALSE;
			hdcMemCanvasUndoStack.push_back( hdcMemCanvas );
			hdcMemCanvas = CopyHdcBitmapMem( hdcMemCanvas );

			// clear all redo
			if ( !hdcMemCanvasRedoStack.empty( ) )
			{
				for ( auto it : hdcMemCanvasRedoStack )
				{
					DeleteDC( it );
				}
				hdcMemCanvasRedoStack.clear( );
			}
		}
	}
	ReleaseCapture( );
	return 0;
}

LRESULT OnMouseMove( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	POINT pt;
	pt.x = ( short ) LOWORD( lParam );
	pt.y = ( short ) HIWORD( lParam );

	POINT ptMouseStart = ptMouse;
	POINT ptMouseEnd = pt;
	ptMouse = pt;
	
	if ( !PtInRect( &canvasRect, ptMouse ) || !bLButtonDown ) return 0;

	bDrawing = TRUE;

	HDC hdc = GetDC( hWnd );
	switch ( tools[ dwCurToolIdx ].idCursor )
	{
	case IDC_PENCIL:
		DealWithPencil( hdc, hdcMemCanvas, ptMouseStart, ptMouseEnd, tools[ dwCurToolIdx ] );
		break;
	case IDC_BRUSH:
		DealWithBrush( hdc, hdcMemCanvas, ptMouseStart, ptMouseEnd, tools[ dwCurToolIdx ] );
		break;
	case IDC_AIRBRUSH:
		DealWithAirbrush( hdc, hdcMemCanvas, ptMouseStart, ptMouseEnd, tools[ dwCurToolIdx ] );
		break;
	case IDC_ERASER:
		DealWithEraser( hdc, hdcMemCanvas, ptMouseStart, ptMouseEnd, tools[ dwCurToolIdx ] );
		break;
	case IDC_TEXT:
		DealWithText( hdc, hdcMemCanvas, ptMouseStart, ptMouseEnd, tools[ dwCurToolIdx ] );
		break;
	case IDC_FILLWITHCOLOR:
	case IDC_COLORPICKER:
	case IDC_MAGNIFIER:
	case IDC_ZOOMIN:
	case IDC_ZOOMOUT:
	case IDC_COPY:
	case IDC_PASTE:
	case IDC_CUT:
	case IDC_LINE:
	case IDC_CURVE:
	case IDC_ROUNDEDRECT:
	case IDC_RECT:
	case IDC_OVAL:
		break;
	}
	ReleaseDC( hWnd, hdc );
	return 0;
}

LRESULT OnSize( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	DWORD i, x, y;
	int cxClient = LOWORD( lParam );
	int cyClient = HIWORD( lParam );

	//============================================================================
	//
	// Color window
	//
	for ( i = 0; i < cColor; ++i )
	{
		x = i % COLNUM;
		y = i / COLNUM;
		MoveWindow( hWndBasicColor[ i ],
					10 + x * ( ICONSIZEX + 5 ),
					10 + y * ( ICONSIZEY + 5 ),
					ICONSIZEX, ICONSIZEY, TRUE );
	}

	x = i % COLNUM;
	y = i / COLNUM;
	MoveWindow( hWndEditColor,
				10 + x * ( ICONSIZEX + 5 ),
				10 + 20 + y * ( ICONSIZEY + 5 ),
				ICONSIZEX, ICONSIZEY, TRUE );
	++i;
	x = i % COLNUM;
	y = i / COLNUM;
	MoveWindow( hWndCurColor,
				10 + x * ( ICONSIZEX + 5 ),
				10 + 20 + y * ( ICONSIZEY + 5 ),
				ICONSIZEX, ICONSIZEY, TRUE );

	//============================================================================
	//
	// Tools window
	//
	int base = ( y + 2 ) * COLNUM;
	for ( i = base; i < base + cTools; ++i )
	{
		x = i % COLNUM;
		y = i / COLNUM;
		MoveWindow( tools[ i - base ].hWnd,
					10 + x * ( ICONSIZEX + 5 ),
					10 + y * ( ICONSIZEY + 5 ),
					ICONSIZEX, ICONSIZEY, TRUE );
	}

	//============================================================================
	//
	// Undo, Redo window
	//
	i = ( y + 1 ) * COLNUM;
	x = i % COLNUM;
	y = i / COLNUM;
	MoveWindow( hWndUndo,
				10 + x * ( ICONSIZEX + 5 ),
				10 + 20 + y * ( ICONSIZEY + 5 ),
				ICONSIZEX, ICONSIZEY, TRUE );
	++i;
	x = i % COLNUM;
	y = i / COLNUM;
	MoveWindow( hWndRedo,
				10 + x * ( ICONSIZEX + 5 ),
				10 + 20 + y * ( ICONSIZEY + 5 ),
				ICONSIZEX, ICONSIZEY, TRUE );
	

	// canvas rectangle
	canvasRect.left = ( ICONSIZEX + 5 ) * COLNUM + 20;
	canvasRect.right = cxClient - 10;
	canvasRect.top = 10;
	canvasRect.bottom = cyClient - 10;
	return 0;
}

LRESULT OnCommand( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int wmId = LOWORD( wParam );
	HMENU hMenu = GetMenu( hWnd );
	DWORD dwCurColor;
	LOGPEN lgPen;
	// Parse the menu selections:

	if ( IS_COLORID( wmId ) ) // color select
	{
		dwCurColor = GetWindowLongPtr( ( HWND ) lParam, GWLP_USERDATA );
		switch ( tools[ dwCurToolIdx ].idCursor )
		{
		case IDC_PENCIL:
		case IDC_BRUSH:
		case IDC_LINE:
		case IDC_CURVE:
		case IDC_ROUNDEDRECT:
		case IDC_RECT:
		case IDC_OVAL:
		{
			GetObject( tools[ dwCurToolIdx ].hPen, sizeof lgPen, &lgPen );
			DeleteObject( tools[ dwCurToolIdx ].hPen );
			lgPen.lopnColor = dwCurColor;
			tools[ dwCurToolIdx ].hPen = CreatePenIndirect( &lgPen );

			SetWindowLongPtr( hWndCurColor, GWLP_USERDATA, dwCurColor );
			InvalidateRect( hWndCurColor, NULL, TRUE );
			break;
		}
		case IDC_AIRBRUSH:
		case IDC_ERASER:
			break;
		case IDC_TEXT:
		case IDC_FILLWITHCOLOR:
			//case IDC_COLORPICKER:
		case IDC_MAGNIFIER:
			//case IDC_ZOOMIN:
			//case IDC_ZOOMOUT:
			//case IDC_COPY:
			//case IDC_PASTE:
			//case IDC_CUT:
		default:
			break;
		}
	}
	else if ( IS_TOOLID( wmId ) )
	{
		// set the cursor
		dwCurToolIdx = INDEX_TOOL( wmId );
		SET_GLOBAL_CURSOR( tools[ dwCurToolIdx ].hCursor );

		// set the current color
		LOGPEN lgPen;
		GetObject( tools[ dwCurToolIdx ].hPen, sizeof LOGPEN, &lgPen );
		SetWindowLongPtr( hWndCurColor, GWLP_USERDATA, lgPen.lopnColor );
		InvalidateRect( hWndCurColor, NULL, TRUE );

		// assign the attribute of this tool
		if ( wmId >= ID_TEXT )
			MsgBox( MSGBOX_UNFINISHED, hInst, hWnd );
	}
	else if ( wmId == ID_EDITCOLOR )
	{
		CHOOSECOLOR cc;
		COLORREF crCustColors[ 16 ];
		ZeroMemory( &cc, sizeof cc );
		cc.lStructSize = sizeof CHOOSECOLOR;
		cc.rgbResult = RGB( 0x80, 0x80, 0x80 );
		cc.lpCustColors = crCustColors;
		cc.Flags = CC_RGBINIT | CC_FULLOPEN;
		cc.hwndOwner = hWnd;

		if ( ChooseColor( &cc ) )
		{
			LOGPEN lgPen;
			GetObject( tools[ dwCurToolIdx ].hPen, sizeof lgPen, &lgPen );
			DeleteObject( tools[ dwCurToolIdx ].hPen );
			lgPen.lopnColor = cc.rgbResult;
			tools[ dwCurToolIdx ].hPen = CreatePenIndirect( &lgPen );

			SetWindowLongPtr( hWndCurColor, GWLP_USERDATA, lgPen.lopnColor );
			InvalidateRect( hWndCurColor, NULL, TRUE );
		}
	} else switch ( wmId )
	{
	case ID_UNDO:
	case ID_REDO:
		SendMessage( hWnd, WM_COMMAND, 
					 wmId == ID_UNDO? IDM_EDIT_UNDO: IDM_EDIT_REDO, 0 );
		return 0;
	case IDM_FILE_NEW:
	case IDM_FILE_OPEN:
	case IDM_FILE_SAVE:
	case IDM_FILE_SAVEAS:
	case IDM_FILE_PRINT:
		MsgBox( MSGBOX_UNFINISHED, hInst, hWnd );
		break;
	case IDM_FILE_EXIT:
		SendMessage( hWnd, WM_CLOSE, 0, 0 );
		break;

	case IDM_EDIT_UNDO:
		DeleteDC( hdcMemCanvas );
		hdcMemCanvas = MenuEditUndo( hdcMemCanvasUndoStack, hdcMemCanvasRedoStack );
		InvalidateRect( hWnd, NULL, FALSE );
		break;
	case IDM_EDIT_REDO:
		DeleteDC( hdcMemCanvas );
		hdcMemCanvas = MenuEditRedo( hdcMemCanvasRedoStack, hdcMemCanvasUndoStack );
		InvalidateRect( hWnd, NULL, FALSE );
		break;

	case IDM_HELP_ABOUT:
		DialogBox( hInst, MAKEINTRESOURCE( IDD_ABOUTBOX ), hWnd, About );
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}

LRESULT OnPaint( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint( hWnd, &ps );
	// TODO: Add any drawing code that uses hdc here...
	BitBlt( hdc, canvasRect.left, canvasRect.top,
			canvasRect.right - canvasRect.left,
			canvasRect.bottom - canvasRect.top,
			hdcMemCanvas, canvasRect.left, canvasRect.top, SRCCOPY );
	EndPaint( hWnd, &ps );
	ReleaseDC( hWnd, hdc );
	return DefWindowProc( hWnd, message, wParam, lParam );
}

LRESULT OnDrawItem( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	LPDRAWITEMSTRUCT pDIS = ( LPDRAWITEMSTRUCT ) lParam;

	int cx = pDIS->rcItem.right - pDIS->rcItem.left;
	int cy = pDIS->rcItem.bottom - pDIS->rcItem.top;
	RECT rcButton = pDIS->rcItem;
	rcButton.left += cx / 8;
	rcButton.right -= cx / 8;
	rcButton.top += cy / 8;
	rcButton.bottom -= cy / 8;

	FillRect( pDIS->hDC, &pDIS->rcItem, GetSysColorBrush( COLOR_BTNFACE ) );

	if ( IS_COLORID( pDIS->CtlID ) )
	{
		DWORD color = GetWindowLongPtr( pDIS->hwndItem, GWLP_USERDATA );
		FillRect( pDIS->hDC, &rcButton, CreateSolidBrush( color ) );
		DrawEdge( pDIS->hDC, &rcButton, EDGE_SUNKEN, BF_RECT );
	}
	else if ( IS_TOOLID( pDIS->CtlID ) )
	{
		HDC hdcMem = CreateCompatibleDC( pDIS->hDC );
		SelectObject( hdcMem, tools[ INDEX_TOOL( pDIS->CtlID ) ].hBitmap );
		BitBlt( pDIS->hDC, 4, 4, 32, 32, hdcMem, 0, 0, SRCCOPY );
		DeleteDC( hdcMem );
	}
	else switch (pDIS->CtlID )
	{
	case ID_EDITCOLOR:
	case ID_UNDO:
	case ID_REDO:
	{
		HDC hdcMem = CreateCompatibleDC( pDIS->hDC );
		SelectObject( hdcMem, LoadBitmap( hInst, MAKEINTRESOURCE( pDIS->CtlID ) ) );
		BitBlt( pDIS->hDC, 4, 4, 32, 32, hdcMem, 0, 0, SRCCOPY );
		DeleteDC( hdcMem );
		break;
	}
	default:
		break;
	}


	if ( pDIS->itemState & ODS_FOCUS )
	{
		pDIS->rcItem.left += cx / 16;
		pDIS->rcItem.right -= cx / 16;
		pDIS->rcItem.top += cy / 16;
		pDIS->rcItem.bottom -= cy / 16;
		DrawFocusRect( pDIS->hDC, &pDIS->rcItem );
	}
	return 0;
}

LRESULT OnDestroy( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PostQuitMessage( 0 );
	return DefWindowProc( hWnd, message, wParam, lParam );
}

// Message handler for about box.
INT_PTR CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( lParam );
	switch ( message )
	{
	case WM_INITDIALOG:
		return ( INT_PTR ) TRUE;

	case WM_COMMAND:
		if ( LOWORD( wParam ) == IDOK || LOWORD( wParam ) == IDCANCEL )
		{
			EndDialog( hDlg, LOWORD( wParam ) );
			return ( INT_PTR ) TRUE;
		}
		break;
	}
	return ( INT_PTR ) FALSE;
}
