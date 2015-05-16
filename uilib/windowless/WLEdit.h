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
#include "wndless.h"
#include <vector>
#include "IWLEdit.h"
#include "common/controls/ToolTip/ToolTipBalloon.h"

class WLEditImp :
	public WLEditImpBase
{
public:
	WLEditImp(void);
	virtual ~WLEditImp(void);

	enum STYLE
	{
		WLES_NOHIDESEL	= 0x00000001,
	};

	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
public:
	virtual BOOL Create(DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent, UINT uID) ;
	virtual BOOL SetWindowText(LPCTSTR lpszText) ;

	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;

	//////////////////////////////////////////////////////////////////////////
	// WLEdit Implement
	virtual void OnDrawBkgnd(HDC hDC, RECT const &rcInHost, RECT const &rcUpdate) ;

// Operations
public:
	UINT SetFrameWidth(UINT uNewFrameWidth) ;
	void SetMargin(
		int nLeftMargin,
		int nTopMargin,
		int nRightMargin,
		int nBottomMargin
		) ;
	void GetMargin(LPRECT lprcMargin) const
	{
		if (lprcMargin == NULL)
		{
			ASSERT(FALSE) ;
			return ;
		}
		::SetRect(
			lprcMargin,
			m_nLeftMargin, m_nTopMargin, m_nRightMargin, m_nBottomMargin
			) ;
	}

	void SetCueBanner(LPCTSTR lpszText) ;
	CString GetCueBanner() const ;

	void ReplaceSel(LPCTSTR lpszNewText) ;
	void SetSel(int nStartChar, int nEndChar) ;
	void LimitText(int nChars) ;
	BOOL CanAutoCompleteWord() const { return m_bCanAutoCompleteWord ; }
	BOOL IsEmpty() const { CString text;GetWindowText(text); return text.IsEmpty();}

	COLORREF SetFrameColor(COLORREF crFrame) ;
	Color SetBkgndColor(Color crBkgndColor) ;

	COLORREF SetTextNormalColor(COLORREF crTextNormal) ;

	void Undo() ;
	void Cut() ;
	void Copy() ;
	void Paste() ;
	void Clear() ;

	// 获得刚好存放文字时的大小
	void CalcWrapContentSize(LPSIZE lpSize) ;

protected:
	//////////////////////////////////////////////////////////////////////////
	// Inner Function

	// bForce为TRUE时强制替换，在SetWindowText时使用
	void _ReplaceSel(LPCTSTR lpszNewText, BOOL bForce) ;

	int HitTestCaretPos(CPoint point) ;
	int GetFirstVisiblePos() const ;
	void DeleteSelNoChangeOriginX() ;
	void ProcessMenuCommand(int nMenuID) ;
	//void DrawBkgnd(HDC hDC, RECT const &rcInHost, RECT const &rcUpdate) ;
	void DrawHighlightTextBkgnd(HDC hDC, RECT const &rcBkgnd) ;
	void SendChangeCommand() ;

	BOOL IsPassword() const { return GetStyle() & ES_PASSWORD ; }
	BOOL IsReadOnly() const { return GetStyle() & ES_READONLY ; }

	// 获得光标下端位置(客户区坐标系)
	void GetCaretPos(LPPOINT lpptPos) const ;

	// 更新输入法窗口位置
	void UpdateImeWndPosByCaret() const ;

	// 弹出大写提示气泡
	void PopCapitalBallonTip() ;

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
	void OnTimer(UINT_PTR nIDEvent);
	BOOL OnSetCursor(UINT message);
	void OnCaptureChanged(HWL hWl) ;

// Data
protected:
	BOOL m_bFocus ;

	int m_nCaretPos ;
	BOOL m_bShowCaret ;
	BOOL m_bInputting ;

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
	int m_nLineHeight ;

	int m_nOriginPosX ;
	typedef std::vector<int> VCT_INT ;
	VCT_INT m_vctCaretPos ;
	int m_nMaxChars ;

	// Menu
	enum
	{
		IDM_UNDO		= 47,
		IDM_CUT			= 48,
		IDM_COPY		= 49,
		IDM_PASTE		= 50,
		IDM_DELETE		= 51,
		IDM_SELECTALL	= 52,
	};

	UINT m_uFrameWidth ;				// 边框宽度
	COLORREF m_crFrameColor ;			// 边框颜色

	COLORREF m_crTextNormalColor ;		// 文字普通颜色
	COLORREF m_crTextHighlightColor ;	// 文字高亮颜色
	Color m_crTextHighlightBkColor ;	// 文字高亮背景颜色(需要Alpha通道)

	Color m_crBkgndColor ;				// 背景颜色(需要Alpha通道)

	CString m_sCueBanner ;				// 提示文字
	COLORREF m_crCurBannerColor ;		// 提示文字颜色
	TCHAR m_cPasswordChar ;				// 密码显示字符

	BOOL m_bCanAutoCompleteWord ;		// 标记“允许自动完成单词”状态

	CToolTipBalloon m_ttb ;

};
