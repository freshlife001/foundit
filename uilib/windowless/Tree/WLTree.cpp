#include "StdAfx.h"
#include "WLTree.h"

WLTree::WLTree(void)
{
}

WLTree::~WLTree(void)
{
}

HWLTREEITEM WLTree::InsertItem(
							   HWLTREEITEM hItem,
							   HWLTREEITEM hParent,// = TI_ROOT
							   HWLTREEITEM hInsertAfter,// = TI_LAST
							   BOOL /*bDeferInsert*/// = FALSE
							   ) 
{
	if (hParent == TI_ROOT)
	{
		hParent = m_pRoot ;
	}
	else
	{
// 		if (m_pRoot != hParent && !m_pRoot->HasChild(hParent, TRUE))
// 			return NULL ;
	}

	// 插入孩子不成功则返回FALSE
	if (!hParent->InsertChild(hItem, hInsertAfter))
		return NULL ;

	//hItem->m_hRoot = &m_Root ;
	return hItem ;
}

BOOL WLTree::DeleteItem(
						HWLTREEITEM hItem
						) 
{
	if (!m_pRoot->HasChild(hItem, TRUE))
		return FALSE ;

	return hItem->GetParentItem()->DeleteChild(hItem) ;
}

void WLTree::DeleteAllItem() 
{
	if (m_pRoot != NULL)
		m_pRoot->DeleteAllChild() ;
}

//////////////////////////////////////////////////////////////////////////
// Implement

BOOL WLTree::OnCreate( DWORD /*dwStyle*/, int /*x*/, int /*y*/, int cx, int /*cy*/, IWndLess * /*pParent*/ ) 
{
	if (!m_pRoot->Create(WS_VISIBLE, 0, 0, cx, 0, this, ID_ROOT))
		return FALSE ;

	m_pRoot->Expand(TE_EXPAND) ;

	return TRUE ;
}
