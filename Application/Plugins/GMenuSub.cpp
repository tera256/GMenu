#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_desktop.h>
#include <xsi_selection.h>
#include <xsi_subcomponent.h>
using namespace XSI;

#include "GMenuSub.h"

bool myInC0( struct GDATA *data, POINT pt )
{
	POINT p0 = { data->gCenter.x, data->gCenter.y };
	p0.x = (data->gWinSize.right-data->gWinSize.left)/2;
	p0.y = (data->gWinSize.bottom-data->gWinSize.top)/2;

	int dx = (p0.x - pt.x)*(p0.x - pt.x);
	int dy = (p0.y - pt.y)*(p0.y - pt.y);
	int r = MENU_R0*MENU_R0;

//	DebugLog( CString( r ) +L":"+ CString( dx ) +L":"+ CString( dy ) );
	if ( (dx+dy) < r ) {
//		DebugLog( CString( r ) +L":"+ CString( dx ) +L":"+ CString( dy ) );
		return true;
	} else
		return false;
}

int sign( int x )
{
	int y = x==0 ? 0 : ( x>0 ? 1: -1 );
	return y;
}

/* ファイル名から拡張子を返す */
TCHAR *myGetFileExt( TCHAR *s )
{
	TCHAR drive[_MAX_DRIVE];	// ドライブ名
	TCHAR dir[_MAX_DIR];		// ディレクトリ名
	TCHAR fname[_MAX_FNAME];	// ファイル名
	TCHAR ext[_MAX_EXT];		// 拡張子

	_wsplitpath_s( s, drive, dir, fname, ext );

	return ext;
}

/* ファイルのフルパスを生成して cmd にセット */
void myGetFullPath( TCHAR *cmd, TCHAR *file )
{
	TCHAR drive[_MAX_DRIVE];	// ドライブ名
	TCHAR dir[_MAX_DIR];		// ディレクトリ名
	TCHAR fname[_MAX_FNAME];	// ファイル名
	TCHAR ext[_MAX_EXT];		// 拡張子
	struct _stat buf;

	_wsplitpath_s( file, drive, dir, fname, ext );

	TCHAR tmp[_MAX_PATH];

	if ( lstrlen( dir ) == 0 ) {
		lstrcpy( tmp, gProfDir );
		wcsncat( tmp, file, _MAX_PATH -lstrlen( tmp ) -1 );
	} else
		lstrcpy( tmp, file );

	_wfullpath( cmd, tmp, _MAX_PATH );
}

/* 選択したメニューの実行 */
void myExec( struct GDATA *data, int num )
{
	int item_num = num + data->gMenuPage * data->gMenuSplits; //実行するアイテム実番号
	TCHAR cmd[_MAX_PATH]; //実行する外部ファイル
	TCHAR ext[_MAX_EXT];  //実行する外部ファイルの拡張子
	FILE *fp;
	TCHAR *script = &gSubset[data->gCSubset].Item[item_num]->gItemScript[0];
	TCHAR line[1000]=L"";
	int size = 0;

	lstrcpy( cmdBuf, L"" );

	if ( *script == L'@' ) {
		TCHAR *p = script + 1;
		myGetFullPath( cmd, p );

		_wsetlocale(LC_ALL, L".ACP");

		if ( (fp = _wfopen( cmd, L"rt" )) == NULL ) {
			Application().LogMessage( L"Cannot open:" + CString( cmd ), siVerboseMsg );
			return;
		}
		while ( (fgetws( line, sizeof(line)-1, fp ) != NULL) && ( size<BUF_SIZE) ) {
			size += lstrlen( line );
			lstrcat( cmdBuf, line );
		}
		if ( size == 0 ) {
			fp = _wfreopen( cmd, L"rb", fp );
			DebugLog( L"fread" );
			fread( cmdBuf, sizeof(TCHAR), BUF_SIZE, fp );
		}

		fclose( fp );

		lstrcpy( ext, myGetFileExt(cmd) );

//		Application().LogMessage( L"Execute: " + CString(cmd), siVerboseMsg );

		if ( _wcsicmp( ext, L".vbs" ) == 0 )
			Application().ExecuteScriptCode( CString(cmdBuf), L"VBScript" );
		else if ( _wcsicmp( ext, L".js" ) == 0 )
			Application().ExecuteScriptCode( CString(cmdBuf), L"JScript" );
		else if ( _wcsicmp( ext, L".py" ) == 0 )
			Application().ExecuteScriptCode( CString(cmdBuf), L"Python" );
	} else {
		lstrcpy( cmdBuf, script );
		Application().ExecuteScriptCode( CString(cmdBuf), gSubset[data->gCSubset].Item[item_num]->gItemLang );
	}
}

bool myIsMode( struct GMENUSET set, DWORD mode )
{
	DWORD c_mode;

	switch ( mode ) {
		case 1:
			c_mode = GM_MODEL;			break;
		case 2:
			c_mode = GM_ANIMATION;		break;
		case 3:
			c_mode = GM_SIMU;			break;
		case 4:
			c_mode = GM_RENDER;			break;
		case 5:
			c_mode = GM_HAIR;			break;
		default:
			c_mode = 0xFFFFFFFF;
	}
	DebugLog( L"IsMode M:" + CString(set.Timing.mode) );
	DebugLog( L"IsMode m:" + CString(c_mode) );
	if ( (set.Timing.mode & c_mode) > 0 || (set.Timing.mode == 0))
		return true;
	else
		return false;
}

bool myIsType( struct GMENUSET set, TCHAR *type )
{
	TCHAR *ret;

	if ( (lstrlen(set.Timing.type) == 0) && (lstrlen( type ) == 0) )
		return true;
	DebugLog( L"IsType>" + CString( set.Timing.type ) );
	if ( (lstrlen( type ) == 0) && (lstrlen(set.Timing.type) > 0) )
		return false;
	if ( (ret = wcsstr( set.Timing.type, type )) != NULL ) {
		DebugLog( L"IsType:" + CString(ret) );
		return true;
	} else {
		DebugLog( L"IsType:none" );
		return false;
	}
}

/* Modeとselectionのtypeからサブセット新規追加 */
bool myMenuSetAdd( int num, DWORD mode, TCHAR *type )
{
	return false; // どうするか決められないので、追加はなし 2010/4/25
/*	if ( num > MENU_SUBSET_MAX )
		return false;
	gSubsetCount++;
	gSubset[gSubsetCount].ItemCount = MENU_SUBSET_DEF_COUNT;
	gSubset[gSubsetCount].MaxItem = MENU_SUBSET_DEF_COUNT;
	gSubset[gSubsetCount].Timing.mode = mode;
	lstrcpy( gSubset[gSubsetCount].Timing.type, type );
	for ( int i=0; i<MENU_ITEM_MAX; i++ ) {
		lstrcpy( (gSubset[gSubsetCount].Item)[i]->gItemScript, L"" );
		lstrcpy( (gSubset[gSubsetCount].Item)[i]->gItemLang, L"VBScript" );
		wprintf( gSubset[gSubsetCount].Item)[i]->gItemLabel, L"Menu%d", i );
	}
	return true;*/
}

/* サブセット番号 num から struct GDATA 作成 */
void myBuildMenuSub( struct GDATA *data, int num )
{
	struct GMENUSET *set = &gSubset[num];

	data->gMenuSplits = set->MaxItem;
	data->gMenuItemCount = set->ItemCount;
	data->gMenuPage = 0;
//	data->gMenuText = (TCHAR **)calloc( data->gMenuItemCount, sizeof(TCHAR *) );
	for ( int i=0; i<(data->gMenuItemCount); i++ ) {
//		*(data->gMenuText+i) = (TCHAR *)calloc( MENU_ITEM_MAXLEN, sizeof(TCHAR) );
		lstrcpy( data->gMenuText[i], set->Item[i]->gItemLabel );
		DebugLog( set->Item[i]->gItemLabel );
	}
}

/* メニュー用データ struct GDATA 生成 */
bool myBuildMenu( struct GDATA *data, DWORD mode )
{
	Application app;
	TCHAR otype[MENU_TIMING_TYPE];
	CScriptErrorDescriptor status;
	CValueArray fooArgs(1);
	CValue retVal;

	status = app.ExecuteScriptProcedure(
		L"function foo() {\n if (selection.count > 0) {\n var oo=selection(0)\n var typen=Application.ClassName(oo)\n if (typen==\"CollectionItem\") {\n typen=oo.subcomponent.type\n }\n }\n return typen\n }",
		L"JScript",L"foo", fooArgs, retVal);
	if ( 0 == (status.GetCode()) ) {
		lstrcpy( otype, retVal.GetAsText().GetWideString() );
	} else {
		lstrcpy( otype, L"" );
	}
	DebugLog( L"Selection:" + CString(otype) );
	//app.LogMessage( status.GetDescription() + CString( L" - " ) + CValue( status.GetErrorLineNumber() ).GetAsText() ) ;
	int i=0;
	do {
		if ( myIsMode( gSubset[i], mode ) && myIsType( gSubset[i], otype ) ) // First match
			break;
		i++;
	} while ( i<gSubsetCount );

	if ( i == gSubsetCount ) { // Modeとselectionにマッチするサブセットが無ければ追加？
		DebugLog( L"Select SubsetID:None" );
		if ( ! myMenuSetAdd( i, mode, otype ) )
			return false;
	}
	DebugLog( L"Select SubsetID:" + CString(i) );
	myBuildMenuSub( data, i );
	data->gCSubset = i; // 選択されたサブセット番号

	return true;
}

/* メニューセット名から読み込む設定ファイルを取得 */
void myMakeProfFile( TCHAR *name )
{
	TCHAR path[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];	// ドライブ名
	TCHAR dir[_MAX_DIR];		// ディレクトリ名
	TCHAR fname[_MAX_FNAME];	// ファイル名
	TCHAR ext[_MAX_EXT];		// 拡張子
	TCHAR tmp[_MAX_PATH];

	lstrcpyn( path, gFilePath, _MAX_PATH-1 );
	_tsplitpath( gFilePath, drive, dir, fname, ext );

	swprintf( tmp, L"%s%s%s\\", drive, dir, D_SUBDIR );
//	DebugLog( L"myMakeProfFile:D  :" + CString(tmp) );
	_wfullpath( gProfDir, tmp, sizeof(gProfDir) );
//	DebugLog( L"myMakeProfFile:D  :" + CString(gProfDir) );
	swprintf( gProfFileG, L"%s%s", gProfDir, PROF_FILE ); //メニュー項目定義ファイル

	if ( lstrlen( name ) == 0 ) {
		lstrcpy( fname, D_PROF_FILE );
	} else {
		swprintf( fname, L"%s.xml", name );
	}
	swprintf( gProfFile, L"%s%s", gProfDir, fname );

	DebugLog( L"myMakeProfFile:G  :" + CString(gProfFileG) );
	DebugLog( L"myMakeProfFile:Set:" + CString(gProfFile) );

}


// page のメニュー用ウィンドウ用 RECT設定
void mySetWinSize( struct GDATA *data, int page )
{
	SIZE sz;
	int dx = 0;
	int num = page * data->gMenuSplits;

	for ( int i=num; i<num + data->gMenuSplits; i++ ) {
		sz = myGetTextWandH( data->gMenuText[i] );
		dx = MAX( dx, sz.cx );
	}
	data->gWinSize.left   = data->gCenter.x - MENU_R2 - MENU_BOX_R - dx;
	data->gWinSize.top    = data->gCenter.y - MENU_R2 - MENU_BOX_R - dx;
	data->gWinSize.right  = data->gCenter.x + MENU_R2 + MENU_BOX_R + dx;
	data->gWinSize.bottom = data->gCenter.y + MENU_R2 + MENU_BOX_R + dx;

/*	TCHAR tmp[256];
	wsprintf(tmp, L"x= %d, y= %d, x0= %d, y0= %d, x1= %d, y1=%d", data->gCenter.x, data->gCenter.y,
		data->gWinSize.left, data->gWinSize.top, data->gWinSize.right, data->gWinSize.bottom );
	DebugLog( tmp );*/
}


void mySetMenuRgn( struct GDATA *data, int page )
{
	int num = data->gMenuSplits * page;
	bool del_flag = false;

	if ( data->hRgnMenu[0] != NULL )
		del_flag = true;
	for (int i=0 ; i<data->gMenuSplits; i++ ) { // メニュー用のリージョン作成
		if ( del_flag )
			DeleteObject( data->hRgnMenu[i] );
		data->hRgnMenu[i] = myCreateMenuRgn( data->gCenter, data->gMenuText[i+num], data->gMenuSplits, i, data );
//		DebugLog( L"mySetMenuRgn:" + CString(data->gMenuText[i+num]) );
	}
	for (int i=0; i<data->gMenuSplits; i++ ) { // 選択検知用のリージョン作成
		if ( del_flag )
			DeleteObject( data->hRgnAMenu[i] );
		data->hRgnAMenu[i] = myCreateMenuARgn( data, i );
	}
}

/* メニュー用データ初期化 */
bool myInitData( CRef& in_ctxt, DWORD mode )
{
	Context ctxt( in_ctxt );
	CValueArray args = ctxt.GetAttribute(L"Arguments");
	CString strArg0 = args[0].GetAsText();
	TCHAR *MenuSetName = (TCHAR *)strArg0.GetWideString(); // UNICODE対応のため
//	struct GDATA *data = (struct GDATA *)calloc( 1, sizeof( struct GDATA ) );
	struct GDATA *data = &gMenu;

	GetCursorPos( &(data->gCenter) );
	data->gT[0].x = 0 , data->gT[0].y = MENU_R0+5;
	data->gT[1].x = -3, data->gT[1].y = MENU_R2;
	data->gT[2].x = 3 , data->gT[2].y = MENU_R2;

	myMakeProfFile( MenuSetName ); // gProfFileG, gProfFile セット
	if ( ! myLoadMenu() ) return false;	// メニュー設定データのロード
	if ( ! myBuildMenu( data, mode ) ) // 表示メニューデータ構築
		return false;

	mySetWinSize( data, data->gMenuPage );

	data->hRgnMenu[0] = NULL;
	data->hRgnAMenu[0] = NULL;
	mySetMenuRgn( data, data->gMenuPage );

//	ctxt.PutUserData( (CValue::siPtrType) data );

	return true;
}

/* テキスト表示に必要なサイズ計算 */
SIZE myGetTextWandH( TCHAR *txt )
{
	HDC hdc = GetDC( 0 );
	HFONT hF = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
	HFONT hFO = (HFONT)SelectObject( hdc, hF );
	SIZE sz;

	DebugLog( L"myGetTextWandH:" + CString( txt ) );
	GetTextExtentPoint32( hdc, txt, lstrlen( txt ), &sz );

	ReleaseDC( 0, hdc );

	return sz;
}

/* メニュー選択検知用リージョン作成 */
HRGN myCreateMenuARgn( struct GDATA *data, int num )
{
	HRGN hC0, hC1, hT;
	double fRadDelta = 2*PI/data->gMenuSplits;
	double dt = fRadDelta*(num);
	int r0 = MENU_R0; //不感領域半径
//	int r1 = (data->gWinSize.right-data->gWinSize.left)/2; //検知領域半径
	int r1 = MENU_R1;
	POINT tri[3];

	int x0 = (data->gWinSize.right-data->gWinSize.left)/2;
	int y0 = (data->gWinSize.bottom-data->gWinSize.top)/2;

	tri[0].x = x0; tri[0].y = y0; //クライアント領域センター
	double dt1 = dt - fRadDelta*0.5;
	double dt2 = dt + fRadDelta*0.5;
	tri[1].x = x0 + r1*sin(dt1); tri[1].y = y0 - r1*cos(dt1);
	tri[2].x = x0 + r1*sin(dt2); tri[2].y = y0 - r1*cos(dt2);

	if ( (hT = CreatePolygonRgn( tri, 3 , ALTERNATE )) == NULL ) // メニュー吹き出し部分三角作成
		Application().LogMessage(L"cannot create triangle area region");

	if ( (hC0 = CreateEllipticRgn( x0-r0, y0-r0, x0+r0, y0+r0 )) == NULL ) // メニュー吹き出し部分三角作成
		Application().LogMessage(L"cannot create c0 circle area region");

	if ( (hC1 = CreateEllipticRgn( x0-r1, y0-r1, x0+r1, y0+r1 )) == NULL ) // メニュー吹き出し部分三角作成
		Application().LogMessage(L"cannot create c1 circle area region");

	CombineRgn( hC0, hC0, hC1, RGN_XOR );
	CombineRgn( hC0, hC0, hT, RGN_AND );
	CombineRgn( hC0, hC0, data->hRgnMenu[num], RGN_OR );

	DeleteObject( hT );
	DeleteObject( hC1 );

	return hC0;
}

/* メニュー表示用リージョン作成 */
/* 中心:p0 メニューテキスト:txt 何分割か:nums 何番目か:num */
HRGN myCreateMenuRgn( POINT p0, TCHAR *txt, int nums, int num, struct GDATA *data )
{
	HRGN hRgnB,hRgnT;
	double fRadDelta = 2*PI/nums;
	double dt = fRadDelta*num;
	int dx = MENU_R1*sin( dt );
	int dy = -MENU_R1*cos( dt );
	POINT box[2],tri[3];

	int x0 = (data->gWinSize.right-data->gWinSize.left)/2;
	int y0 = (data->gWinSize.bottom-data->gWinSize.top)/2;

	SIZE sz = myGetTextWandH( txt ); // メニューテキスト表示に必要な幅&高さ
	sz.cx += 10; // メニューBOX用にちょっと広げる

	tri[0].x = x0 + data->gT[0].x*cos(dt) + data->gT[0].y*sin(dt); tri[0].y = y0 + data->gT[0].x*sin(dt) - data->gT[0].y*cos(dt);
	tri[1].x = x0 + data->gT[1].x*cos(dt) + data->gT[1].y*sin(dt); tri[1].y = y0 + data->gT[1].x*sin(dt) - data->gT[1].y*cos(dt);
	tri[2].x = x0 + data->gT[2].x*cos(dt) + data->gT[2].y*sin(dt); tri[2].y = y0 + data->gT[2].x*sin(dt) - data->gT[2].y*cos(dt);

	if ( (hRgnT = CreatePolygonRgn( tri, 3 , ALTERNATE )) == NULL ) // メニュー吹き出し部分三角作成
		Application().LogMessage(L"cannot create triangle region");

	switch ( sign(dx) ) {
		case -1:		// BOX位置が左側
			box[0].x = x0 + dx - sz.cx;		box[0].y = y0 + dy - MENU_BOX_H/2;
			box[1].x = x0 + dx;				box[1].y = y0 + dy + MENU_BOX_H/2;
			break;
		case 0:			// BOX位置が上下いずれか
			box[0].x = x0 + dx - sz.cx/2;	box[0].y = dy>0 ? y0 + dy				: y0 + dy - MENU_BOX_H;
			box[1].x = x0 + dx + sz.cx/2;	box[1].y = dy>0 ? y0 + dy + MENU_BOX_H	: y0 + dy;
			break;
		case 1:			// BOX位置が右側
			box[0].x = x0 + dx;				box[0].y = y0 + dy - MENU_BOX_H/2;
			box[1].x = x0 + dx + sz.cx;		box[1].y = y0 + dy + MENU_BOX_H/2;
			break;
	}

	if ( (hRgnB = CreateRoundRectRgn( box[0].x, box[0].y, box[1].x, box[1].y, MENU_BOX_R, MENU_BOX_R )) == NULL ) // メニュー用BOXリージョン作成
		Application().LogMessage(L"cannot create box region");

	data->gMenuTextPos[num].x = box[0].x+MENU_BOX_R;
	data->gMenuTextPos[num].y = box[0].y+(MENU_BOX_H-sz.cy)/2;

	CombineRgn( hRgnT, hRgnT, hRgnB, RGN_OR ); // 吹き出しとBOX合成

	DeleteObject( hRgnB );

	return hRgnT;
}
