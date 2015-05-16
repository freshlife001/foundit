#pragma once


class CDragTabImage : public CWnd, public COleDropSource
{
	DECLARE_DYNAMIC(CDragTabImage)

public:
	CDragTabImage();
	virtual ~CDragTabImage();

	BOOL Create(
		CDC * pDC,
		CRect const &rcInDC,			// 所截取的内容在dc中的范围
		CWnd * pParent,
		CPoint const &ptInScreen,		// 所截取的内容在屏幕中的位置
		CPoint ptDragInScreen			// 鼠标拖拽点在屏幕中的位置
		) ;

	BOOL Create(
		CBitmap* pBmp, // 拖动图标的内容
		CSize const &szItem,			// 所截取的内容在dc中的范围
		CWnd * pParent,
		CPoint const &ptInScreen,		// 所截取的内容在屏幕中的位置
		CPoint ptDragInScreen			// 鼠标拖拽点在屏幕中的位置
		) ;

	void DoDragging(CPoint ptDrag) ;
	void EndDrag() ;

	virtual SCODE QueryContinueDrag(BOOL bEscapePressed, DWORD dwKeyState);
	virtual SCODE GiveFeedback(DROPEFFECT dropEffect);

protected:
	CSize m_szOffset ;

	CDC * m_pdcImage ;
	CBitmap * m_pOldImageBmp ;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


