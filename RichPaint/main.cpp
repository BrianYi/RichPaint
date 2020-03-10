// RichPaint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "RichPaint.h"
#include <windowsx.h>


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[ MAX_LOADSTRING ];                  // The title bar text
WCHAR szWindowClass[ MAX_LOADSTRING ];            // the main window class name
WCHAR szTransparentClass[ MAX_LOADSTRING ];            // the main window class name


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
	wcex.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
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
		HANDLE_MSG( hWnd, WM_CREATE, OnCreate );
		HANDLE_MSG( hWnd, WM_INITMENU, OnInitMenu );
		HANDLE_MSG( hWnd, WM_LBUTTONDOWN, OnLButtonDown );
		HANDLE_MSG( hWnd, WM_LBUTTONUP, OnLButtonUp );
		HANDLE_MSG( hWnd, WM_MOUSEMOVE, OnMouseMove );
		HANDLE_MSG( hWnd, WM_SIZE, OnSize );
		HANDLE_MSG( hWnd, WM_COMMAND, OnCommand );
		HANDLE_MSG( hWnd, WM_CONTEXTMENU, OnContextMenu );
		HANDLE_MSG( hWnd, WM_PAINT, OnPaint );
		HANDLE_MSG( hWnd, WM_DRAWITEM, OnDrawItem );
		HANDLE_MSG( hWnd, WM_DESTROY, OnDestroy );

 	default:
 		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}
