
#include "stdafx.h"
#include "common/utils/UIUtils.h"
#include "hdmenu.h"
#include "MenuWindowMannger.h"

namespace hdui {

HHDMENU CreatePopupMenu() 
{
	MENUCORE * pMenu = new MENUCORE ;
	memset(pMenu, 0, sizeof(MENUCORE)) ;

	return (HHDMENU)pMenu ;
}

BOOL _FreeMenuCore(PMENUCORE pMenuCore) ;

BOOL _FreeMenuItemCore(PMENUITEMCORE pMenuItemCore)
{
	if (pMenuItemCore == NULL)
		return FALSE ;

	if (pMenuItemCore->hSubMenu != NULL)
	{
		_FreeMenuCore(pMenuItemCore->hSubMenu) ;
	}

	delete pMenuItemCore ;

	return TRUE ;
}

BOOL _FreeMenuCore(PMENUCORE pMenuCore)
{
	if (pMenuCore == NULL)
		return FALSE ;

	PMENUITEMCORE pItem = pMenuCore->pFirstItem ;
	for ( ; pItem != NULL ; )
	{
		PMENUITEMCORE pTemp = pItem->pNextItem ;
		_FreeMenuItemCore(pItem) ;
		pItem = pTemp ;
	}

	delete pMenuCore ;

	return TRUE ;
}

BOOL DestroyMenu(HHDMENU hMenu) 
{
	return _FreeMenuCore((PMENUCORE)hMenu) ;
}

BOOL AppendMenu(
				HHDMENU hMenu,
				unsigned int uFlags,
				unsigned int uIDNewItem,
				LPCTSTR lpszText,
				unsigned int uIconId// = 0
				) 
{
	if (hMenu == NULL)
		return FALSE ;

	PMENUITEMCORE pMenuItemCore = new MENUITEMCORE ;
	memset(pMenuItemCore, 0, sizeof(MENUITEMCORE)) ;

	pMenuItemCore->uStyle = uFlags ;
	if (lpszText != NULL)
	{
		_tcsncpy_s(pMenuItemCore->szText, lpszText, _countof(pMenuItemCore->szText) - 1) ;
	}

	if (uFlags & MF_POPUP)
		pMenuItemCore->hSubMenu = (PMENUCORE)uIDNewItem ;
	else
		pMenuItemCore->uCmd = uIDNewItem ;

	pMenuItemCore->uIconId = uIconId ;

	PMENUCORE pMenuCore = (PMENUCORE)hMenu ;
	PMENUITEMCORE pLastItem = pMenuCore->pFirstItem ;
	if (pLastItem == NULL)
	{
		pMenuCore->pFirstItem = pMenuItemCore ;
	}
	else
	{
		PMENUITEMCORE pPrevItem = NULL ;
		do
		{
			pPrevItem = pLastItem ;
			pLastItem = pLastItem->pNextItem ;
		} while (pLastItem != NULL) ;

		pPrevItem->pNextItem = pMenuItemCore ;
	}

	return TRUE ;
}

BOOL _EnableMenuItem(PMENUCORE pMenuCore, unsigned int uId, unsigned int uEnable) 
{
	if (pMenuCore == NULL)
		return FALSE ;

	PMENUITEMCORE pMenuItem = pMenuCore->pFirstItem ;
	while (pMenuItem != NULL)
	{
		if (pMenuItem->uStyle & MF_POPUP)
		{
			_EnableMenuItem(pMenuCore, uId, uEnable) ;
		}
		else
		{
			if (!(pMenuItem->uStyle & MF_SEPARATOR) && pMenuItem->uCmd == uId)
			{
				pMenuItem->uStyle &= ~(MF_GRAYED | MF_DISABLED) ;
				pMenuItem->uStyle |= uEnable ;
			}
		}

		pMenuItem = pMenuItem->pNextItem ;
	}

	return TRUE ;
}

BOOL EnableMenuItem(HHDMENU hMenu, unsigned int uId, unsigned int uEnable) 
{
	if (hMenu == NULL)
		return FALSE ;

	PMENUCORE pMenuCore = (PMENUCORE)hMenu ;
	return _EnableMenuItem(pMenuCore, uId, uEnable) ;
}

BOOL _CheckMenuItem(PMENUCORE pMenuCore, unsigned int uId, unsigned int check_or_radio) 
{
	if (pMenuCore == NULL)
		return FALSE ;

	PMENUITEMCORE pMenuItem = pMenuCore->pFirstItem ;
	while (pMenuItem != NULL)
	{
		if (pMenuItem->uStyle & MF_POPUP)
		{
			_CheckMenuItem(pMenuCore, uId, check_or_radio) ;
		}
		else
		{
			if (!(pMenuItem->uStyle & MF_SEPARATOR) && pMenuItem->uCmd == uId)
			{
				pMenuItem->uStyle &= ~(MF_POPUP | MF_CHECKED) ;
				pMenuItem->uStyle |= check_or_radio ;
			}
		}

		pMenuItem = pMenuItem->pNextItem ;
	}

	return TRUE ;
}

BOOL CheckMenuItem(HHDMENU hMenu, unsigned int uId, unsigned int check_or_radio) 
{
	if (hMenu == NULL)
		return FALSE ;

	PMENUCORE pMenuCore = (PMENUCORE)hMenu ;
	return _CheckMenuItem(pMenuCore, uId, check_or_radio) ;
}

BOOL TrackPopupMenu(HHDMENU hMenu, unsigned int uFlags, int x, int y, HWND hOwner, LPRECT lprcExclude/* = NULL*/)
{
	if (!::IsWindow(hOwner))	
		return FALSE ;

	CMenuWindowMannger mgr ;
	int ret = mgr.Run(hMenu, uFlags, x, y, hOwner, lprcExclude) ;

 	if ((uFlags & TPM_RETURNCMD) && ret)
 	{
 		hdutils::TrackUIOperation(hOwner, WM_COMMAND, MAKEWPARAM(ret, 0), 0);
 	}

	return ret;
}

};
