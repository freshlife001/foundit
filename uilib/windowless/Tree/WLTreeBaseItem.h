//////////////////////////////////////////////////////////////////////////
//
//	描述:   树控件节点根基类
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

class WLTreeBaseItem ;
typedef WLTreeBaseItem * HWLTREEITEM ;

// Special Item Sign
#define TI_ROOT                ((HWLTREEITEM)0xFFFF0000)
#define TI_FIRST               ((HWLTREEITEM)0xFFFF0001)
#define TI_LAST                ((HWLTREEITEM)0xFFFF0002)
#define TI_SORT                ((HWLTREEITEM)0xFFFF0003)

// Status
#define TIS_NORMAL				0x00		// 普通
#define TIS_HOT					0x01		// 鼠标经过
#define TIS_SELECTED			0x02		// 被选中
#define TIS_EXPAND				0x04		// 被展开
#define TIS_HIDE				0x08		// 被隐藏
#define TIS_DRAGOVER			0x10		// 拖拽经过
#define TIS_FILTERED			0x20		// 被过滤的(不显示)

// Expand Code
#define TE_COLLAPSE				0x01
#define TE_EXPAND				0x02
#define TE_TOGGLE				0x03
#define TE_EXPAND_RECURSIVE		0x04
#define TE_COLLAPSE_RECURSIVE	0x05

// TreeItem Type
#define TIT_BASE				_T("TreeBaseItem")

class WLTreeBaseItem :
	public CWndLess
{
public:
	WLTreeBaseItem(void);
	virtual ~WLTreeBaseItem(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// Attribute
	HWLTREEITEM GetParentItem() const ;
	HWLTREEITEM GetPrevSibling() const ;
	HWLTREEITEM GetNextSibling() const ;
	HWLTREEITEM GetFirstChild() const ;

	void SetParentItem(HWLTREEITEM hItem) ;
	void SetPrevSibling(HWLTREEITEM hItem) ;
	void SetNextSibling(HWLTREEITEM hItem) ;
	void SetFirstChild(HWLTREEITEM hItem) ;

	// 判断当前节点是否有指定节点这个孩子
	BOOL HasChild(HWLTREEITEM hItem, BOOL bRecursive = TRUE) const ;

	// 获得最后一个孩子
	HWLTREEITEM GetLastChild() ;

	// 是否展开
	BOOL IsExpand() const ;

	// 是否可见
	BOOL IsVisible() const ;

	// 是否显示
	BOOL IsShow() const ;

	// 获得树根节点
	HWLTREEITEM GetRoot() ;

	// 获得树
	HWL GetTree() ;

public:
	//////////////////////////////////////////////////////////////////////////
	// TreeItem Implement

	virtual BOOL InsertChild(HWLTREEITEM hItem, HWLTREEITEM hInsertAfter) ;		// 插入孩子
	virtual BOOL DeleteChild(HWLTREEITEM hChild) ;								// 删除孩子
	virtual void DeleteAllChild() ;												// 删除所有孩子
	virtual BOOL Expand(UINT uCode) ;											// 展开/闭合
	virtual BOOL Show(BOOL bShow) ;												// 显示/隐藏
	virtual BOOL Filter(BOOL bShow) ;											// 过滤，显示/隐藏
	virtual void Type(CString &sType) const { sType = TIT_BASE ; }

protected:
	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;

protected:
	// Base Data
	//HWLTREEITEM	m_hRoot ;
	HWLTREEITEM	m_hParentItem ;
	HWLTREEITEM	m_hPrevSibling ;
	HWLTREEITEM	m_hNextSibling ;
	HWLTREEITEM	m_hFirstChild ;

	// Data
	DWORD m_dwStatus ;			// 节点状态

};
