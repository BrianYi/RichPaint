// RichPaint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "RichPaint.h"

Tool tools[ ] =
{
	{ID_PENCIL,			IDC_PENCIL,			IDB_PENCIL,			NULL,NULL,							NULL,CreatePen( PS_SOLID, 1, 0 ),	(HBRUSH)GetStockObject(BLACK_BRUSH),NULL,NULL},
	{ID_BRUSH,			IDC_BRUSH,			IDB_BRUSH,			NULL,NULL,							NULL,CreatePen( PS_SOLID, 10, 0 ),	( HBRUSH ) GetStockObject( BLACK_BRUSH ),NULL,NULL},
	{ID_AIRBRUSH,		IDC_AIRBRUSH,		IDB_AIRBRUSH,		NULL,NULL,							NULL,NULL,							( HBRUSH ) GetStockObject( BLACK_BRUSH ),NULL,NULL},
	{ID_ERASER,			IDC_ERASER,			IDB_ERASER,			NULL,NULL,							NULL,NULL,							( HBRUSH ) GetStockObject( WHITE_BRUSH ),NULL,NULL},

	{ID_TEXT,			IDC_TEXT,			IDB_TEXT,			NULL,NULL,							NULL,NULL,							( HBRUSH ) GetStockObject( WHITE_BRUSH ),NULL,NULL},
	{ID_FILLWITHCOLOR,	IDC_FILLWITHCOLOR,	IDB_FILLWITHCOLOR,	NULL,NULL,							NULL,NULL,							( HBRUSH ) GetStockObject( BLACK_BRUSH ),NULL,NULL},
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
HWND hWndTransparent;

DWORD dwCurToolIdx = 0;
DWORD cColor = cBasicColor * cBasicColor * cBasicColor;
BOOL bLButtonDown, bDrawing;
POINT ptMouse;
RECT canvasRect;
std::vector<HDC> hdcMemCanvasUndoStack;
std::vector<HDC> hdcMemCanvasRedoStack;
HDC hdcMemCanvas;
HBITMAP hBitmapCanvas;
HPEN g_hPen;


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[ MAX_LOADSTRING ];                  // The title bar text
WCHAR szWindowClass[ MAX_LOADSTRING ];            // the main window class name
WCHAR szTransparentClass[ MAX_LOADSTRING ];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass( HINSTANCE hInstance );
BOOL                InitInstance( HINSTANCE, int );
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK    About( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    TransparentWndProc( HWND, UINT, WPARAM, LPARAM );

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
	BOOL bSuccess = FALSE;

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
	bSuccess = RegisterClassExW( &wcex );

	lstrcpy( szTransparentClass, TEXT( "TRANSPARENT" ) );
	wcex.lpfnWndProc = TransparentWndProc;
	wcex.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
	wcex.lpszClassName = szTransparentClass;
	bSuccess = RegisterClassExW( &wcex );

	return TRUE;
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
											   WS_VISIBLE | WS_CHILD | BS_OWNERDRAW | WS_CLIPSIBLINGS,
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
	
	SetFocus( hWnd );

	if ( !PtInRect( &canvasRect, pt ) ) return 0;

	HDC hdc = GetDC( hWnd );
	DWORD dwCurColor = GetWindowLongPtr( hWndCurColor, GWLP_USERDATA );
	switch ( tools[ dwCurToolIdx ].id )
	{
	case ID_PENCIL:
		g_hPen = CreatePen( PS_SOLID, 1, dwCurColor );
		break;
	case ID_BRUSH:
		g_hPen = CreatePen( PS_SOLID, 10, dwCurColor );
		break;
	case ID_AIRBRUSH:
		g_hPen = CreateAirbrush( );
		break;
	case ID_ERASER:
		g_hPen = CreateEraser( );
		break;
	case ID_TEXT:
	{
		// TODO:
		//		I wannt to create a transparent child window, only border and text is visible
		//		but i always get fail when create a child window with attribute of WS_EX_LAYERED,
		//		it seems to try other method.
		//		The official said child window with WS_EX_LAYERED cannot only support win 8
		//	
		// 		hWndTransparent = CreateWindowEx( WS_EX_TRANSPARENT | WS_EX_LAYERED, szTransparentClass, szTitle, 
		// 										  WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		// 										  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst, nullptr );

		hWndTransparent = CreateWindowEx( WS_EX_LAYERED, szTransparentClass, NULL,
										  WS_OVERLAPPEDWINDOW | WS_CHILD,
										pt.x, pt.y, 200, 100,
										hWnd, ( HMENU ) ID_TRABSPARENT_WIN, hInst, NULL );
		
		break;
	}
	case ID_FILLWITHCOLOR:
	{
		SelectObject( hdc, tools[dwCurToolIdx].hBrush );
		SelectObject( hdcMemCanvas, tools[ dwCurToolIdx ].hBrush );
		ExtFloodFill( hdc, pt.x, pt.y,
					  GetWindowLongPtr( hWndCurColor, GWLP_USERDATA ), FLOODFILLBORDER );
		ExtFloodFill( hdcMemCanvas, pt.x, pt.y, 
					  GetWindowLongPtr(hWndCurColor, GWLP_USERDATA), FLOODFILLBORDER );
		break;
	}
	case ID_COLORPICKER:
		break;
	case ID_MAGNIFIER:
	case ID_ZOOMIN:
	case ID_ZOOMOUT:
	case ID_COPY:
	case ID_PASTE:
	case ID_CUT:
	case ID_LINE:
	case ID_CURVE:
	case ID_ROUNDEDRECT:
	case ID_RECT:
	case ID_OVAL:
		break;
	default:
		break;
	}
	ReleaseDC( hWnd, hdc );

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
			for ( auto it : hdcMemCanvasRedoStack )
			{
				DeleteDC( it );
			}
			hdcMemCanvasRedoStack.clear( );
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
	switch ( tools[ dwCurToolIdx ].id )
	{
	case ID_PENCIL:
		DealWithPencil( hdc, hdcMemCanvas, ptMouseStart, ptMouseEnd, g_hPen );
		break;
	case ID_BRUSH:
		DealWithBrush( hdc, hdcMemCanvas, ptMouseStart, ptMouseEnd, g_hPen );
		break;
	case ID_AIRBRUSH:
		DealWithAirbrush( hdc, hdcMemCanvas, ptMouseStart, ptMouseEnd, g_hPen );
		break;
	case ID_ERASER:
		DealWithEraser( hdc, hdcMemCanvas, ptMouseStart, ptMouseEnd, g_hPen );
		break;
	case ID_TEXT:
		DealWithText( hdc, hdcMemCanvas, ptMouseStart, ptMouseEnd, g_hPen );
		break;
	case ID_FILLWITHCOLOR:
	case ID_COLORPICKER:
	case ID_MAGNIFIER:
	case ID_ZOOMIN:
	case ID_ZOOMOUT:
	case ID_COPY:
	case ID_PASTE:
	case ID_CUT:
	case ID_LINE:
	case ID_CURVE:
	case ID_ROUNDEDRECT:
	case ID_RECT:
	case ID_OVAL:
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
	DWORD dwIconWidth = ICONSIZEW + 8;
	DWORD dwIconHeight = ICONSIZEH + 8;
	for ( i = 0; i < cColor; ++i )
	{
		x = i % COLNUM;
		y = i / COLNUM;
		MoveWindow( hWndBasicColor[ i ],
					10 + x * ( dwIconWidth + 5 ),
					10 + y * ( dwIconHeight + 5 ),
					dwIconWidth, dwIconHeight, TRUE );
	}

	x = i % COLNUM;
	y = i / COLNUM;
	MoveWindow( hWndEditColor,
				10 + x * ( dwIconWidth + 5 ),
				10 + 20 + y * ( dwIconHeight + 5 ),
				dwIconWidth, dwIconHeight, TRUE );
	++i;
	x = i % COLNUM;
	y = i / COLNUM;
	MoveWindow( hWndCurColor,
				10 + x * ( dwIconWidth + 5 ),
				10 + 20 + y * ( dwIconHeight + 5 ),
				dwIconWidth, dwIconHeight, TRUE );

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
					10 + x * ( dwIconWidth + 5 ),
					10 + y * ( dwIconHeight + 5 ),
					dwIconWidth, dwIconHeight, TRUE );
	}

	//============================================================================
	//
	// Undo, Redo window
	//
	i = ( y + 1 ) * COLNUM;
	x = i % COLNUM;
	y = i / COLNUM;
	MoveWindow( hWndUndo,
				10 + x * ( dwIconWidth + 5 ),
				10 + 20 + y * ( dwIconHeight + 5 ),
				dwIconWidth, dwIconHeight, TRUE );
	++i;
	x = i % COLNUM;
	y = i / COLNUM;
	MoveWindow( hWndRedo,
				10 + x * ( dwIconWidth + 5 ),
				10 + 20 + y * ( dwIconHeight + 5 ),
				dwIconWidth, dwIconHeight, TRUE );

	//============================================================================
	//
	// Transparent window
	//
	//MoveWindow(hWndTransparent, 0, 0, )

	// canvas rectangle
	canvasRect.left = ( dwIconWidth + 5 ) * COLNUM + 20;
	canvasRect.right = cxClient - 10;
	canvasRect.top = 10;
	canvasRect.bottom = cyClient - 10;
	return 0;
}

LRESULT OnCommand( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int wmId = LOWORD( wParam );
	HMENU hMenu = GetMenu( hWnd );
	// Parse the menu selections:
	
	if ( IS_COLORID( wmId ) ) // color select
	{
		SetWindowLongPtr( hWndCurColor, GWLP_USERDATA, 
						  GetWindowLongPtr( ( HWND ) lParam, GWLP_USERDATA ) );
		InvalidateRect( hWndCurColor, NULL, TRUE );
	}
	else if ( IS_TOOLID( wmId ) )
	{
		// set the cursor
		dwCurToolIdx = INDEX_TOOL( wmId );
		SET_GLOBAL_CURSOR( tools[ dwCurToolIdx ].hCursor );
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
			SetWindowLongPtr( hWndCurColor, GWLP_USERDATA, cc.rgbResult );
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
		DeleteDC( hdcMemCanvas );
		while (hdcMemCanvasUndoStack.size()>1)
		{
			DeleteDC( hdcMemCanvasUndoStack.back( ) );
			hdcMemCanvasUndoStack.pop_back( );
		}
		hdcMemCanvas = CopyHdcBitmapMem( hdcMemCanvasUndoStack.back( ) );
		while ( !hdcMemCanvasRedoStack.empty( ) )
		{
			DeleteDC( hdcMemCanvasRedoStack.back( ) );
			hdcMemCanvasRedoStack.pop_back( );
		}
		InvalidateRect( hWnd, NULL, FALSE );
		return 0;
	case IDM_FILE_OPEN:
	case IDM_FILE_SAVE:
	case IDM_FILE_SAVEAS:
	case IDM_FILE_PRINT:
		MsgBox( MSGBOX_UNFINISHED, hWnd );
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

	// draw background color
	FillRect( pDIS->hDC, &pDIS->rcItem, GetSysColorBrush( COLOR_BTNFACE ) );
	
	// draw focus
	if ( pDIS->itemState & ODS_FOCUS )
	{
		FillRect( pDIS->hDC, &pDIS->rcItem, GetSysColorBrush( COLOR_BTNSHADOW ) );
	}

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
		BitBlt( pDIS->hDC, (cx - ICONSIZEW) / 2, (cy - ICONSIZEH) / 2, 
				ICONSIZEW, ICONSIZEH, hdcMem, 0, 0, SRCCOPY );
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
		BitBlt( pDIS->hDC, ( cx - ICONSIZEW ) / 2, ( cy - ICONSIZEH ) / 2, 
				ICONSIZEW, ICONSIZEH, hdcMem, 0, 0, SRCCOPY );
		DeleteDC( hdcMem );
		break;
	}
	default:
		break;
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

LRESULT CALLBACK TransparentWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL bLButtonDown = FALSE;
	static POINT ptMouse;
	switch ( message )
	{
	case WM_LBUTTONDOWN:
	{
		bLButtonDown = TRUE;
		SetCapture( hWnd );
		return 0;
	}
	case WM_LBUTTONUP:
	{
		if ( bLButtonDown )
		{
			bLButtonDown = FALSE;
		}
		ReleaseCapture( );
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		POINT pt;
		pt.x = LOWORD( lParam );
		pt.y = HIWORD( lParam );

// 		if ( bLButtonDown )
// 		{
// 			SetWindowPos( hWnd, NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE );
// 		}
		return 0;
	}
	case WM_DESTROY:
		break;
	default:
		break;
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}