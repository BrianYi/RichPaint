#include "stdafx.h"
#include "Header.h"
#include "RichPaint.h"
#include <windowsx.h>

Tool tools[ ] =
{
	{ID_PENCIL,			IDC_PENCIL,			IDB_PENCIL,			NULL,NULL,							NULL,( HBRUSH ) GetStockObject( BLACK_BRUSH ),NULL,NULL},
	{ID_BRUSH,			IDC_BRUSH,			IDB_BRUSH,			NULL,NULL,							NULL,( HBRUSH ) GetStockObject( BLACK_BRUSH ),NULL,NULL},
	{ID_AIRBRUSH,		IDC_AIRBRUSH,		IDB_AIRBRUSH,		NULL,NULL,							NULL,( HBRUSH ) GetStockObject( BLACK_BRUSH ),NULL,NULL},
	{ID_ERASER,			IDC_ERASER,			IDB_ERASER,			NULL,NULL,							NULL,( HBRUSH ) GetStockObject( WHITE_BRUSH ),NULL,NULL},

	{ID_TEXT,			IDC_TEXT,			IDB_TEXT,			NULL,NULL,							NULL,( HBRUSH ) GetStockObject( WHITE_BRUSH ),NULL,NULL},
	{ID_FILLWITHCOLOR,	IDC_FILLWITHCOLOR,	IDB_FILLWITHCOLOR,	NULL,NULL,							NULL,( HBRUSH ) GetStockObject( BLACK_BRUSH ),NULL,NULL},
	{ID_COLORPICKER,	IDC_COLORPICKER,	IDB_COLORPICKER,	NULL,NULL,							NULL,NULL,NULL,NULL},

	{ID_MAGNIFIER,		IDC_MAGNIFIER,		IDB_MAGNIFIER,		NULL,NULL,							NULL,NULL,NULL,NULL},
	{ID_ZOOMIN,			IDC_ZOOMIN,			IDB_ZOOMIN,			NULL,NULL,							NULL,NULL,NULL,NULL},
	{ID_ZOOMOUT,		IDC_ZOOMOUT,		IDB_ZOOMOUT,		NULL,NULL,							NULL,NULL,NULL,NULL},

	{ID_COPY,			IDC_COPY,			IDB_COPY,			NULL,LoadCursor( NULL,IDC_ARROW ),	NULL,NULL,NULL,NULL},
	{ID_PASTE,			IDC_PASTE,			IDB_PASTE,			NULL,LoadCursor( NULL,IDC_ARROW ),	NULL,NULL,NULL,NULL},
	{ID_CUT,			IDC_CUT,			IDB_CUT,			NULL,LoadCursor( NULL,IDC_ARROW ),	NULL,NULL,NULL,NULL},
	{ID_LINE,			IDC_LINE,			IDB_LINE,			NULL,NULL,							NULL,NULL,NULL,NULL},
	{ID_CURVE,			IDC_CURVE,			IDB_CURVE,			NULL,NULL,							NULL,NULL,NULL,NULL},
	{ID_ROUNDEDRECT,	IDC_ROUNDEDRECT,	IDB_ROUNDEDRECT,	NULL,NULL,							NULL,NULL,NULL,NULL},
	{ID_RECT,			IDC_RECT,			IDB_RECT,			NULL,NULL,							NULL,NULL,NULL,NULL},
	{ID_OVAL,			IDC_OVAL,			IDB_OVAL,			NULL,NULL,							NULL,NULL,NULL,NULL},
};

OPENFILENAME ofn;
TCHAR szFilter[ ] = TEXT( "Bitmap Files (*.bmp)\0*.bmp\0\0" );
TCHAR szFileTitle[ MAX_PATH ];
TCHAR szFileName[ MAX_PATH ];

DWORD dwBasicColors[ ] = { 0,64,128,255 };
const DWORD cBasicColor = sizeof dwBasicColors / sizeof dwBasicColors[ 0 ];
HWND hWndBasicColor[ cBasicColor * cBasicColor * cBasicColor ];
const DWORD cTools = sizeof tools / sizeof tools[ 0 ];

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

extern WCHAR szTransparentClass[ MAX_LOADSTRING ];            // the main window class name
extern HINSTANCE hInst;

BOOL DlgOnInitAnimation( HWND hWnd, HWND hWndFocus, LPARAM lParam )
{
	// Load and register animation control class.
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof( INITCOMMONCONTROLSEX );
	iccx.dwICC = ICC_ANIMATE_CLASS;
	if ( !InitCommonControlsEx( &iccx ) )
	{
		MsgBox( MSBBOX_INIT_COMCTL32_FAILED, hWnd,
				TEXT( "Initialization" ), TEXT( "Init ICC_ANIMATE_CLASS failed!" ) );
	}

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

BOOL OnCreate( HWND hWnd, LPCREATESTRUCT lpCreateStruct )
{
	// Initialize common dlg
	DealInitCommonDlg( hWnd );

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


	HWND hWndCurColor = CreateWindow( TEXT( "button" ), NULL,
									  WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
									  0, 0, 0, 0,
									  hWnd, ( HMENU ) ( ID_CURCOLOR ), hInst, NULL );

	HWND hWndEditColor = CreateWindow( TEXT( "button" ), NULL,
									   WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
									   0, 0, 0, 0,
									   hWnd, ( HMENU ) ( ID_EDITCOLOR ), hInst, NULL );

	HWND hWndUndo = CreateWindow( TEXT( "button" ), NULL,
								  WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
								  0, 0, 0, 0,
								  hWnd, ( HMENU ) ( ID_UNDO ), hInst, NULL );

	HWND hWndRedo = CreateWindow( TEXT( "button" ), NULL,
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

	return TRUE;
}

BOOL OnInitMenu( HWND hwnd, HMENU hMenu )
{
	// TODO...

	return TRUE;
}

BOOL OnLButtonDown( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags )
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	SetFocus( hWnd );

	if ( !PtInRect( &canvasRect, pt ) )
		return TRUE;

	HDC hdc = GetDC( hWnd );
	DWORD dwCurColor = GetWindowLongPtr( GetDlgItem( hWnd, ID_CURCOLOR ),
										 GWLP_USERDATA );
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

		HWND hWndTransparent = GetDlgItem( hWnd, ID_TRABSPARENT_WIN );
		if ( hWndTransparent )
			DestroyWindow( hWndTransparent );
		hWndTransparent = CreateWindow( szTransparentClass, NULL,
										WS_SIZEBOX | WS_CHILD | WS_VISIBLE,
										pt.x, pt.y, 200, 100,
										hWnd, ( HMENU ) ID_TRABSPARENT_WIN, hInst, NULL );

		SetFocus( hWndTransparent );
		break;
	}
	case ID_FILLWITHCOLOR:
	{
		SelectObject( hdc, tools[ dwCurToolIdx ].hBrush );
		SelectObject( hdcMemCanvas, tools[ dwCurToolIdx ].hBrush );
		ExtFloodFill( hdc, pt.x, pt.y,
					  GetWindowLongPtr( GetDlgItem( hWnd, ID_CURCOLOR ), GWLP_USERDATA ),
					  FLOODFILLBORDER );
		ExtFloodFill( hdcMemCanvas, pt.x, pt.y,
					  GetWindowLongPtr( GetDlgItem( hWnd, ID_CURCOLOR ), GWLP_USERDATA ),
					  FLOODFILLBORDER );
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
	return TRUE;
}

BOOL OnLButtonUp( HWND hWnd, int x, int y, UINT keyFlags )
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
	return TRUE;
}

BOOL OnMouseMove( HWND hWnd, int x, int y, UINT keyFlags )
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	POINT ptMouseStart = ptMouse;
	POINT ptMouseEnd = pt;
	ptMouse = pt;
	HDC hdc;

#ifdef DEBUG
	hdc = GetDC( hWnd );
	DebugShowPosition( hdc, hdcMemCanvas, 0, 0, pt );
	ReleaseDC( hWnd, hdc );
#endif

	if ( !PtInRect( &canvasRect, ptMouse ) || !bLButtonDown ) return 0;

	bDrawing = TRUE;

	hdc = GetDC( hWnd );
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
	return TRUE;
}

BOOL OnSize( HWND hWnd, UINT state, int cx, int cy )
{
	DWORD i, x, y;
	int cxClient = cx;
	int cyClient = cy;

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
	MoveWindow( GetDlgItem( hWnd, ID_EDITCOLOR ),
				10 + x * ( dwIconWidth + 5 ),
				10 + 20 + y * ( dwIconHeight + 5 ),
				dwIconWidth, dwIconHeight, TRUE );
	++i;
	x = i % COLNUM;
	y = i / COLNUM;
	MoveWindow( GetDlgItem( hWnd, ID_CURCOLOR ),
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
	MoveWindow( GetDlgItem( hWnd, ID_UNDO ),
				10 + x * ( dwIconWidth + 5 ),
				10 + 20 + y * ( dwIconHeight + 5 ),
				dwIconWidth, dwIconHeight, TRUE );
	++i;
	x = i % COLNUM;
	y = i / COLNUM;
	MoveWindow( GetDlgItem( hWnd, ID_REDO ),
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
	return TRUE;
}

BOOL OnCommand( HWND hWnd, int id, HWND hwndCtl, UINT codeNotify )
{
	int wmId = id;
	HMENU hMenu = GetMenu( hWnd );
	// Parse the menu selections:

	if ( IS_COLORID( wmId ) ) // color select
	{
		SetWindowLongPtr( GetDlgItem( hWnd, ID_CURCOLOR ), GWLP_USERDATA,
						  GetWindowLongPtr( hwndCtl, GWLP_USERDATA ) );
		InvalidateRect( GetDlgItem( hWnd, ID_CURCOLOR ), NULL, TRUE );
	}
	else if ( IS_TOOLID( wmId ) )
	{
		// set the cursor
		dwCurToolIdx = INDEX_TOOL( wmId );
		SET_GLOBAL_CURSOR( tools[ dwCurToolIdx ].hCursor );
		switch ( wmId )
		{
		case ID_PASTE:
		{
			int ret = DialogBox( hInst, MAKEINTRESOURCE( IDD_DLGANIM ), hWnd,
								 AnimationDlgProc );
			if ( ret == IDOK ||
				 ret == IDCANCEL )
			{
				// TODO...
				break;
			}
			break;
		}
		default:
			break;
		}
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
			SetWindowLongPtr( GetDlgItem( hWnd, ID_CURCOLOR ),
							  GWLP_USERDATA, cc.rgbResult );
			InvalidateRect( GetDlgItem( hWnd, ID_CURCOLOR ),
							NULL, TRUE );
		}
	}
	else switch ( wmId )
	{
	case ID_UNDO:
	case ID_REDO:
		SendMessage( hWnd, WM_COMMAND,
					 wmId == ID_UNDO ? IDM_EDIT_UNDO : IDM_EDIT_REDO, 0 );
		return TRUE;
	case IDM_FILE_NEW:
		DeleteDC( hdcMemCanvas );
		DealClearUndoRedoStack( hdcMemCanvasUndoStack, hdcMemCanvasRedoStack );
		hdcMemCanvas = CopyHdcBitmapMem( hdcMemCanvasUndoStack.back( ) );
		InvalidateRect( hWnd, NULL, FALSE );
		return TRUE;
	case IDM_FILE_OPEN:
	{
		if ( GetOpenFileName( &ofn ) )
		{
			HBITMAP hBitmap = CreateDIBSectionFromDIBFile( szFileName );

			if ( hBitmap != NULL )
			{
				HDC hdcMem = CreateCompatibleDC( NULL );
				BITMAP bm;
				SelectObject( hdcMem, hBitmap );
				GetObject( hBitmap, sizeof BITMAP, ( LPVOID ) &bm );
				BitBlt( hdcMemCanvas, canvasRect.left, canvasRect.top,
						bm.bmWidth, bm.bmHeight,
						hdcMem, 0, 0, SRCCOPY );

				DealClearUndoRedoStack( hdcMemCanvasUndoStack, hdcMemCanvasRedoStack );

				InvalidateRect( hWnd, NULL, FALSE );
				DeleteDC( hdcMem );
			}
		}
		return TRUE;
	}
	case IDM_FILE_SAVE:
	{
		if ( GetSaveFileName( &ofn ) )
		{
			TCHAR szBuffer[ MAX_PATH ];
			int cx = canvasRect.right - canvasRect.left;
			int cy = canvasRect.bottom - canvasRect.top;
			HDC hdc = GetDC( hWnd );
			HDC hdcMem = CreateCompatibleDC( hdc );
			HBITMAP hBitmap = CreateCompatibleBitmap( hdc, cx, cy );
			SelectObject( hdcMem, hBitmap );
			BitBlt( hdcMem, 0, 0, cx, cy,
					hdcMemCanvas, canvasRect.left, canvasRect.top, SRCCOPY );
			if ( SaveDIBtoFile( hdcMem, hBitmap, ofn.lpstrFile ) )
			{
				wsprintf( szBuffer, TEXT( "File %s saved successfully." ), ofn.lpstrFileTitle );
				MsgBox( MSGBOX_FILE_SAVE_SUCCESS, hWnd, TEXT( "Save File" ), szBuffer );
			}
			else
			{
				wsprintf( szBuffer, TEXT( "Unable save the file %s." ), ofn.lpstrFileTitle );
				MsgBox( MSGBOX_FILE_SAVE_FAILED, hWnd, TEXT( "Save File" ), szBuffer );
			}

			DeleteDC( hdcMem );
			ReleaseDC( hWnd, hdc );
		}
		return TRUE;
	}
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
		return TRUE;
	}
	return TRUE;
}

BOOL OnPaint( HWND hWnd )
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
	return TRUE;
}

BOOL OnDrawItem( HWND hWnd, const DRAWITEMSTRUCT * lpDrawItem )
{
	const DRAWITEMSTRUCT *pDIS = lpDrawItem;

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
		BitBlt( pDIS->hDC, ( cx - ICONSIZEW ) / 2, ( cy - ICONSIZEH ) / 2,
				ICONSIZEW, ICONSIZEH, hdcMem, 0, 0, SRCCOPY );
		DeleteDC( hdcMem );
	}
	else switch ( pDIS->CtlID )
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

	return TRUE;
}

BOOL OnDestroy( HWND hwnd )
{
	PostQuitMessage( 0 );
	return TRUE;
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


INT_PTR CALLBACK AnimationDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
		HANDLE_MSG( hWnd, WM_INITDIALOG, DlgOnInitAnimation );
		HANDLE_MSG( hWnd, WM_COMMAND, DlgOnCommand );
		HANDLE_MSG( hWnd, WM_CLOSE, DlgOnClose );
	default:
		break;
	}
	return FALSE;
}

#define WM_LINECHANGE	(WM_USER+1)
#define CARET_X(iChar)	(iChar % cxBuffer)
#define CARET_Y(iChar)	(iChar / cxBuffer)
#define MAXBUFFSIZE		(cxMaxBuffer * cyMaxBuffer)

UINT_PTR CALLBACK Lpcfhookproc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD( wParam );
		if ( wmId == 0x402 )	// apply
		{
			LOGFONT logFont;
			SendMessage( hDlg, WM_CHOOSEFONT_GETLOGFONT, 0, ( LPARAM ) &logFont );
			HWND hWndTransparent = GetDlgItem( GetParent( hDlg ), ID_TRABSPARENT_WIN );
			SendMessage( hWndTransparent, WM_SETFONT,
				( WPARAM ) CreateFontIndirect( &logFont ), 0 );
			InvalidateRect( hWndTransparent, NULL, TRUE );
			return TRUE;
		}
		break;
	}
	default:
		break;
	}
	return FALSE;
}

LRESULT CALLBACK TransparentWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static BOOL bLButtonDown = FALSE;
	static int cxChar, cyChar;
	static int cxBuffer, cyBuffer;
	static int cxMaxBuffer, cyMaxBuffer;
	static int iNumChar, iChar;
	static TCHAR *pBuffer;
	static DWORD dwCharSet = DEFAULT_CHARSET;
	static HFONT hFont;
	static HDC hdcMem;
	static BOOL bSelectingFont = FALSE;
	PAINTSTRUCT ps;
	TEXTMETRIC tm;
	int i, y;
	switch ( message )
	{
	case WM_LINECHANGE:
	{
		if ( lParam == 0 ) return 0;

		cyBuffer += lParam;

		RECT rc;
		GetWindowRect( hWnd, &rc );
		ScreenToClient( GetParent( hWnd ), ( LPPOINT ) &rc );
		ScreenToClient( GetParent( hWnd ), ( ( LPPOINT ) &rc ) + 1 );
		MoveWindow( hWnd, rc.left, rc.top,
					rc.right - rc.left,
					rc.bottom - rc.top + cyChar * lParam, TRUE );
		return 0;
	}
	case WM_INPUTLANGCHANGE:
		dwCharSet = wParam;
		break;
	case WM_CREATE:
	{
		bSelectingFont = bLButtonDown = FALSE;
		cxBuffer = cyBuffer = 0;
		dwCharSet = DEFAULT_CHARSET;

		HDC hdc = GetDC( hWnd );
		hdcMem = CreateCompatibleDC( hdc );
		HBITMAP hBitmap = CreateCompatibleBitmap( hdc, GetSystemMetrics( SM_CXSCREEN ),
												  GetSystemMetrics( SM_CYSCREEN ) );

		SelectObject( hdcMem, hBitmap );

		hFont = ( HFONT ) GetStockObject( SYSTEM_FIXED_FONT );
		SelectObject( hdc, hFont );
		SelectObject( hdcMem, hFont );

		GetTextMetrics( hdc, &tm );
		ReleaseDC( hWnd, hdc );

		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight;

		iChar = iNumChar = 0;

		cxMaxBuffer = GetSystemMetrics( SM_CXSCREEN ) / cxChar;
		cyMaxBuffer = GetSystemMetrics( SM_CYSCREEN ) / cyChar;

		if ( pBuffer )
			free( pBuffer );
		pBuffer = ( TCHAR * ) malloc( cxMaxBuffer * cyMaxBuffer * sizeof TCHAR );
		_tcsnset( pBuffer, TEXT( ' ' ), ( size_t ) ( cxMaxBuffer * cyMaxBuffer ) );
		break;
	}
	case WM_SETFONT:
	{
		LOGFONT logFont;
		GetObject( ( HFONT ) wParam, sizeof LOGFONT, &logFont );
		hFont = CreateFontIndirect( &logFont );
		HDC hdc = GetDC( hWnd );
		GetTextMetrics( hdc, &tm );
		cyChar = abs( logFont.lfHeight );
		cxChar = tm.tmAveCharWidth * ( cyChar / tm.tmHeight );
		RECT rcWindow;
		int cxClient = cxChar * cxBuffer;
		int cyClient = cyChar * cyBuffer;

		HWND hwndParent = GetParent( hWnd );
		GetWindowRect( hWnd, &rcWindow );
		ScreenToClient( hwndParent, ( LPPOINT ) &rcWindow );
		ScreenToClient( hwndParent, ( ( LPPOINT ) &rcWindow ) + 1 );
		MoveWindow( hWnd, rcWindow.left, rcWindow.top,
					cxClient + GetSystemMetrics( SM_CXBORDER ) * 2,
					cyClient + GetSystemMetrics( SM_CYCAPTION ), TRUE );
		ReleaseDC( hWnd, hdc );
		return 0;
	}
	case WM_SIZE:
	{
		cxBuffer = max( 1, ( short ) LOWORD( lParam ) / cxChar );
		cyBuffer = max( 1, ( short ) HIWORD( lParam ) / cyChar );

		if ( hWnd == GetFocus( ) )
			SetCaretPos( CARET_X( iChar ) * cxChar,
						 CARET_Y( iChar ) * cyChar );

		InvalidateRect( hWnd, NULL, TRUE );
		break;
	}
	case WM_CONTEXTMENU:
	{
		POINT pt;
		pt.x = ( short ) LOWORD( lParam );
		pt.y = ( short ) HIWORD( lParam );

		HMENU hMenu = CreatePopupMenu( );
		AppendMenu( hMenu, MF_STRING, IDM_TRANS_CUT, TEXT( "Cut" ) );
		AppendMenu( hMenu, MF_STRING, IDM_TRANS_COPY, TEXT( "Copy" ) );
		AppendMenu( hMenu, MF_STRING, IDM_TRANS_PASTE, TEXT( "Paste" ) );
		AppendMenu( hMenu, MF_STRING, IDM_TRANS_DEL, TEXT( "Clear" ) );
		AppendMenu( hMenu, MF_STRING, IDM_TRANS_SELALL, TEXT( "Select All" ) );
		AppendMenu( hMenu, MF_SEPARATOR, 0, 0 );
		AppendMenu( hMenu, MF_STRING, IDM_TRANS_FONT, TEXT( "Font" ) );
		AppendMenu( hMenu, MF_SEPARATOR, 0, 0 );
		AppendMenu( hMenu, MF_STRING, IDM_TRANS_FINDREPLACE, TEXT( "Find or Replace" ) );
		AppendMenu( hMenu, MF_SEPARATOR, 0, 0 );
		AppendMenu( hMenu, MF_STRING, IDM_TRANS_UNDO, TEXT( "Undo" ) );
		AppendMenu( hMenu, MF_STRING, IDM_TRANS_REDO, TEXT( "Redo" ) );


		EnableMenuItem( hMenu, IDM_TRANS_CUT, MF_DISABLED );
		EnableMenuItem( hMenu, IDM_TRANS_COPY, MF_DISABLED );
		EnableMenuItem( hMenu, IDM_TRANS_PASTE,
						IsClipboardFormatAvailable( CF_UNICODETEXT ) ?
						MF_ENABLED : MF_DISABLED );

		EnableMenuItem( hMenu, IDM_TRANS_DEL, MF_ENABLED );
		EnableMenuItem( hMenu, IDM_TRANS_SELALL, MF_DISABLED );
		EnableMenuItem( hMenu, IDM_TRANS_FONT, MF_ENABLED );
		EnableMenuItem( hMenu, IDM_TRANS_FINDREPLACE, MF_DISABLED );
		EnableMenuItem( hMenu, IDM_TRANS_UNDO, MF_DISABLED );
		EnableMenuItem( hMenu, IDM_TRANS_REDO, MF_DISABLED );

		TrackPopupMenu( hMenu, TPM_RIGHTBUTTON, pt.x, pt.y,
						0, hWnd, NULL );
		DestroyMenu( hMenu );

		return 0;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD( wParam );
		switch ( wmId )
		{
		case IDM_TRANS_CUT:
		case IDM_TRANS_COPY:
		case IDM_TRANS_PASTE:
		{
			if ( IsClipboardFormatAvailable( CF_UNICODETEXT ) )
			{
				OpenClipboard( hWnd );
				HGLOBAL hGlobal = GetClipboardData( CF_UNICODETEXT );
				TCHAR *pText = ( TCHAR * ) malloc( GlobalSize( hGlobal ) * sizeof TCHAR );
				TCHAR *pGlobal = ( TCHAR * ) GlobalLock( hGlobal );
				lstrcpy( pText, pGlobal );
				GlobalUnlock( hGlobal );
				CloseClipboard( );
				DWORD iLength = lstrlen( pText );
				for ( i = 0; i < iLength; ++i )
					SendMessage( hWnd, WM_CHAR, pText[ i ], 1 );
				free( pText );
			}
			break;
		}
		case IDM_TRANS_DEL:
		{
			_tcsnset( pBuffer, TEXT( ' ' ), ( size_t ) ( cxMaxBuffer * cyMaxBuffer ) );
			iNumChar = iChar = 0;
			SetCaretPos( CARET_X( iChar ), CARET_Y( iChar ) );
			InvalidateRect( hWnd, NULL, FALSE );
			break;
		}
		case IDM_TRANS_SELALL:
			break;
		case IDM_TRANS_FONT:
		{
			CHOOSEFONT cf;
			LOGFONT logFont;
			GetObject( hFont, sizeof( LOGFONT ), &logFont );
			ZeroMemory( &cf, sizeof CHOOSEFONT );
			cf.lStructSize = sizeof CHOOSEFONT;
			cf.hwndOwner = hWnd;
			cf.lpLogFont = &logFont;
			cf.Flags = CF_EFFECTS | CF_APPLY |
				CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_ENABLEHOOK;
			cf.lpfnHook = Lpcfhookproc;
			bSelectingFont = TRUE;
			if ( ChooseFont( &cf ) )
			{
				SendMessage( hWnd, WM_SETFONT, ( WPARAM ) CreateFontIndirect( &logFont ), 0 );
			}
			SetFocus( hWnd );
			bSelectingFont = FALSE;
			return 0;
		}
		case IDM_TRANS_FINDREPLACE:
		case IDM_TRANS_UNDO:
		case IDM_TRANS_REDO:
			break;
		default:
			MsgBox( MSGBOX_UNFINISHED, hWnd );
			break;
		}
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		// TODO...
		POINT pt;
		pt.x = ( short ) LOWORD( lParam );
		pt.y = ( short ) HIWORD( lParam );

		SetFocus( hWnd );

		bLButtonDown = TRUE;
		SetCapture( hWnd );

		return 0;
	}
	case WM_LBUTTONUP:
	{
		// TODO...
		if ( bLButtonDown )
		{
			bLButtonDown = FALSE;
		}
		ReleaseCapture( );
		return 0;
	}
	case WM_SETFOCUS:
	{
		CreateCaret( hWnd, NULL, cxChar, cyChar );
		SetCaretPos( CARET_X( iChar ) * cxChar, CARET_Y( iChar ) * cyChar );
		ShowCaret( hWnd );
		return 0;
	}
	case WM_KILLFOCUS:
	{
		HideCaret( hWnd );
		DestroyCaret( );
		if ( !bSelectingFont )
			SendMessage( hWnd, WM_CLOSE, 0, 0 );
		return 0;
	}
	case WM_PAINT:
	{
		HDC hdc = BeginPaint( hWnd, &ps );
		RECT rcClient;
		GetClientRect( hWnd, &rcClient );
		int cx = rcClient.right - rcClient.left;
		int cy = rcClient.bottom - rcClient.top;

		// copy background to bitmap
		POINT pt { 0,0 };
		ClientToScreen( hWnd, &pt );
		ScreenToClient( GetParent( hWnd ), &pt );
		BitBlt( hdcMem, 0, 0, cx, cy,
				hdcMemCanvas, pt.x, pt.y, SRCCOPY );

		// text background is transparent
		SetBkMode( hdcMem, TRANSPARENT );

		// select font
		SelectObject( hdcMem, hFont );

		for ( y = 0; y <= cyBuffer; ++y )
		{
			TextOut( hdcMem, 0, y * cyChar, pBuffer + y * cxBuffer, cxBuffer );
		}

		// copy bitmap to hdc, show bitmap
		BitBlt( hdc, 0, 0, cx, cy,
				hdcMem, 0, 0, SRCCOPY );
		EndPaint( hWnd, &ps );
		return 0;
	}
	case WM_KEYDOWN:
	{
		switch ( wParam )
		{
		case VK_HOME:
			iChar = iChar / cxBuffer * cxBuffer;
			break;
		case VK_END:
			iChar = ( iChar + cxBuffer ) / cxBuffer * cxBuffer - 1;
			if ( iChar > iNumChar )
				iChar = iNumChar;
			break;
		case VK_LEFT:
			iChar = max( 0, iChar - 1 );
			break;
		case VK_RIGHT:
			iChar = max( 0, min( iNumChar, iChar + 1 ) );
			break;
		case VK_UP:
			iChar = iChar - cxBuffer >= 0 ? iChar - cxBuffer : iChar;
			break;
		case VK_DOWN:
			if ( iChar + cxBuffer <= iNumChar )
				iChar = iChar + cxBuffer;
			break;
		case VK_DELETE:
		{
			if ( iNumChar - 1 < iChar ) break;

			if ( --iNumChar < 0 )
			{
				iNumChar = 0;
				break;
			}

			for ( i = iChar; i < cxMaxBuffer * cyMaxBuffer - 1; ++i )
				pBuffer[ i ] = pBuffer[ i + 1 ];
			pBuffer[ i ] = TEXT( ' ' );

			if ( CARET_Y( iNumChar ) + 1 - cyBuffer )
				SendMessage( hWnd, WM_LINECHANGE, 0, CARET_Y( iNumChar ) + 1 - cyBuffer );

			InvalidateRect( hWnd, NULL, TRUE );
			break;
		}
		case VK_ESCAPE:
		{
			SendMessage( hWnd, WM_CLOSE, 0, 0 );
			break;
		}
		default:
			break;
		}
		SetCaretPos( CARET_X( iChar ) * cxChar, CARET_Y( iChar ) * cyChar );
		return 0;
	}
	case WM_CHAR:
	{
		for ( i = 0; i < ( short ) LOWORD( lParam ); ++i )
		{
			switch ( wParam )
			{
			case '\b':
			{
				if ( iChar == 0 ) break;
				--iChar;
				SendMessage( hWnd, WM_KEYDOWN, VK_DELETE, 1 );
				break;
			}
			case '\t':
			{
				do
				{
					SendMessage( hWnd, WM_CHAR, TEXT( ' ' ), 1 );
				} while ( CARET_X( iChar ) % 8 );
				break;
			}
			case '\n':
			{
				int diff = cxBuffer;
				while ( diff-- )
					SendMessage( hWnd, WM_CHAR, TEXT( ' ' ), 1 );
				break;
			}
			case '\r':
			{
				int diff = ( iChar + cxBuffer ) / cxBuffer * cxBuffer - iChar;
				while ( diff-- )
					SendMessage( hWnd, WM_CHAR, TEXT( ' ' ), 1 );
				break;
			}
			case '\x1B': // ESC
				_tcsnset( pBuffer, TEXT( ' ' ), ( size_t ) ( cxBuffer * cyBuffer ) );
				iChar = 0;
				iNumChar = 0;
				InvalidateRect( hWnd, NULL, FALSE );
				break;
			default:
			{
				if ( ++iNumChar >= MAXBUFFSIZE )
				{
					iNumChar = MAXBUFFSIZE;
					break;
				}

				for ( i = cxMaxBuffer * cyMaxBuffer - 1; i > iChar; --i )
					pBuffer[ i ] = pBuffer[ i - 1 ];
				pBuffer[ i ] = ( TCHAR ) wParam;

				++iChar;
				if ( CARET_Y( iNumChar ) + 1 - cyBuffer )
					SendMessage( hWnd, WM_LINECHANGE, 0, CARET_Y( iNumChar ) + 1 - cyBuffer );

				InvalidateRect( hWnd, NULL, TRUE );
				break;
			}
			}
		}
		SetCaretPos( CARET_X( iChar ) * cxChar, CARET_Y( iChar ) * cyChar );
		return 0;
	}
	case WM_DESTROY:
	{
		HideCaret( hWnd );
		DestroyCaret( );

		RECT rcClient;
		GetClientRect( hWnd, &rcClient );
		int cx = rcClient.right - rcClient.left;
		int cy = rcClient.bottom - rcClient.top;

		HWND hwndParent = GetParent( hWnd );
		HDC hdcParent = GetDC( hwndParent );

		POINT pt { 0,0 };
		ClientToScreen( hWnd, &pt );
		ScreenToClient( hwndParent, &pt );

		BitBlt( hdcMemCanvas, pt.x, pt.y, cx, cy, hdcMem, 0, 0, SRCCOPY );
		BitBlt( hdcParent, pt.x, pt.y, cx, cy, hdcMem, 0, 0, SRCCOPY );

		ReleaseDC( hwndParent, hdcParent );
		DeleteDC( hdcMem );
		if ( pBuffer )
		{
			free( pBuffer );
			pBuffer = NULL;
		}
		break;
	}
	default:
		break;
	}
	return DefWindowProc( hWnd, message, wParam, lParam );
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

BOOL OnContextMenu( HWND hwnd, HWND hwndContext, UINT xPos, UINT yPos )
{
	POINT pt;
	pt.x = xPos;
	pt.y = yPos;

	if ( !PtInRect( &canvasRect, pt ) ) 
		return FALSE;
	
	HMENU hMenu = CreatePopupMenu( );
	AppendMenu( hMenu, MF_STRING, IDM_TRANS_CUT, TEXT( "Cut" ) );
	AppendMenu( hMenu, MF_STRING, IDM_TRANS_COPY, TEXT( "Copy" ) );
	AppendMenu( hMenu, MF_STRING, IDM_TRANS_PASTE, TEXT( "Paste" ) );
	AppendMenu( hMenu, MF_STRING, IDM_TRANS_DEL, TEXT( "Delete" ) );
	AppendMenu( hMenu, MF_SEPARATOR, 0, 0 );
	AppendMenu( hMenu, MF_STRING, IDM_TRANS_UNDO, TEXT( "Undo" ) );
	AppendMenu( hMenu, MF_STRING, IDM_TRANS_REDO, TEXT( "Redo" ) );
	TrackPopupMenu( hMenu, TPM_RIGHTBUTTON, pt.x, pt.y,
					0, hwnd, NULL );
	DestroyMenu( hMenu );
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