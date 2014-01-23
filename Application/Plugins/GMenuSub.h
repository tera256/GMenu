#define PI 3.141592

#define MENU_ITEM_MAXNUM 12		//���j���[�\�����ڐ�MAX
#define MENU_ITEM_MAXLEN 100	//���j���[�\�����ڕ���MAX
#define MENU_ITEM_MAX 12*5	//���j���[�\�����ڕ���MAX(5�y�[�W��)

#define MENU_ITEMD_MAX 1000		//���j���[���ڒ�`MAX
#define MENU_TIMING_TYPE 100	//�I�u�W�F�N�g�̃^�C�v��
#define MENU_ITEMD_LANG 10		//���j���[���ڃX�N���v�g����
#define MENU_SUBSET_MAX 50		//���j���[�Z�b�g���̃T�u�Z�b�gMAX
#define MENU_SUBSET_DEF_COUNT 8 //�T�u�Z�b�g���j���[�\���f�t�H���g

#define MENU_R0 15		// ���j���[���S����̕s���̈�̔��a
#define MENU_R1 65		// ���j���[���S����̃��j���[�e�L�X�gBOX�`��J�n����
#define MENU_R2 70		// ���j���[���S����̃��j���[�e�L�X�gBOX�`��J�n�����i�����o���p�j
#define MENU_BOX_R 4	// ���j���[�e�L�X�gBOX�̊p��R
#define MENU_BOX_H 18	// ���j���[�e�L�X�gBOX�̍���

#define PROF_FILE L"GMenu.xml"					// ���j���[�ݒ�t�@�C����
#define D_PROF_FILE L"default.xml"				// �f�t�H���g���j���[�Z�b�g
#define D_SUBDIR L"..\\..\\Data\\Preferences"	// �f�t�H���g�T�u�f�B���N�g���iPluginPath����̑��΁j

#define BUF_SIZE 100*1000	// ���s�X�N���v�g�o�b�t�@�T�C�Y

#define MAX(a,b) ((a)>(b)?(a):(b))

#ifdef _DEBUG
#define DebugLog( message )	XSI::Application().LogMessage( message, XSI::siVerboseMsg )
#else
#define DebugLog( message )
#endif

// GMenu.xml ���� ///////////////////////
struct GKEYMAP { //�L�[�A�T�C��
	WPARAM Select;
	WPARAM Cancel;
	WPARAM Forward;
	WPARAM Back;
};

struct GITEMDATA { //���j���[�A�C�e����`
	TCHAR gItemLabel[MENU_ITEM_MAXLEN];		// ���j���[���x��
	TCHAR gItemLang[MENU_ITEMD_LANG];		// ���j���[�X�N���v�g����
	TCHAR gItemScript[BUF_SIZE];	// �X�N���v�g
};
/////////////////////////////////////////

// default.xml(���j���[�Z�b�g����)///////
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
	HWND hWnd;			// ���j���[�E�B���h�E�n���h��
	RECT gWinSize;		// ���j���[�`��p��Window�T�C�Y(�~)
	POINT gT[3];		// ���j���[�����o�������O�p�`���_���W
	POINT gCenter;		// ���j���[�Z���^�[���W�i�}�E�X�J�[�\���̈ʒu�j
	int gMenuSplits;	// ���j���[���������� 2�`
	TCHAR gMenuText[MENU_ITEM_MAX][MENU_ITEM_MAXLEN];	// ���j���[�̍��ڕ\���e�L�X�g
	int gMenuItemCount;	// ���j���[�Z�b�g�Ɋ܂܂�郁�j���[�f�[�^�̐� 0�`
	int gMenuPage;		// �\�����̃y�[�W�ԍ� 0�`
	POINT gMenuTextPos[MENU_ITEM_MAX]; // ���j���[�\���e�L�X�g���W�ʒu
	HRGN hRgnMenu[MENU_ITEM_MAX];		// ���j���[�\���p���[�W����
	HRGN hRgnAMenu[MENU_ITEM_MAX];	// ���j���[���m�p���[�W����
	int gCSubset;
//	TCHAR gMenuFile[MENU_ITEM_MAX][BUF_SIZE];	// ���j���[�I�����A���s�t�@�C����
};


//------------------------------------------------------------------------------------

#ifdef    GLOBAL_VALUE_DEFINE
#define   GLOBAL
#define   GLOBAL_VAL(v)  = (v)
#else
#define   GLOBAL	extern
#define   GLOBAL_VAL(v)
#endif

GLOBAL HINSTANCE gInstance; 			// Window�N���X�o�^�p�̃n���h��
GLOBAL HWND gWnd;						// XSI��Window�n���h��
GLOBAL TCHAR szClassName[] GLOBAL_VAL(L"GMENU");	//���j���[�E�B���h�E�̃N���X��
GLOBAL TCHAR gFilePath[_MAX_PATH];				// Plugin��FilePath
GLOBAL TCHAR cmdBuf[BUF_SIZE];					// �X�N���v�g���s�p�o�b�t�@
GLOBAL int cmdNum;						// �I�����ꂽ���j���[INDEX

GLOBAL TCHAR gProfDir[_MAX_PATH];		// �ݒ�t�@�C���f�B���N�g����
GLOBAL TCHAR gProfFile[_MAX_PATH];		// ���j���[�Z�b�g��`�t�@�C����( gProfDir + PROF_FILE )
GLOBAL TCHAR gProfFileG[_MAX_PATH];		// ���j���[��`�t�@�C����( gProfDir + D_PROF_FILE )
GLOBAL struct GKEYMAP gKeymap;

GLOBAL int gItemCount;					// ���j���[��`�̍��ڐ�
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