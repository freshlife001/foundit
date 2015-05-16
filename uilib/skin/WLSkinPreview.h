#pragma once

#include "common\windowless\wndlessroot.h"
#include "common\utils\CanvasDC.h"

#include "WLWindowFace.h"
#include "WLSkinOption.h"

class CustomImageSkin ;
class SkinPreviewListener ;

class WLSkinPreview :
	public CWndLessRoot,
	public IFaceListener,
	public IOptionListener
{
public:
	WLSkinPreview(void);
	virtual ~WLSkinPreview(void);

	CCanvasDC & GetFace() ;
	void SetSkin(CustomImageSkin * skin) ;
	void SetListener(SkinPreviewListener * listener) { m_listener = listener ; }

protected:
	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual void OnSized(SIZE const &szOld, SIZE const &szNew) ;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

	//////////////////////////////////////////////////////////////////////////
	// IFaceListener Interface
	virtual void OnFaceMove() ;

	//////////////////////////////////////////////////////////////////////////
	// IOptionListener Interface
	virtual void OnSelectTileMode() ;
	virtual void OnSelectSingleMode() ;
	virtual void OnOK() ;
	virtual void OnCancel() ;

	//////////////////////////////////////////////////////////////////////////
	// Inner Function
protected:
	void BeginDrag(POINT const &point) ;
	void DoDrag(POINT const &point) ;
	void EndDrag() ;

	void RecalcLayout() ;		// 调整选项面板的位置

protected:
	WLWindowFace m_face ;
	WLSkinOption m_option ;

	POINT m_bkgnd_orgin_pos ;

	CustomImageSkin * m_skin ;
	SkinPreviewListener * m_listener ;

	BOOL m_dragging ;
	POINT m_drag_point ;

};
