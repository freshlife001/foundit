#pragma once
#include "uilib/wndless/IWLListItem.h"

class WLSimpleListItemImp :
    public WLSimpleListItemImpBase
{
public:
    WLSimpleListItemImp(LPCTSTR lpszText);
    virtual ~WLSimpleListItemImp(void);

public:
    virtual LPCTSTR GetText() const { return m_text.GetString() ; }
	virtual void SetText(LPCTSTR lpszText) { m_text = lpszText; }

    virtual void SetSelectManager(ItemSelectManager * manager) { m_select_manager = manager ; }
    virtual void Select() ;
    virtual void Unselect() ;

	virtual void SetIndex(int index) { m_index = index ; }
	virtual int GetIndex() const { return m_index ; }
	virtual bool IsSelected() const { return m_selected ; }

protected:
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
    virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;

private:
    CString m_text ;

    //int m_index ;
    //bool m_selected ;

    ItemSelectManager * m_select_manager ;

    bool m_left_button_down ;
    bool m_right_button_down ;

private:
	int m_index ;
	bool m_selected ;

};
