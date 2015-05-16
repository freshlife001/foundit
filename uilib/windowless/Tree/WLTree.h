//////////////////////////////////////////////////////////////////////////
//
//	描述:   树控件基类
//
//	
//	日期:   2011/01/27
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "..\wndless.h"
#include "WLTreeBaseItem.h"

class WLTree :
	public CWndLess
{
public:
	WLTree(void);
	virtual ~WLTree(void);

public:
	HWLTREEITEM InsertItem(
		HWLTREEITEM hItem,
		HWLTREEITEM hParent = TI_ROOT,
		HWLTREEITEM hInsertAfter = TI_LAST,
		BOOL bDeferInsert = FALSE
		) ;

	virtual BOOL DeleteItem(
		HWLTREEITEM hItem
		) ;

	virtual void DeleteAllItem() ;

	HWLTREEITEM GetRootItem() const
	{
		return m_pRoot ;
	}

protected:
	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;

protected:
	// Data
	enum
	{
		ID_ROOT = 47,
	};
	WLTreeBaseItem * m_pRoot ;

};
