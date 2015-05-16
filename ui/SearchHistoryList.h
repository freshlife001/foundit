#pragma once
#include "uilib/wndless/IWLSimpleList.h"
#include "uilib/wndless/IWLListItem.h"
#include "uilib/common/utils/ImageEx/Shared_ImageEx.h"

class CSearchHistoryItem :
	public WLSimpleListItem
{
public:
	CSearchHistoryItem();
	virtual ~CSearchHistoryItem(void);

public:
	void SetIcon(Shared_ImageEx icon) { m_imgIcon = icon; }
	void SetBkgndImages(
		Shared_ImageEx imgHot,
		Shared_ImageEx imgSel
		) ;
	void SetActivate(bool bActivate);
	bool IsActivated() const { return m_bActivated; }

	void SetLeftMargin( int nLeftMargin ){ m_nMarginLeft = nLeftMargin; }
	virtual void SetMarkImg(Shared_ImageEx pAffiMark);

protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult);
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate);

protected:
	bool m_bHoted;
	bool m_bActivated;
	Shared_ImageEx m_imgIcon;
	Shared_ImageEx m_pImgHot;
	Shared_ImageEx m_pImgSel;

	int m_nMarginLeft;
};


class CSearchHistoryList :
	public WLSimpleList
{
public:
	CSearchHistoryList(void);
	virtual ~CSearchHistoryList(void);
	void Init();
	int Insert(LPCTSTR text);
	bool Delete(unsigned int index);
	void DeleteAll();

	int SearchItem(LPCTSTR text);
	LPCTSTR GetItemText(unsigned int index);

protected:
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate);
private:
	Shared_ImageEx m_pImgHot;
	Shared_ImageEx m_pImgSel;

};
