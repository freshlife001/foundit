//////////////////////////////////////////////////////////////////////////
//
//	描述:   组合框控件中的列表控件
//
//	
//	日期:   2011/02/17
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#define LCIS_NORMAL			0			// 普通
#define LCIS_SUBHOVERED		1			// 次悬浮
#define LCIS_PRIHOVERED		2			// 主悬浮

#define LCIBS_HIDE			0			// 隐藏
#define LCIBS_NORMAL		1			// 显示
#define LCIBS_HOVERED		2			// 鼠标悬浮
#define LCIBS_PUSHED		3			// 鼠标按下

struct LCITEM 
{
	// 默认构造函数
	LCITEM(LPCTSTR lpszText)
		: m_strText(lpszText), m_uState(LCIS_NORMAL), m_uBtnState(LCIBS_NORMAL)
	{

	}

	// 数据
	CString m_strText ;			// 文本

	// 属性
	int		m_nPosition ;		// 相对原点位置
	int		m_nHeight ;			// 高度
	int		m_nCurHeight ;		// 当前高度
	UINT	m_uState ;			// 状态

	// 删除按钮属性
	UINT	m_uBtnState ;		// 鼠标状态
};

typedef std::vector<LCITEM>	VEC_LCITEM ;

// CCmbListCtrl
#define ST_CMBLISTCTRL	_T("St.CmbListCtrl")

class CCmbListCtrl : public CWnd
{
	DECLARE_DYNAMIC(CCmbListCtrl)

public:
	CCmbListCtrl();
	virtual ~CCmbListCtrl();

public:
	enum OPERATION_TYPE
	{
		EXITSEL	= 0,
		SELITEM	= 1,
		DELITEM	= 2,
	};

	typedef struct _tagRESDATA
	{
		OPERATION_TYPE ot ;
		int nData ;				// 表示选择的节点
	}RESDATA, LPRESDATA;

	int Popup(
		int x, int y, int cx, int cy,
		HWND hParent,
		RESDATA &res
		) ;

	void LoadImage(
		Image * pImgDelBtn
		) ;

	// 添加字符串
	int AddString(LPCTSTR lpszString) ;

	// 跟踪子项
	int SetCurSel(int nIndex) ;

	// 设置子项高度
	void SetItemHeight(int nHeight) ;

protected:
	// 判断子项是否可见
	BOOL IsItemVisible(UINT nIndex) ;

	// 自适应大小
	void AutoAdjustSize() ;

	// 点击测试
	int HitTest(CPoint point) ;

	// 判断子项是否可见
	BOOL IsItemVisible(LCITEM const & item) ;

	// 绘制子项
	void DrawItem(CDC * pDC, LCITEM const & item) ;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);

	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//afx_msg void OnCancelMode();

protected:
	RESDATA * m_pResData ;

	CRect		m_rcClient ;
	COLORREF	m_crFrame ;					// 边框颜色

	CFont		m_ftFontNormal ;			// 正常情况下子项字体
	COLORREF	m_crTextColorNormal ;		// 正常情况下子项字体颜色
	COLORREF	m_crTextColorHovered ;		// 鼠标滑过情况下子项字体颜色

	COLORREF	m_crBackColorNormal ;		// 正常情况下子项背景颜色
	COLORREF	m_crBackColorHovered ;		// 鼠标滑过情况下子项背景颜色

	int			m_nItemHeightNormal ;		// 正常情况下子项高度
	int			m_nItemPosOrigin ;			// 子项位置原点
	int			m_nTrackItemIndex ;			// 被跟踪子项索引
	int			m_nTextOffsetLeft ;			// 子项文本左偏移
	int			m_nBtnOffsetRight ;			// 删除按钮右偏移
	int			m_nBtnWidth ;				// 删除按钮宽度
	int			m_nBtnHeight ;				// 删除按钮高度

	int			m_nBtnPushedItem ;			// 当前鼠标按下的子项索引
	BOOL		m_bBtnPushed ;				// 标记当前鼠标是否按下
	CRect		m_rcCurrentBtn ;			// 当前按下按钮的位置范围
	Image *		m_pImgDelBtn ;

	// 标记是否开始响应左键弹起消息
	// 当按下组合框下拉按钮后，如果用户立即弹起鼠标左键，则不做任何操作，
	// 如果用户将鼠标移至列表上再弹起左键，则选择该项
	BOOL		m_bBeginHandleLBtnUp ;

	// 子项数组
	VEC_LCITEM	m_vecItems ;

};


