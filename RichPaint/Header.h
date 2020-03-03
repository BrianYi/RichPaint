#pragma once
#include "resource.h"

#define MAX_LOADSTRING 100


//============================================================================
//
// all the windows id
//
#define ID_CURCOLOR						100

#define ID_PENCIL						IDB_PENCIL
#define ID_BRUSH						IDB_BRUSH
#define ID_AIRBRUSH						IDB_AIRBRUSH
#define ID_ERASER						IDB_ERASER
#define ID_TEXT							IDB_TEXT
#define ID_FILLWITHCOLOR				IDB_FILLWITHCOLOR
#define ID_COLORPICKER					IDB_COLORPICKER
#define ID_MAGNIFIER					IDB_MAGNIFIER
#define ID_ZOOMIN						IDB_ZOOMIN
#define ID_ZOOMOUT						IDB_ZOOMOUT
#define ID_COPY							IDB_COPY
#define ID_PASTE						IDB_PASTE
#define ID_CUT							IDB_CUT
#define ID_LINE							IDB_LINE
#define ID_CURVE						IDB_CURVE
#define ID_ROUNDEDRECT					IDB_ROUNDEDRECT
#define ID_RECT							IDB_RECT
#define ID_OVAL							IDB_OVAL


#define ID_EDITCOLOR					IDB_EDITCOLOR
#define ID_UNDO							IDB_UNDO
#define ID_REDO							IDB_REDO


//============================================================================
//
// the lowest id number of every category
//
#define COLORID		0
#define CURSORID	200
#define TOOLID		300
#define EDITCOLORID	500
#define CANVASID	1000
#define COLNUM		8
#define ICONSIZEX	40
#define ICONSIZEY	40

//============================================================================
//
// find out which category the id is belongs to
//
#define IS_COLORID(id)		(id >= COLORID && id < CURSORID)
#define IS_CURSORID(id)		(id >= CURSORID && id < TOOLID)
#define IS_TOOLID(id)		(id >= TOOLID && id < EDITCOLORID)
// #define IS_EDITCOLORID(id)	(id >= EDITCOLORID && id < CANVASID)
// #define IS_CANVASID(id)		(id == CANVASID)

//============================================================================
//
// be used to find out the index of every category
//
#define INDEX_COLOR(id)		(id - COLORID)
#define INDEX_CURSOR(id)	(id - CURSORID)
#define INDEX_TOOL(id)		(id - TOOLID)

#define SET_GLOBAL_CURSOR(hCursor)	\
	SetClassLongPtr( GetParent(tools[ 0 ].hWnd), GCLP_HCURSOR, (LONG)hCursor );	\
	SetClassLongPtr( tools[ 0 ].hWnd, GCLP_HCURSOR, ( LONG ) hCursor );	