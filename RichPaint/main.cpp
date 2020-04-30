// RichPaint.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "RichPaint.h"
#include <windowsx.h>
#include <magnification.h>
#include <assert.h>


// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR szTitle[ MAX_LOADSTRING ];                  // The title bar text
TCHAR szWindowClass[ MAX_LOADSTRING ];            // the main window class name
TCHAR szTransparentClass[ MAX_LOADSTRING ];            // the main window class name
TCHAR szHostWindowClassName[ MAX_LOADSTRING ];            // the main window class name
LRESULT CALLBACK HostWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
void CALLBACK       UpdateMagWindow( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );


int APIENTRY wWinMain( _In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPWSTR    lpCmdLine,
					   _In_ int       nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	// TODO: Place code here.

	// Initialize global strings
	LoadString( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
	LoadString( hInstance, IDC_RICHPAINT, szWindowClass, MAX_LOADSTRING );
	MyRegisterClass( hInstance );
	
	// Perform application initialization:
	if ( !InitInstance( hInstance, nCmdShow ) )
	{
		TCHAR szBuffer[128];
		wsprintf( szBuffer, TEXT( "%lu" ), GetLastError() );
		MessageBox( NULL, szBuffer, szWindowClass, MB_OK );
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
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof( WNDCLASSEX );

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_RICHPAINT ) );
	wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
	wcex.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 2 );
	wcex.lpszMenuName = MAKEINTRESOURCE( IDC_RICHPAINT );
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon( wcex.hInstance, MAKEINTRESOURCE( IDI_RICHPAINT ) );
	BOOL bSuccess = RegisterClassEx( &wcex );
	assert( bSuccess );

	lstrcpy( szTransparentClass, TEXT( "TRANSPARENT" ) );
	wcex.lpfnWndProc = TransparentWndProc;
	wcex.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
	wcex.lpszClassName = szTransparentClass;
	bSuccess = RegisterClassEx( &wcex ); 
	assert( bSuccess );

	bSuccess = MagInitialize();
	assert( bSuccess );

	lstrcpy( szHostWindowClassName, TEXT( "HostWindow" ) );
	wcex.cbSize = sizeof WNDCLASSEX;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = HostWndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = (HBRUSH)(1 + COLOR_BTNFACE);
	wcex.lpszClassName = szHostWindowClassName;
	bSuccess = RegisterClassEx( &wcex );
	assert( bSuccess );
	
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

	HWND hWnd = CreateWindow( szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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
		HANDLE_MSG( hWnd, WM_KEYDOWN, OnKey );
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
