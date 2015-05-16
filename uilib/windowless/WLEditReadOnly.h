//////////////////////////////////////////////////////////////////////////
//
//	描述:   编辑框
//
//	
//	日期:   2010/12/25
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "IWLEditReadOnly.h"
#include <vector>

class WLEditReadOnlyImp :
	public WLEditReadOnlyImpBase
{
public:
	WLEditReadOnlyImp(void);
	virtual ~WLEditReadOnlyImp(void);

	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
public:
	virtual BOOL Create(DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent, UINT uID) ;
	virtual BOOL SetWindowText(LPCTSTR lpszText) ;

protected:
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;

	void CalculateCharaterRect();

	//////////////////////////////////////////////////////////////////////////
	// WLEditReadOnly Implement
	virtual void OnDrawBkgnd(HDC hDC, RECT const &rcInHost, RECT const &rcUpdate) ;

// Operations
public:
	void SetMargin(
		int nLeftMargin,
		int nTopMargin,
		int nRightMargin,
		int nBottomMargin
		) ;
	void GetMargin(LPRECT lprcMargin) const;

	void SetSel(int nStartChar, int nEndChar) ;
	void LimitText(int nChars) ;
	BOOL IsEmpty() const ;

	Color SetBkgndColor(Color crBkgndColor) ;

	COLORREF SetTextNormalColor(COLORREF crTextNormal) ;

	void SetRowInterval(int nRowInterval) { m_nRowInterval = nRowInterval ; }

	void Copy() ;
protected:
	//////////////////////////////////////////////////////////////////////////
	// Inner Function

	// bForce为TRUE时强制替换，在SetWindowText时使用


	int HitTestCaretPos(CPoint point) ;
	int GetFirstVisiblePos() const ;
	void DeleteSelNoChangeOriginX() ;
	void ProcessMenuCommand(int nMenuID) ;
	//void DrawBkgnd(HDC hDC, RECT const &rcInHost, RECT const &rcUpdate) ;
	void DrawHighlightTextBkgnd(HDC hDC, RECT const &rcBkgnd) ;
	void SendChangeCommand() ;

	//////////////////////////////////////////////////////////////////////////
	// Message Handler

	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point) ;
	void OnRButtonUp(UINT nFlags, CPoint point) ;

	void OnSetFocus(HWL pOldWL);
	void OnKillFocus(HWL pNewWL);

	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	UINT OnGetDlgCode();
	//void OnTimer(UINT_PTR nIDEvent);
	BOOL OnSetCursor(UINT message);
	void OnCaptureChanged(HWL hWl) ;

private:
		bool isSelected( int nIndex );

// Data
protected:
	BOOL m_bFocus ;

	int m_nCaretPos ;
	BOOL m_bShowCaret ;

	int m_nLeftMargin ;
	int m_nTopMargin ;
	int m_nRightMargin ;
	int m_nBottomMargin ;

	int m_nFixSel ;
	int m_nLeftSel ;
	int m_nRightSel ;

	BOOL m_bLDown ;
	BOOL m_bRDown ;
	BOOL m_bBeginSel ;

	BOOL m_bPopMenu ;		// 标记当前是否弹出菜单，弹出菜单时，光标显示成箭头

	CRect m_rcTextBound ;

	int m_nMaxChars ;

	// Menu
	enum
	{
		IDM_COPY		= 49,
		IDM_SELECTALL	= 52,
	};

	COLORREF m_crTextNormalColor ;		// 文字普通颜色
	COLORREF m_crTextHighlightColor ;	// 文字高亮颜色
	Color m_crTextHighlightBkColor ;	// 文字高亮背景颜色(需要Alpha通道)

	Color m_crBkgndColor ;				// 背景颜色(需要Alpha通道)

private:
	std::vector<CRect> m_vecCharacterRect;
	std::vector<int> m_vecRowsHeight;

	int m_nRowInterval;

};
