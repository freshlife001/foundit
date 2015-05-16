//////////////////////////////////////////////////////////////////////////
//
//	描述:   列表项基类，提供基本的操作
//
//	
//	日期:   2011/08/18
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <set>

#include "uilib/wndless/IWndLess.h"

class IWLIndexItem :public IWndLess
{
public:
	virtual ~IWLIndexItem(){}
    virtual void SetIndex(int) = 0 ;
    virtual int GetIndex() const = 0 ;
};

//////////////////////////////////////////////////////////////////////////
// Selectable Item

class IWLSelectableItem : public IWLIndexItem
{
public:
	virtual ~IWLSelectableItem(){}
    virtual bool IsSelected() const = 0 ;
    virtual void Select() = 0 ;
    virtual void Unselect() = 0 ;
};

class ItemSelectManager
{
public:
	typedef std::set<IWLSelectableItem*> ITEMLIST ;
	virtual ~ItemSelectManager(){}

	virtual void SetMultipleSelect(bool mutiple) = 0 ;
	virtual bool IsMultipleSelect() const = 0 ;

	virtual IWLSelectableItem * GetSelectedItem() const = 0 ;
	virtual int GetMultipleSelectedItem(ITEMLIST & selected_item_list) const = 0 ;

	// uFlags标记当前的按键、鼠标状态
	virtual void SelectItem(IWLSelectableItem * item, UINT uFlags) = 0 ;
	virtual void UnselectItem(IWLSelectableItem * item, UINT uFlags) = 0 ;
	virtual void UnselectAllItem() = 0 ;

	virtual void LeftClickItem(IWLSelectableItem * item) = 0 ;
	virtual void LeftDoubleClickItem(IWLSelectableItem * item) = 0 ;
	virtual void RightClickItem(IWLSelectableItem * item) = 0 ;
	virtual void RightDoubleClickItem(IWLSelectableItem * item) = 0 ;

};

class IWLSimpleListItem :  public IWLSelectableItem
{
public:
    virtual ~IWLSimpleListItem() {}
	virtual LPCTSTR GetText() const = 0;
	virtual void SetText(LPCTSTR lpszText) = 0;
	virtual void SetSelectManager(ItemSelectManager * manager)= 0;
};

typedef CWndLessImpBase< CWndLessShell<IWLSimpleListItem> >  WLSimpleListItemImpBase;

template <class IT = IWLSimpleListItem, class IMPT = WLSimpleListItemImpBase > 
class CWLSimpleListItemShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLSimpleListItemShell() {}
	virtual ~CWLSimpleListItemShell(){}
	virtual void SetIndex(int i)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetIndex(i);
		}
		return ;

	}
	virtual int GetIndex() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetIndex();
		}
		return -1;

	}
	virtual bool IsSelected() const
	{
		if (m_wlimp)
		{
			return m_wlimp->IsSelected();
		}
		return false;

	}
	virtual void Select()
	{
		if (m_wlimp)
		{
			return m_wlimp->Select();
		}
		return;

	}
	virtual void Unselect()
	{
		if (m_wlimp)
		{
			return m_wlimp->Unselect();
		}
		return;

	}
	virtual LPCTSTR GetText() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetText();
		}
		return L"";

	}
	virtual void SetText(LPCTSTR lpszText)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetText(lpszText);
		}
		return ;

	}
	virtual void SetSelectManager(ItemSelectManager * manager)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetSelectManager(manager);
		}
		return ;

	}
	
};
typedef CWLSimpleListItemShell<> WLSimpleListItem;
