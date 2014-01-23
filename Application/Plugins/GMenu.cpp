// GMenuPlugin
// Initial code generated by XSI SDK Wizard
// Executed Sun Feb 7 17:30:50 UTC+0900 2010 by yama
// 
// Tip: You need to compile the generated code before you can load the plug-in.
// After you compile the plug-in, you can load it by clicking Update All in the Plugin Manager.
#include <tchar.h>
#include <windows.h>

#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_desktop.h>
using namespace XSI;

#define GLOBAL_VALUE_DEFINE
#include "GMenuSub.h"

/* ���j���[�E�B���h�E���b�Z�[�W���� */
LRESULT CALLBACK WndProc(HWND hwnd , UINT msg , WPARAM wp , LPARAM lp) {
	HDC hdc;
	PAINTSTRUCT ps;
	CREATESTRUCT *cs;
	static struct GDATA *data;
	static int gMenuSplits;
	static TCHAR **gMenuText;
	static HRGN *hRgnMenu;
	static HRGN *hRgnAMenu;
	static POINT pt;
	static BOOL bIn;
	static int cRgn;
	static HRGN hWindow;
	int x,y;
	int num;

	switch (msg) {
	case WM_CREATE: // Window���ŏ��ɍ��ꂽ�Ƃ��i��x�������s�����̂ŁA�e�평���������j
		cs = (CREATESTRUCT *)lp;
		data = (GDATA *)cs->lpCreateParams;
		gMenuSplits = data->gMenuSplits;
		gMenuText = (TCHAR **)&(data->gMenuText[0][0]);
		hRgnMenu = data->hRgnMenu;
		hRgnAMenu = data->hRgnAMenu;
		cRgn = -1;
		cmdNum = -1;
		hWindow = CreateEllipticRgn( 1,1, data->gWinSize.right-data->gWinSize.left, data->gWinSize.bottom-data->gWinSize.top );
		SetWindowRgn(hwnd, hWindow, TRUE);
		SetLayeredWindowAttributes(hwnd, RGB(255,0,0), 0, LWA_COLORKEY);
		return 0;

	case WM_KEYDOWN: // �L�[���͏���
		if (( wp == gKeymap.Forward ) && ( cRgn == -1 )){ // �y�[�W���O
			if ( GetKeyState( gKeymap.Back ) & 0x80 ) { // �߂�
				if ( data->gMenuPage > 0 )
					data->gMenuPage--;
			} else {									// �i��
				if ( data->gMenuPage < (data->gMenuItemCount/(data->gMenuSplits*1.0))-1 )
					data->gMenuPage++;
			}
			mySetWinSize( data, data->gMenuPage );
			mySetMenuRgn( data, data->gMenuPage );

			MoveWindow( hwnd, data->gWinSize.left, data->gWinSize.top, data->gWinSize.right - data->gWinSize.left, data->gWinSize.bottom - data->gWinSize.top, FALSE );
			DeleteObject( hWindow );
			hWindow = CreateEllipticRgn( 1,1, data->gWinSize.right-data->gWinSize.left, data->gWinSize.bottom-data->gWinSize.top );
			SetWindowRgn(hwnd, hWindow, FALSE);
			InvalidateRect( hwnd, NULL, TRUE );
			gMenuText = (TCHAR **)&(data->gMenuText[data->gMenuPage*data->gMenuSplits][0]);
			cRgn = -1;
		} else if ( wp == gKeymap.Cancel ) { // Cancel
			SendMessage(hwnd, WM_CLOSE, 0, 0); // Window����
		} else if ( wp == gKeymap.Select ) { // Select
			if ( cRgn != -1 ) {
				cmdNum = cRgn;
			}
			SendMessage(hwnd, WM_CLOSE, 0, 0); // Window����
		}
		return 0;

	case WM_LBUTTONDOWN:    // �}�E�X���N���b�N
		if ( cRgn != -1 ) { // �A�N�e�B�u�ȃ��j���[������΁A���j���[Index���Z�b�g
			cmdNum = cRgn;
		}
		SendMessage(hwnd, WM_CLOSE, 0, 0); // Window����
		return 0;

	case WM_RBUTTONDOWN: // �}�E�X�E�N���b�N
		if ( cRgn == -1 ) {
			//
		} else {
			//
		}
		SendMessage(hwnd, WM_CLOSE, 0, 0); // Window����
		return 0;

	case WM_KILLFOCUS:   // ���j���[Window�ȊO�Ƀt�H�[�J�X���ڂ�����
		SendMessage(hwnd, WM_CLOSE, 0, 0); // Window����
		return 0;

	case WM_DESTROY: // �I�����b�Z�[�W
		PostQuitMessage(0);
		return 0;

	case WM_PAINT: // �`�揈��
		hdc = BeginPaint(hwnd , &ps);
		SelectObject( hdc, CreateSolidBrush(RGB(255, 0, 0)) );
		ExtFloodFill( hdc, (data->gWinSize.right-data->gWinSize.left)/2, (data->gWinSize.bottom-data->gWinSize.top)/2,  RGB(255,255,255), FLOODFILLSURFACE );
		SelectObject( hdc, CreateSolidBrush(RGB(200, 200, 200)) );
		for (int i=0; i<gMenuSplits; i++ ) { // ���j���[�p�̃��[�W�����`��
			PaintRgn(hdc , hRgnMenu[i]);
		}
		SelectObject( hdc, GetStockObject( DEFAULT_GUI_FONT ) ); // ���j���[�t�H���g�ݒ�
		SelectObject( hdc, CreateSolidBrush(RGB(0,0,0)) ); // �t�H���g�̐F�ݒ�
		SetBkMode( hdc, TRANSPARENT ); // �t�H���g�`�惂�[�h�ݒ�
		num = data->gMenuPage * data->gMenuSplits;
		for ( int i=0; i<data->gMenuSplits; i++ ) { // ���j���[�t�H���g�`��
			TextOut( hdc, data->gMenuTextPos[i].x, data->gMenuTextPos[i].y, data->gMenuText[i+num], lstrlen( data->gMenuText[i+num] ) );
		}
		EndPaint(hwnd , &ps);

		return 0;

	case WM_MOUSEMOVE: // �}�E�X�ړ�
		pt.x = LOWORD(lp);
		pt.y = HIWORD(lp);
		hdc = GetDC( hwnd );
		for (int i=0; i<gMenuSplits; i++ ) { // ���j���[�I�����m
			bIn = PtInRegion( hRgnAMenu[i], pt.x, pt.y ); // ���m�p���[�W���������`�F�b�N
			if ( bIn ) {
				if ( cRgn != i ) { // �O�̃��[�W�����ԍ��ƈႤ�Ȃ�i���]�\�������j
					if ( cRgn == -1 ) { // �����j���[���[�W�����A�N�e�B�u���i���]�\�������j
						InvertRgn( hdc, hRgnMenu[i] );
					} else { // ���[�W�������烊�[�W�����ֈړ����i���]�\�������j
						InvertRgn( hdc, hRgnMenu[cRgn] );
						InvertRgn( hdc, hRgnMenu[i] );
					}
					cRgn = i; // �I�����ꂽ���[�W�����ԍ����Z�b�g
				}
//				DebugLog(L"IN REGION" + CString(cRgn));
			} else {
			}
		}
		if (( cRgn != -1 ) && myInC0( data, pt )) {
			InvertRgn( hdc, hRgnMenu[cRgn] );
			cRgn = -1;
		}
		ReleaseDC(hwnd, hdc);
		return 0;
	}

	return DefWindowProc(hwnd , msg , wp , lp); // ��`���ĂȂ����b�Z�[�W���������肢
}

/* ���j���[�p�E�B���h�E�N���X�o�^ */
static ATOM myWindowClassReg( HINSTANCE hInst )
{
    WNDCLASS        wc;

    wc.style        =CS_HREDRAW|CS_VREDRAW; 
    wc.lpfnWndProc  =WndProc;
    wc.cbClsExtra   =0;
    wc.cbWndExtra   =0;
    wc.hInstance    =hInst;
    wc.hIcon        =NULL;
    wc.hCursor      =LoadCursor(0,MAKEINTRESOURCE(IDC_ARROW));
    wc.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName =NULL;
    wc.lpszClassName=szClassName;


    return (RegisterClass(&wc));
}

/* Plugin ���[�h�G���g�� */
XSIPLUGINCALLBACK CStatus XSILoadPlugin( PluginRegistrar& in_reg )
{
	in_reg.PutAuthor(L"oyaG");
	in_reg.PutName(L"GMenuPlugin");
	in_reg.PutEmail(L"");
	in_reg.PutURL(L"");
	in_reg.PutVersion(1,0);
	in_reg.RegisterCommand(L"GMenu",L"GMenu");
	//RegistrationInsertionPoint - do not remove this line

	Application().LogMessage(in_reg.GetName() + L" is loading.",siVerboseMsg);
	return CStatus::OK;
}

/* Plugin �A�����[�h�G���g�� */
XSIPLUGINCALLBACK CStatus XSIUnloadPlugin( const PluginRegistrar& in_reg )
{
	CString strPluginName;
	strPluginName = in_reg.GetName();
	UnregisterClass( szClassName, gInstance );
	Application().LogMessage(strPluginName + L" has been unloaded.",siVerboseMsg);
	return CStatus::OK;
}

/* ���j���[�R�}���h�p�����[�^��` */
/* GMenu ���j���[�Z�b�g��		  */
/* �ŌĂяo�����				  */
/* �E�B���h�E�N���X�o�^���s�AXSI�̃E�B���h�E�n���h���擾�͂����� */
XSIPLUGINCALLBACK CStatus GMenu_Init( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	Command oCmd(ctxt.GetSource());
	oCmd.PutDescription(L"");
	oCmd.SetFlag(siCannotBeUsedInBatch,true);
	oCmd.SetFlag(siNoLogging,true);
	oCmd.EnableReturnValue(true);

	ArgumentArray oArgs = oCmd.GetArguments();
	oArgs.Add(L"MenuSetName");

	gWnd = (HWND)Application().GetDesktop().GetApplicationWindowHandle();	//XSI �̃E�B���h�E�n���h��
	gInstance = GetModuleHandle(NULL);										//�����̃C���X�^���X


	if ( myWindowClassReg( gInstance ) == 0 ) {	//�E�B���h�E�N���X�o�^
		Application().LogMessage(L" cannnot regiser window class.",siVerboseMsg);
		return CStatus::False;
	}
	DebugLog(L" registered windows class.");
	return CStatus::OK;
}

/* ���j���[���������G���g���B�@���b�Z�[�W���[�v�֓˓� */
XSIPLUGINCALLBACK CStatus GMenu_Execute( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );

	Command oCmd(ctxt.GetSource());

	lstrcpy( gFilePath ,oCmd.GetFileName().GetWideString() ); //�����̃t�@�C���p�X
//	cmdBuf = (TCHAR *)calloc( BUF_SIZE, sizeof(TCHAR) ); //���s�p�o�b�t�@�m��

	DebugLog(L"GMenu_Execute called:");
/////////////////////////////////////////////////////////////////////////
	int mode = myGetToolBarMode( gWnd );
	DebugLog( L"Mode:" + CString(mode) );
/////////////////////////////////////////////////////////////////////////
	// 
	// TODO: Put your command implementation here.
	if ( ! myInitData( in_ctxt, mode )) //���j���[�p�f�[�^���[�h��������
		return CStatus::False;
//return CStatus::OK;
////////////////////////////////////////////////////////////////////////
//	struct GDATA *data = (struct GDATA *)( (CValue::siPtrType)ctxt.GetUserData() );
	struct GDATA *data = &gMenu; //���j���[�\���p�f�[�^

	HWND hWnd = CreateWindowEx(WS_EX_LAYERED,
            szClassName, L"GMenu",
			/*WS_OVERLAPPEDWINDOW|*/ WS_POPUP,
			data->gWinSize.left , data->gWinSize.top ,
			data->gWinSize.right - data->gWinSize.left, data->gWinSize.bottom - data->gWinSize.top,
            gWnd, //�e�E�B���h�E�̃n���h���A�e�����Ƃ���NULL
            NULL, //���j���[�n���h���A�N���X���j���[���g���Ƃ���NULL
            gInstance, //�C���X�^���X�n���h��
            data); // �J�X�^���f�[�^

	if (!hWnd) { // Window�n���h���`�F�b�N
		Application().LogMessage(L"Cannot create menu window");
		return CStatus::False;
	} else {
		data->hWnd = hWnd;
	}

    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);

	DebugLog(L"While start");
	MSG msg;

	while(GetMessage(&msg , NULL , 0 , 0)) DispatchMessage(&msg); //���b�Z�[�W���[�v

	DebugLog(L"While End");
	// 
	// Return a value by setting this attribute:
	ctxt.PutAttribute( L"ReturnValue", true );

	if ( cmdNum != -1 ) // ���j���[���s
		myExec( data, cmdNum );

	// Return CStatus::Fail if you want to raise a script error
	return CStatus::OK;
}