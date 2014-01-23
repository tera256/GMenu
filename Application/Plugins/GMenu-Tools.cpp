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

struct XSI_MODE xsi_mode[] = { {1,L"�R���|�[�l���g"}, {2,L"MOTOR"}, {3,L"�����_"}, {4,L"���W�b�h�{�f�B"}, {5,L"�w�A"},
	{1,L"Component"}, {3,L"Render"}, {4,L"Rigid Body"}, {5,L"Hair"}, {-1,L""} };
static HWND hXSI_WND;
static TCHAR sSearchTitle[1024];

BOOL CALLBACK EnumChildProc( HWND hWnd, LPARAM lParam )
{
	TCHAR szTitle[1024];
	TCHAR szClass[1024];
	int *lpCount = (int *)lParam;
  
	hXSI_WND = 0;
	*lpCount += 1;                                       // �J�E���g�̉��Z
	GetWindowText( hWnd, szTitle, sizeof(szTitle) );    // �L���v�V�����̎擾
	GetClassName(  hWnd, szClass, sizeof(szClass) );    // �N���X������̎擾

	if ( _wcsicmp( szTitle, sSearchTitle ) == 0 ) {
//		CString mes = CString(L"HWND:") + CString( hWnd ) + CString( L":") + CString(szClass) + CString( L":") + CString(szTitle);
//		DebugLog( mes );
		hXSI_WND = hWnd;
		return FALSE;
	}

	return TRUE;
}

/* �c�[���o�[�̌��݂̃��[�h��Ԃ� */
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
//  �����܂łŁAXSI���C���E�B���h�E�̘g����
	lstrcpy( sSearchTitle, L"ModuleMenuBar" );
	EnumChildWindows( h, EnumChildProc, (LPARAM)&count );
	h = hXSI_WND;
//  ToolBar �̃E�B���h�E�n���h���Q�b�g
	do { // �c�[���o�[��Mode�ɑ΂��镶��������E�B���h�E������΁A���̃��[�h�Ɣ��f����
		lstrcpy( sSearchTitle, xsi_mode[i].label );
		EnumChildWindows( h, EnumChildProc, (LPARAM)&count );
		if ( hXSI_WND != 0 )
			mode = xsi_mode[i].mode;
		i++;
	} while ( mode <= 0 );

	DeleteObject( h );

	return mode;
}