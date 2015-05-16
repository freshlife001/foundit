//////////////////////////////////////////////////////////////////////////
//
//	描述：	气泡提示控件
//	
//	日期：	06/10/2009
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLTIPBALLOON_H__9E1D0B54_B238_4DA3_8402_D029CEF7244A__INCLUDED_)
#define AFX_TOOLTIPBALLOON_H__9E1D0B54_B238_4DA3_8402_D029CEF7244A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolTipBallon.h : header file
//

#include "ToolTipBaseST.h"

/////////////////////////////////////////////////////////////////////////////
// CToolTipBalloon window

class CToolTipBalloon : public CToolTipBaseST
{
// Construction
public:
	CToolTipBalloon();
	virtual ~CToolTipBalloon();

// Attributes
public:
	// 设置、获取宽度
	void SetWidth(int nWidth) ;
	int GetWidth() const ;

	// 设置、获取嘴巴长度、宽度、位移
	void SetMouthLength(int nLength) ;
	int GetMouthLength() const ;
	void SetMouthWidth(int nWidth) ;
	int GetMouthWidth() const ;
	void SetMouthBeginOffset(int nOffset) ;
	int GetMouthBeginOffset() const ;
	void SetMouthEndOffset(int nOffset) ;
	int GetMouthEndOffset() const ;
	
	// 设置、获取关闭按钮大小
	void SetCloseBtnSize(int cx, int cy) ;
	CSize GetCloseBtnSize() const ;

	// 设置、获取圆边直径
	void SetRoundDia(int nRoundDia) ;
	int GetRoundDia() const ;

	// 设置边缘空白宽度
	void SetMargin(int nMargin) ;
	int GetMargin() const ;
	
	// 设置、获取边框颜色
	void SetFrameColor(COLORREF crFrame) ;
	COLORREF GetFrameColor() const ;
	
	// 设置、获取气泡是否置顶
	void SetTopMost(BOOL bTopMost = TRUE) ;
	BOOL IsTopMost() const ;
	
	// 设置、获取是否易碎
	void SetFriable(BOOL bFriable = TRUE) ;
	BOOL GetFriable() const ;
	
	// 设置冒泡方向优先级
	void SetDirPRI(UINT n0, UINT n1 = 0, UINT n2 = 0, UINT n3 = 0) ;

protected:
	CPoint	m_ptBalloon ;		// 气泡位置
	CRgn	m_rgnBolloon ;		// 气泡区域

	CPoint	m_ptMouthPos ;		// 气泡嘴巴位置

	int		m_nMouthBeginOffset ;	// 嘴巴起点位移
	int		m_nMouthEndOffset ;		// 嘴巴终点位移
	int		m_nMouthLength ;		// 嘴巴长度
	int		m_nMouthWidth ;			// 嘴巴宽度

	int		m_nRoundDia ;		// 圆边直径

	int		m_nMargin ;			// 边缘宽度

	CPoint	m_ptText ;			// 文字位置
	CSize	m_szText ;			// 文字大小

	int		m_nBalloonHeight ;	// 气泡高度(随文本变化)
	int		m_nBalloonWidth ;	// 气泡宽度(固定)

	int		m_nTotalHeight ;	// 总高度
	int		m_nTotalWidth ;		// 总宽度

	int		m_nBtnMargin ;		// 关闭按钮和边缘之间的空隙
	CPoint	m_ptCloseBtn ;		// 关闭按钮的位置
	CSize	m_szCloseBtn ;		// 关闭按钮的大小(固定)
	
	COLORREF m_crFrame ;		// 边框颜色

	BOOL	m_bTopMost ;		// 是否置顶
	BOOL	m_bFriable ;		// 是否易碎

	CString m_strText ;			// 显示的文本
	
	enum CLOSEBTN_MODE
	{
		CBM_NORMAL = 0,
		CBM_HOVER,
		CBM_PUSH
	} m_nCloseBtnMode ;			// 关闭按钮当前模式
	BOOL	m_bCloseBtnDown	;	// 关闭按钮是否按下

	UINT	m_uDuration ;		// 持续时间
	UINT	m_uLastTime ;		// 剩余时间
	UINT	m_nTimePiece ;		// 时间片
	
	UINT	m_arnDirPRI[4] ;	// 方向优先级数组

// Operations
public:
	// 弹出气泡提示
	BOOL Pop(CPoint ptPos, CString const &strText, CWnd * pParent, UINT nDuration = ~0) ;
	BOOL Pop(CPoint ptPos, CString const &strText, HWND hParent, UINT nDuration = ~0) ;
	// 压入气泡提示
	void Push() ;

protected:
	// 计算各相关区域
	void CalcPostion() ;

	// 绘制关闭按钮
	void DrawCloseBtn(CDC * pDC, CRect rect, CLOSEBTN_MODE nMode) ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTipBalloon)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTipBalloon)
	afx_msg HFONT OnGetFont(WPARAM, LPARAM) ;
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTIPBALLON_H__9E1D0B54_B238_4DA3_8402_D029CEF7244A__INCLUDED_)
