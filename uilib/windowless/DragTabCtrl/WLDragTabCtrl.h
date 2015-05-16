#pragma once
#include "common/utils/ImageEx/Shared_ImageEx.h"

#include "windowless/TabCtrl/WLTabCtrl.h"
#include "WLTabDropTarget.h"
#include "WLDragTabItem.h"

#include "IWLDragTabCtrl.h"
#include "IWLButtonAL.h"


class CDragTabImage;
class WLDragTabCtrlImp : public WLTabCtrlImp<IWLDragTabCtrl>
{
	enum // 控件ID
	{
		CTRL_ID_BTN_ADD = 100,
	};


public:
	WLDragTabCtrlImp( BOOL bCanDrag );
	virtual ~WLDragTabCtrlImp(void);

	void SetAddBtnSpace( int nAddBtnSpace );
	void SetAddBtnTipString( const CString& tipString );

	void SetTabItemMinSize( int nMinSize );

	UINT GetMinimizeSize( );

	virtual BOOL OnCreate(
		DWORD dwStyle, 
		int x, int y, int cx, int cy,
		IWndLess * pParent
		) ;

public:
	DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);

protected:
	virtual void RecalcLayout() ;

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult);

	virtual void OnDragItemMove( LPNMWLHDR lpNMWLHDR );
	virtual void OnLDownItem( LPNMWLHDR lpNMWLHDR) ;
	virtual void OnDestroy();

	virtual WLTabItem* CreateTabItem( LPCTSTR lpszText, 
		UINT uTextStyle,
		CPoint	ptText,
		LPCTSTR sTipText,
		Shared_ImageEx pImgIcon,
		Shared_ImageEx pImgBkgnd,
		BOOL bDrag,
		int nTabState );

	virtual void RepositionAfterDeleteItem( BOOL bResize, int nIndex, CSize &delItemSize );

private:
	BOOL m_bBeginDrag ;				// 开始拖拽
	BOOL m_bDragging ;				// 正在拖拽
	//CPoint m_ptPreProcPoint;
	//CSize m_szDragOffset;

	WLTabDropTarget m_dropTarget;

	static WORD s_uDragTabClipboardFormat;

	CBitmap& GetDragBitmap( UINT uDragIndex, CBitmap& bmp );

	WLButtonAL* m_btnAdd;
	Shared_ImageEx m_imgAdd;
	int m_nAddBtnSpace;

	int m_nTabItemMinSize;

private:
	void BeginDrag( const CPoint& ptDrag );
	//void CheckOrder( const CPoint& ptCurrent );
};
