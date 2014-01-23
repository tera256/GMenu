#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <tchar.h>
#include <windows.h>
#include <stdio.h>

#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_desktop.h>
using namespace XSI;

#include "GMenuSub.h"

struct XSI_MODE {
	int mode;
	TCHAR label[100];
};

struct XSI_MODE xsi_mode[] = { {1,L"コンポーネント"}, {2,L"MOTOR"}, {3,L"レンダ"}, {4,L"リジッドボディ"}, {5,L"ヘア"},
	{1,L"Component"}, {3,L"Render"}, {4,L"Rigid Body"}, {5,L"Hair"}, {-1,L""} };
static HWND hXSI_WND;
static TCHAR sSearchTitle[1024];

BOOL CALLBACK EnumChildProc( HWND hWnd, LPARAM lParam )
{
	TCHAR szTitle[1024];
	TCHAR szClass[1024];
	int *lpCount = (int *)lParam;
  
	hXSI_WND = 0;
	*lpCount += 1;                                       // カウントの加算
	GetWindowText( hWnd, szTitle, sizeof(szTitle) );    // キャプションの取得
	GetClassName(  hWnd, szClass, sizeof(szClass) );    // クラス文字列の取得

	if ( _wcsicmp( szTitle, sSearchTitle ) == 0 ) {
//		CString mes = CString(L"HWND:") + CString( hWnd ) + CString( L":") + CString(szClass) + CString( L":") + CString(szTitle);
//		DebugLog( mes );
		hXSI_WND = hWnd;
		return FALSE;
	}

	return TRUE;
}

/* ツールバーの現在のモードを返す */
/* Model:1 Animate:2 Render:3 Simulate:4 Hair:5 */
int myGetToolBarMode( HWND xsi )
{
	int count = 0;
	int mode = 0;
	int i = 0;

	HWND h = FindWindowEx( gWnd, NULL, L"MDIClient", NULL );
	h = FindWindowEx( h, NULL, L"TrayWindowClass", NULL );
	h = GetWindow( h, GW_HWNDLAST );
	h = GetWindow( h, GW_CHILD );
	h = GetWindow( h, GW_CHILD );
//  ここまでで、XSIメインウィンドウの枠限定
	lstrcpy( sSearchTitle, L"ModuleMenuBar" );
	EnumChildWindows( h, EnumChildProc, (LPARAM)&count );
	h = hXSI_WND;
//  ToolBar のウィンドウハンドルゲット
	do { // ツールバーにModeに対する文字列を持つウィンドウがあれば、そのモードと判断する
		lstrcpy( sSearchTitle, xsi_mode[i].label );
		EnumChildWindows( h, EnumChildProc, (LPARAM)&count );
		if ( hXSI_WND != 0 )
			mode = xsi_mode[i].mode;
		i++;
	} while ( mode <= 0 );

	DeleteObject( h );

	return mode;
}