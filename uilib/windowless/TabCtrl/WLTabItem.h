#pragma once
#include "uilib\windowless\WndLess.h"
#include "common/utils/ImageEx/Shared_ImageEx.h"

class WLTabItem : public CWndLessImp
{
public:
    enum ItemState
    {
        TISTATE_NORMAL = 0,
        TISTATE_HOT,
        TISTATE_SELECTED,
        TISTATE_DISABLED,
    };

    enum ItemTextStyle
    {
        TITS_LEFT = 0x00000000,
        TITS_TOP = 0x00000000,
        TITS_CENTER = 0x00000001,
        TITS_RIGHT = 0x00000002,
        TITS_BOTTOM = 0x00000002,
        TITS_VERICAL = 0x00000004,
        TITS_SPECIFY = 0x00000030,
        TITS_X_SPECIFY = 0x00000010,
        TITS_Y_SPECIFY = 0x00000020,
    };
    enum
    {
        TCN_TAB_ITEM_DRAG_MOVE = TCN_FIRST - 20,
        TCN_TAB_ITEM_LUP = TCN_FIRST - 21,
    };


	WLTabItem(void);
	WLTabItem( LPCTSTR lpszText, 
		UINT uTextStyle,
	CPoint	ptText,
	LPCTSTR sTipText,
	Shared_ImageEx pImgIcon,
	Shared_ImageEx pImgBkgnd,
	BOOL bDrag,
	int nTabStateCount );
	virtual ~WLTabItem(void);

	// CWndLess Implementation
	void SetItemText(LPCTSTR lpszText) ;

	void SetItemData( DWORD_PTR dwData );

	void SetIconSize( const CSize& size );

	void SetIcon( Shared_ImageEx icon );

	void SetTextColor( COLORREF	 crText );

	void GetDragOffset( CSize& offDrag );
	void SetDragOffset( const CSize& szOff );

	BOOL HightLight();

	virtual UINT GetMinimizeSize( );

	DWORD_PTR GetItemData() const;

	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;

	void GetDrawRect( CRect &rcInHost );

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

	void UpdateMovedPosition( CPoint &pt );

	void UpdateDragOffset( CPoint pt );

	// WLTabItem Implementation
	void SetIndex(int nIndex) ;
	int GetIndex() const ;
	void SetPrevItem(WLTabItem * pPrevItem) ;
	WLTabItem * GetPrevItem() const ;
	void SetNextItem(WLTabItem * pNextItem) ;
	WLTabItem * GetNextItem() const ;
	virtual void RecalcLayout(BOOL bNotifyParent) ;					// 根据内容调整大小位置及内部布局

protected:
	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent );

	virtual void GetIconTextDrawRect( CRect& rcInHost );

public:
	// 消息
	enum 
	{
		TIM_SELECTED	= WM_USER + 100,
		TIM_UNSELECTED	= WM_USER + 101,
		TIM_LDOWN,
		TIM_LUP,
	};

protected:
	int m_nIndex ;
	WLTabItem * m_pPrevItem ;
	WLTabItem * m_pNextItem ;

	// 数据
	UINT m_uTextStyle ;
	CPoint	m_ptText ;
	CString m_sTipText ;
	Shared_ImageEx m_pImgIcon ;
	CSize m_szIcon;
	Shared_ImageEx m_pImgBkgnd ;
	int m_ntabStateCount;

	COLORREF	m_crText ;

	// 拖拽相关
	BOOL m_bCanDrag;
	CSize m_ptDragOffset; // 点击距离topleft的位置
	CPoint m_ptCurrent;

	// 数据
	DWORD_PTR m_dwData ;

	// 状态
	enum 
	{
		TIS_NORMAL		= 0x00000000,
		TIS_HOT			= 0x00000001,
		TIS_SELECTED	= 0x00000002,
		TIS_DISABLE		= 0x00000004,
		TIS_HIGHTLIGHT = 0x00000008,
		TIS_LDOWN		= 0x00010000,
		TIS_RDOWN		= 0x00020000,
	};
	DWORD m_dwStatus ;
};
