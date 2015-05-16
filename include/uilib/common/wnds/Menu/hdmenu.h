//////////////////////////////////////////////////////////////////////////
//
//	描述:   菜单接口头文件
//
//	
//	日期:   2011/03/22
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////
#include "uilib/uilib_exports.h"

#pragma once

typedef HANDLE HHDMENU ;

#define MF_RADIO	0x00010000

namespace hdui {

	struct MENUCORE ;
	typedef MENUCORE* PMENUCORE ;

	struct MENUITEMCORE ;
	typedef MENUITEMCORE* PMENUITEMCORE ;

	struct MENUITEMCORE
	{
		unsigned int uCmd ;
		unsigned int uStyle ;
		TCHAR szText[256] ;
		unsigned int uIconId ;

		// 这里可添加额外数据

		//PMENUITEMCORE pPrevItem ;
		PMENUITEMCORE pNextItem ;

		PMENUCORE hSubMenu ;
	} ;

	struct MENUCORE
	{
		PMENUITEMCORE pFirstItem ;
	} ;

	UILIB_EXPORT HHDMENU CreatePopupMenu() ;
	UILIB_EXPORT BOOL DestroyMenu(HHDMENU hMenu) ;

	UILIB_EXPORT BOOL AppendMenu(HHDMENU hMenu, unsigned int uFlags, unsigned int uIDNewItem, LPCTSTR lpszText, unsigned int uIconId = 0) ;

	UILIB_EXPORT BOOL EnableMenuItem(HHDMENU hMenu, unsigned int uId, unsigned int uEnable) ;

	// check_or_radio: MF_CHECK/MF_RADIO
	UILIB_EXPORT BOOL CheckMenuItem(HHDMENU hMenu, unsigned int uId, unsigned int check_or_radio) ;

	UILIB_EXPORT BOOL TrackPopupMenu(HHDMENU hMenu, unsigned int uFlags, int x, int y, HWND hOwner, LPRECT lprcExclude = NULL) ;

} ;
