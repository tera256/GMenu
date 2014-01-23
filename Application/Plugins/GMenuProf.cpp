#define WIN32_LEAN_AND_MEAN
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <tchar.h>
#include <windows.h>
#include <atlbase.h>  // CComPtrを使用するため
#include <atlstr.h>   // CStringを使用するため
#include <xmllite.h>


#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_desktop.h>
//using namespace XSI;

#include "GMenuSub.h"

#pragma comment(lib, "xmllite.lib")

bool mySubsetTiming( struct GTIMING *timing, TCHAR *str )
{
	TCHAR *tok;
	TCHAR tmp[MENU_TIMING_TYPE];
	
	if ( (tok = wcstok( str, L":" )) == NULL ) { // ":"なし
		timing->mode = 0;
		lstrcpy( timing->type, L"" );
	} else {
		lstrcpyn( tmp, tok, MENU_TIMING_TYPE-1 );
		if ( (tok = wcstok( NULL, L":" )) == NULL ) {
			if ( (_wtoi( tmp ) > 0) || ( (_wtoi( tmp ) ==0) && (wcscmp( tmp, L"0" ) == 0) ) ) {
				timing->mode = _wtoi( tmp );
				lstrcpy( timing->type, L"" );
			} else {
				timing->mode = 0;
				lstrcpy( timing->type, tmp );
			}
		} else {
			timing->mode = _wtoi( tmp );
			lstrcpyn( timing->type , tok, MENU_TIMING_TYPE-1 );
		}
	}

	return true;
}

bool mySubsetItemSet( struct GITEMDATA **item, int num, TCHAR *str )
{
	struct _stat buf;
	int index;

	index = _wtoi( str );

/*	if ( (index == 0) && (wcscmp( str, L"0" ) != 0) ) {
		return false;
	}*/
//	DebugLog( gItemData[index].gItemLabel + XSI::CString( index ) );
	item[num] = (GITEMDATA *)&gItemData[index];

	return true;
}

bool myLoadSet( void )
{
	CComPtr<IXmlReader> pReader;
    if(FAILED( CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<void**>(&pReader), 0))) {
        DebugLog( L"CreateXmlReader失敗" );
        return false;
    }

    // XMLファイルパス作成
    TCHAR xml[MAX_PATH];
	lstrcpy( xml, gProfFile );
    // ファイルストリーム作成
    CComPtr<IStream> pStream;
    if(FAILED(SHCreateStreamOnFile(xml, STGM_READ, &pStream))){
        DebugLog( L"err:SHCreateStreamOnFile" );
        return false;
    }

    if(FAILED(pReader->SetInput(pStream))){
		DebugLog( L"err:SetInput" );
        return false;
    }

    CString result;
    CString type;
    LPCWSTR pwszLocalName;
    LPCWSTR pwszValue;
    XmlNodeType nodeType;
    HRESULT hr;
    struct GMENUSET *set;
	gSubset[0].ItemCount = 0;
    while(S_OK == pReader->Read(&nodeType)){
		set = &gSubset[gSubsetCount];
        switch(nodeType){
        case XmlNodeType_EndElement:
            if(FAILED(pReader->GetLocalName(&pwszLocalName, NULL))){
				DebugLog( L"err:GetLocalName" );
                return false;
            }
			if ( _wcsicmp(pwszLocalName, _T("MenuSet") ) == 0 ) {
				gSubsetCount++; // メニューアイテム定義カウント＋＋
			}
            break;

        case XmlNodeType_Element:
            if(FAILED(pReader->GetLocalName(&pwszLocalName, NULL))){
				DebugLog( L"err:GetLocalName" );
                return false;
            }
            hr = pReader->MoveToFirstAttribute();
			if(S_FALSE == hr) break; // 属性が無い
			
			if(S_OK != hr){
				DebugLog( L"err:MoveToFirstAttribute" );
				return false;
			}
			if ( _wcsicmp(pwszLocalName, _T("MenuSet") ) == 0 ) {
				do {
					LPCWSTR pwszAttributeName, pwszAttributeValue;
					if(FAILED(pReader->GetLocalName(&pwszAttributeName, NULL))){
						DebugLog( L"err:GetLocalName" );
						return false;
					}
					if(FAILED(pReader->GetValue(&pwszAttributeValue, NULL))){
						DebugLog( L"err:GetValue" );
						return false;
					}
					if(lstrcmp(pwszAttributeName, _T("id")) == 0){ // MenuSubset ID 読み込み
//						DebugLog( L"subset id = "+ XSI::CString(pwszAttributeValue) );
					}
				} while (S_OK == pReader->MoveToNextAttribute());
			} else if ( _wcsicmp(pwszLocalName, _T("MenuItem") ) == 0 ) {
				do {
					LPCWSTR pwszAttributeName, pwszAttributeValue;
					if(FAILED(pReader->GetLocalName(&pwszAttributeName, NULL))){
						DebugLog( L"err:GetLocalName" );
						return false;
					}
					if(FAILED(pReader->GetValue(&pwszAttributeValue, NULL))){
						DebugLog( L"err:GetValue" );
						return false;
					}
					if(lstrcmp(pwszAttributeName, _T("id")) == 0){ // MenuItem ID 読み込み
						//wprintf(L"item id = %s\n",pwszAttributeValue);
						set->ItemCount++;
					}
				} while (S_OK == pReader->MoveToNextAttribute());
			}
            break;
        case XmlNodeType_Text:
            if(FAILED(pReader->GetValue(&pwszValue, NULL))){
				DebugLog( L"err:GetValue" );
                return false;
            }
            if(lstrcmp(pwszLocalName, _T("MaxItem")) == 0){
            	set->MaxItem = _wtoi( pwszValue );
            }else if(lstrcmp(pwszLocalName, _T("Timing")) == 0){
				mySubsetTiming( &(set->Timing), (TCHAR *)pwszValue );
				DebugLog( XSI::CString(set->Timing.mode) );
				DebugLog( XSI::CString(set->Timing.type) );
            }else if(lstrcmp(pwszLocalName, _T("MenuItem")) == 0){
//				DebugLog( L"Call(mySubsetItemSet):" + XSI::CString( (set->ItemCount)-1 ) );
				mySubsetItemSet( set->Item, (set->ItemCount)-1, (TCHAR *)pwszValue );
				//wprintf(L"MenuItem(%s)\n",set->Text[gSubsetCount]);
            }
            break;
        }
    }

	return true;
}

bool myLoadGlobal( void )
{
	CComPtr<IXmlReader> pReader;
    if(FAILED( CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<void**>(&pReader), 0))) {
        DebugLog( L"CreateXmlReader失敗" );
        return false;
    }

    // XMLファイルパス作成
    TCHAR xml[MAX_PATH];
	lstrcpy( xml, gProfFileG );
    // ファイルストリーム作成
    CComPtr<IStream> pStream;
    if(FAILED(SHCreateStreamOnFile(xml, STGM_READ, &pStream))){
        DebugLog( L"err:SHCreateStreamOnFile" );
        return false;
    }

    if(FAILED(pReader->SetInput(pStream))){
		DebugLog( L"err:SetInput" );
        return false;
    }

    CString result;
    CString type;
    LPCWSTR pwszLocalName;
    LPCWSTR pwszValue;
    XmlNodeType nodeType;
    HRESULT hr;
	gItemCount = 0;
    while(S_OK == pReader->Read(&nodeType)){
        switch(nodeType){
        case XmlNodeType_Element:
            if(FAILED(pReader->GetLocalName(&pwszLocalName, NULL))){
				DebugLog( L"err:GetLocalName" );
                return false;
            }
            hr = pReader->MoveToFirstAttribute();
			if(S_FALSE == hr) break; // 属性が無い
			
			if(S_OK != hr){
				DebugLog( L"err:MoveToFirstAttribute" );
				return false;
			}
			if ( _wcsicmp(pwszLocalName, _T("MenuItemD") ) == 0 ) {
				gItemCount++; // メニューアイテム定義カウント＋＋
				do {
					LPCWSTR pwszAttributeName;
					LPCWSTR pwszAttributeValue;
					if(FAILED(pReader->GetLocalName(&pwszAttributeName, NULL))){
						DebugLog( L"err:GetLocalName" );
						return false;
					}
					if(FAILED(pReader->GetValue(&pwszAttributeValue, NULL))){
						DebugLog( L"err:GetValue" );
						return false;
					}
					if(lstrcmp(pwszAttributeName, _T("id")) == 0){ // MenuItemD ID 読み込み
						//DebugLog ( L"def id = " + XSI::CString(pwszAttributeValue) );
					}
				} while (S_OK == pReader->MoveToNextAttribute());
			}
            break;
        case XmlNodeType_Text:
            if(FAILED(pReader->GetValue(&pwszValue, NULL))){
				DebugLog( L"err:GetValue" );
                return false;
            }
            if(lstrcmp(pwszLocalName, _T("Label")) == 0){
				lstrcpyn( gItemData[gItemCount-1].gItemLabel, (TCHAR *)pwszValue, MENU_ITEM_MAXLEN-1 );
            }else if(lstrcmp(pwszLocalName, _T("Lang")) == 0){
				lstrcpyn( gItemData[gItemCount-1].gItemLang, (TCHAR *)pwszValue, MENU_ITEMD_LANG-1 );
            }else if(lstrcmp(pwszLocalName, _T("Script")) == 0){
				lstrcpyn( gItemData[gItemCount-1].gItemScript, (TCHAR *)pwszValue, BUF_SIZE-1 );
			}else if(lstrcmp(pwszLocalName,_T("Forward")) == 0){
				gKeymap.Forward = _wtoi( pwszValue );
			}else if(lstrcmp(pwszLocalName,_T("Select")) == 0){
				gKeymap.Select = _wtoi( pwszValue );
			}else if(lstrcmp(pwszLocalName,_T("Cancel")) == 0){
				gKeymap.Cancel = _wtoi( pwszValue );
			}else if(lstrcmp(pwszLocalName,_T("Back")) == 0){
				gKeymap.Back = _wtoi( pwszValue );
			}
            break;
        }
    }

	return true;
}

/* メニュー設定ファイルからデータ読み込み */
bool myLoadMenu( void )
{
	gItemCount = 0;
	gSubsetCount = 0;
	//gKeymap,gItemData
//	gKeymap.Forward = 0x44;

	if ( ! myLoadGlobal() )
		return false;
	if ( ! myLoadSet() )
		return false;

	return true;
}
