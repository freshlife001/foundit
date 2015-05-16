#pragma once

#include "IWndLess.h"


// WM_NOTIFY 
typedef struct tagNMWLTABITEM
{
	NMWLHDR hdr ;
	int	nOldItem ;
	DWORD_PTR dwOldItemData ;

	int nNewItem ;
	DWORD_PTR dwNewItemData ;
}NMWLTABITEM, * LPNMWLTABITEM;

typedef struct tagWLTabNotifyData
{
	NMWLHDR		hdr;
	LONG		nIdx;
} WLTabNotifyData;

class IWLTabCtrl :  virtual public IWndLess
{
public:
	virtual ~IWLTabCtrl(void){};
	// Tab Style(可直接添加到窗口风格中)

	enum 
	{
		TS_LEFT					=	0x00000000,
		TS_TOP					=	0x00000000,
		TS_RIGHT				=	0x00000001,
		TS_BOTTOM				=	0x00000001,
		TS_VERTICAL				=	0x00000002,
		TS_FIXBOUND				=	0x00000000,		// 固定大小(默认)
		TS_AUTOADJUSTBOUND		=	0x00000004,		// 自动调整大小(平均大小)
	};

	// 设置背景图片
	virtual void SetImage(Image * pImgBkgnd, BOOL bRedraw = FALSE) = 0;

	// 设置子项间隔
	virtual void SetSpace(int nNearSpace, int nItemSpace, int nFarSpace)  = 0;

	// 设置文字颜色
	virtual void SetTextColor(COLORREF crText, BOOL bRedraw = FALSE)  = 0;

	// 设置背景颜色
	virtual void SetBkgndColor(COLORREF crBkgnd, BOOL bRedraw = FALSE)  = 0;

	// 设置选中文字颜色
	virtual void SetSelectTextColor( COLORREF crText, BOOL bRedraw  ) = 0;

	// 设置是否可以获取焦点
	virtual void SetFocusable(BOOL bFocusable)  = 0;

	// 插入子项
	virtual int InsertItem(
		int nIndex,
		Shared_ImageEx pImgIcon,
		Shared_ImageEx pImgBkgnd,
		int nTabStateCount = 3,
		LPCTSTR lpszText = NULL,
		UINT uTextStyle = 0,
		int nTextX = 0,
		int nTextY = 0,
		LPCTSTR lpszTip = NULL,
		int maxWidth = 0
		)  = 0 ;

	// 设置子项文本
	virtual BOOL SetItemText(int nIndex, LPCTSTR lpszText) = 0 ;

	// 设置子项数据
	virtual BOOL SetItemData(int nIndex, DWORD_PTR dwData) = 0 ;

	// 获取子项数据
	virtual DWORD_PTR GetItemData(int nIndex) const = 0 ;

	// 删除子项
	virtual BOOL DeleteItem(int nIndex, BOOL bResize = TRUE ) = 0 ;

	// 选择子项
	virtual BOOL SelectItem(int nIndex) = 0 ;

	virtual BOOL HightLightItem( int nIndex ) = 0 ;

	// 获得子项区域
	virtual BOOL GetItemRect(int nIndex, CRect &rcItem) = 0 ;

	virtual BOOL SetItemIconSize( int nIndex, CSize& size ) = 0 ; 

	// 获得当前选择的子项索引
	virtual int GetCurSel() const = 0 ;

	// 获得子项数量
	virtual int GetItemCount() const = 0 ;

};

typedef CWndLessImpBase< CWndLessShell<IWLTabCtrl> >  WLTabCtrlImpBase;

template <class IT = IWLTabCtrl, class IMPT = WLTabCtrlImpBase > 
class CWLTabCtrlShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLTabCtrlShell() {}
	virtual ~CWLTabCtrlShell(){}

	// 设置背景图片
	virtual void SetImage(Image * pImgBkgnd, BOOL bRedraw = FALSE)	
	{
		if (m_wlimp)
		{
			m_wlimp->SetImage( pImgBkgnd, bRedraw );
		}
		return;
	};

	// 设置子项间隔
	virtual void SetSpace(int nNearSpace, int nItemSpace, int nFarSpace)	
	{
		if (m_wlimp)
		{
			m_wlimp->SetSpace( nNearSpace, nItemSpace, nFarSpace );
		}
		return;
	};

	// 设置文字颜色
	virtual void SetTextColor(COLORREF crText, BOOL bRedraw = FALSE)	
	{
		if (m_wlimp)
		{
			m_wlimp->SetTextColor( crText, bRedraw );
		}
		return;
	};

	// 设置背景颜色
	virtual void SetBkgndColor(COLORREF crBkgnd, BOOL bRedraw = FALSE)	
	{
		if (m_wlimp)
		{
			m_wlimp->SetBkgndColor( crBkgnd, bRedraw );
		}
		return;
	};

	// 设置是否可以获取焦点
	virtual void SetFocusable(BOOL bFocusable)	
	{
		if (m_wlimp)
		{
			m_wlimp->SetFocusable( bFocusable );
		}
		return;
	};

	virtual void SetSelectTextColor( COLORREF crText, BOOL bRedraw  )
	{
		if (m_wlimp)
		{
			m_wlimp->SetSelectTextColor( crText, bRedraw );
		}
		return;
	};

	// 插入子项
	virtual int InsertItem(
		int nIndex,
		Shared_ImageEx pImgIcon,
		Shared_ImageEx pImgBkgnd,
		int nTabStateCount,
		LPCTSTR lpszText,
		UINT uTextStyle,
		int nTextX,
		int nTextY,
		LPCTSTR lpszTip,
		int maxWidth
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->InsertItem( nIndex, pImgIcon, pImgBkgnd,nTabStateCount,  lpszText,uTextStyle, nTextX, nTextY, lpszTip, maxWidth);
		}
		return -1;
	};

	// 设置子项文本
	virtual BOOL SetItemText(int nIndex, LPCTSTR lpszText)	
	{
		if (m_wlimp)
		{
			return m_wlimp->SetItemText( nIndex, lpszText);
		}
		return FALSE;
	};

	// 设置子项数据
	virtual BOOL SetItemData(int nIndex, DWORD_PTR dwData)	
	{
		if (m_wlimp)
		{
			return m_wlimp->SetItemData( nIndex, dwData);
		}
		return FALSE;
	};

	// 获取子项数据
	virtual DWORD_PTR GetItemData(int nIndex) const	
	{
		if (m_wlimp)
		{
			return m_wlimp->GetItemData( nIndex);
		}
		return NULL;
	};

	// 删除子项
	virtual BOOL DeleteItem(int nIndex, BOOL bResize = TRUE )	
	{
		if (m_wlimp)
		{
			return m_wlimp->DeleteItem( nIndex, bResize);
		}
		return FALSE;
	};

	// 选择子项
	virtual BOOL SelectItem(int nIndex)	
	{
		if (m_wlimp)
		{
			return m_wlimp->SelectItem( nIndex);
		}
		return FALSE;
	};

	virtual BOOL HightLightItem( int nIndex )	
	{
		if (m_wlimp)
		{
			return m_wlimp->HightLightItem( nIndex);
		}
		return FALSE;
	};

	// 获得子项区域
	virtual BOOL GetItemRect(int nIndex, CRect &rcItem)	
	{
		if (m_wlimp)
		{
			return m_wlimp->GetItemRect( nIndex, rcItem);
		}
		return FALSE;
	};

	virtual BOOL SetItemIconSize( int nIndex, CSize& size )	
	{
		if (m_wlimp)
		{
			return m_wlimp->SetItemIconSize( nIndex, size);
		}
		return FALSE;
	};

	// 获得当前选择的子项索引
	virtual int GetCurSel() const	
	{
		if (m_wlimp)
		{
			return m_wlimp->GetCurSel();
		}
		return -1;
	};


	// 获得子项数量
	virtual int GetItemCount() const	
	{
		if (m_wlimp)
		{
			return m_wlimp->GetItemCount();
		}
		return 0;
	};
	
};

typedef CWLTabCtrlShell<> WLTabCtrl;



