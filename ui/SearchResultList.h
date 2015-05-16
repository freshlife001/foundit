#pragma once
#include "uilib/wndless/IWLSimpleList.h"
#include "uilib/wndless/IWLListItem.h"
#include "uilib/common/utils/ImageEx/Shared_ImageEx.h"

typedef struct _tagNMVISIBLERANGE 
{
	NMWLHDR hdr ;
	unsigned int fromIndex ;
	unsigned int toIndex ;
} NMVISIBLERANGE , * LPNMVISIBLERANGE  ;

#define SRLN_VISIBLE_ITEM_CHANGE (NM_WLFIRST - 200U)

class CSearchResultItem :
	public WLSimpleListItem
{
public:
	CSearchResultItem();
	virtual ~CSearchResultItem(void);


public:
	void SetFileFullPath(LPCTSTR path);
	void SetIcon(HICON icon);
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
	Shared_ImageEx m_pImgHot;
	Shared_ImageEx m_pImgSel;
	int m_nMarginLeft;

private:
	//file info
	CString m_fullpath;
	CString m_filename;
	UINT64 m_size;
	CTime m_lastmodify;
	HICON m_hIcon;
};


class CSearchResultList :
	public WLSimpleList, public IWLScrollWnd::ScrollListener
{
public:
	CSearchResultList(void);
	virtual ~CSearchResultList(void);
	void Init();
	int Insert(LPCTSTR fullpath);
	bool Delete(unsigned int index);
	void DeleteAll();

	LPCTSTR GetItemText(unsigned int index);

	//IWLScrollWnd::ScrollListener
	virtual void OnContentPositionXChanged(int /*old_x*/, int /*new_x*/);
	virtual void OnContentPositionYChanged(int /*old_y*/, int /*new_y*/);
	virtual void OnViewportWidthChanged(int /*old_width*/, int /*new_width*/);
	virtual void OnViewportHeightChanged(int /*old_height*/, int /*new_height*/);

	CSearchResultItem *SearchVisibleItem(LPCTSTR fullpath);

protected:
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult);
private:

	void OnVisibleItemChanges();

	Shared_ImageEx m_pImgHot;
	Shared_ImageEx m_pImgSel;
	int m_cpx;
	int m_cpy;
	int m_vpw;
	int m_vph;

};

