#define PI 3.141592

#define MENU_ITEM_MAXNUM 12		//メニュー表示項目数MAX
#define MENU_ITEM_MAXLEN 100	//メニュー表示項目文字MAX
#define MENU_ITEM_MAX 12*5	//メニュー表示項目文字MAX(5ページ分)

#define MENU_ITEMD_MAX 1000		//メニュー項目定義MAX
#define MENU_TIMING_TYPE 100	//オブジェクトのタイプ名
#define MENU_ITEMD_LANG 10		//メニュー項目スクリプト言語
#define MENU_SUBSET_MAX 50		//メニューセット内のサブセットMAX
#define MENU_SUBSET_DEF_COUNT 8 //サブセットメニュー表示デフォルト

#define MENU_R0 15		// メニュー中心からの不感領域の半径
#define MENU_R1 65		// メニュー中心からのメニューテキストBOX描画開始距離
#define MENU_R2 70		// メニュー中心からのメニューテキストBOX描画開始距離（吹き出し用）
#define MENU_BOX_R 4	// メニューテキストBOXの角のR
#define MENU_BOX_H 18	// メニューテキストBOXの高さ

#define PROF_FILE L"GMenu.xml"					// メニュー設定ファイル名
#define D_PROF_FILE L"default.xml"				// デフォルトメニューセット
#define D_SUBDIR L"..\\..\\Data\\Preferences"	// デフォルトサブディレクトリ（PluginPathからの相対）

#define BUF_SIZE 100*1000	// 実行スクリプトバッファサイズ

#define MAX(a,b) ((a)>(b)?(a):(b))

#ifdef _DEBUG
#define DebugLog( message )	XSI::Application().LogMessage( message, XSI::siVerboseMsg )
#else
#define DebugLog( message )
#endif

// GMenu.xml から ///////////////////////
struct GKEYMAP { //キーアサイン
	WPARAM Select;
	WPARAM Cancel;
	WPARAM Forward;
	WPARAM Back;
};

struct GITEMDATA { //メニューアイテム定義
	TCHAR gItemLabel[MENU_ITEM_MAXLEN];		// メニューラベル
	TCHAR gItemLang[MENU_ITEMD_LANG];		// メニュースクリプト言語
	TCHAR gItemScript[BUF_SIZE];	// スクリプト
};
/////////////////////////////////////////

// default.xml(メニューセットから)///////
#define GM_MODEL		0x00000001	//0b00001
#define GM_ANIMATION	0x00000002	//0b00010
#define GM_SIMU			0x00000004	//0b00100
#define GM_RENDER		0x00000008	//0b01000
#define GM_HAIR			0x00000010	//0b10000

struct GTIMING_MODE {
	unsigned int model:1;
	unsigned int animation:1;
	unsigned int simu:1;
	unsigned int render:1;
	unsigned int hair:1;
};

struct GTIMING {
	DWORD mode;
	TCHAR type[MENU_TIMING_TYPE];
};

struct GMENUSET {
	int MaxItem;
	struct GTIMING Timing;
	int ItemCount;
	struct GITEMDATA *Item[MENU_ITEM_MAX];
};
/////////////////////////////////////////

struct GDATA {
	HWND hWnd;			// メニューウィンドウハンドル
	RECT gWinSize;		// メニュー描画用のWindowサイズ(円)
	POINT gT[3];		// メニュー吹き出し部分三角形頂点座標
	POINT gCenter;		// メニューセンター座標（マウスカーソルの位置）
	int gMenuSplits;	// メニューが何分割か 2～
	TCHAR gMenuText[MENU_ITEM_MAX][MENU_ITEM_MAXLEN];	// メニューの項目表示テキスト
	int gMenuItemCount;	// メニューセットに含まれるメニューデータの数 0～
	int gMenuPage;		// 表示中のページ番号 0～
	POINT gMenuTextPos[MENU_ITEM_MAX]; // メニュー表示テキスト座標位置
	HRGN hRgnMenu[MENU_ITEM_MAX];		// メニュー表示用リージョン
	HRGN hRgnAMenu[MENU_ITEM_MAX];	// メニュー感知用リージョン
	int gCSubset;
//	TCHAR gMenuFile[MENU_ITEM_MAX][BUF_SIZE];	// メニュー選択時、実行ファイル名
};


//------------------------------------------------------------------------------------

#ifdef    GLOBAL_VALUE_DEFINE
#define   GLOBAL
#define   GLOBAL_VAL(v)  = (v)
#else
#define   GLOBAL	extern
#define   GLOBAL_VAL(v)
#endif

GLOBAL HINSTANCE gInstance; 			// Windowクラス登録用のハンドル
GLOBAL HWND gWnd;						// XSIのWindowハンドル
GLOBAL TCHAR szClassName[] GLOBAL_VAL(L"GMENU");	//メニューウィンドウのクラス名
GLOBAL TCHAR gFilePath[_MAX_PATH];				// PluginのFilePath
GLOBAL TCHAR cmdBuf[BUF_SIZE];					// スクリプト実行用バッファ
GLOBAL int cmdNum;						// 選択されたメニューINDEX

GLOBAL TCHAR gProfDir[_MAX_PATH];		// 設定ファイルディレクトリ名
GLOBAL TCHAR gProfFile[_MAX_PATH];		// メニューセット定義ファイル名( gProfDir + PROF_FILE )
GLOBAL TCHAR gProfFileG[_MAX_PATH];		// メニュー定義ファイル名( gProfDir + D_PROF_FILE )
GLOBAL struct GKEYMAP gKeymap;

GLOBAL int gItemCount;					// メニュー定義の項目数
GLOBAL struct GITEMDATA gItemData[MENU_ITEMD_MAX];
GLOBAL int gSubsetCount GLOBAL_VAL( 0 );
GLOBAL struct GMENUSET gSubset[MENU_SUBSET_MAX];

GLOBAL struct GDATA gMenu;
//------------------------------------------------------------------------------------

SIZE myGetTextWandH( TCHAR *txt );
HRGN myCreateMenuRgn( POINT p0, TCHAR *txt, int nums, int num, struct GDATA *data );
void mySetMenuText( TCHAR *menu[], int nums );
HRGN myCreateMenuARgn( struct GDATA *data, int num );
void myExec( struct GDATA *data, int num );
int myGetToolBarMode( HWND xsi );
bool myLoadMenu( void );
bool myInitData( XSI::CRef& in_ctxt, DWORD mode );
void mySetWinSize( struct GDATA *data, int page );
void mySetMenuRgn( struct GDATA *data, int page );
bool myInC0( struct GDATA *data, POINT pt );