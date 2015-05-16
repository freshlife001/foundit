#pragma once

#include "common\windowless\WndLess.h"
#include "common\utils\CanvasDC.h"
#include "common/windowless/WLButton.h"
#include "common\utils\ImageEx\Shared_ImageEx.h"

class IOptionListener
{
public:
	virtual void OnSelectTileMode() = 0 ;
	virtual void OnSelectSingleMode() = 0 ;
	virtual void OnOK() = 0 ;
	virtual void OnCancel() = 0 ;
};

class WLSkinOption :
	public CWndLess
{
public:
	WLSkinOption(void);
	~WLSkinOption(void);

	void SetListener(IOptionListener * listener) { m_listener = listener ; }

protected:
	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

private:
	void InitBkgnd() ;
	void InitImage() ;

	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	BOOL OnCommand(WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;

private:
	CCanvasDC m_bkgnd_dc ;

	Shared_ImageEx m_img_tile_mode ;
	Shared_ImageEx m_img_single_mode ;
	Shared_ImageEx m_img_selected_border ;
	Shared_ImageEx m_img_push_btn ;

	enum ControlId
	{
		Id_Btn_TileMode = 2012,
		Id_Btn_SingleMode,
		Id_Btn_OK,
		Id_Btn_Cancel,
	};

	WLButton m_btn_tile_mode ;
	WLButton m_btn_single_mode ;
	WLButton m_btn_ok ;
	WLButton m_btn_cancel ;

	BOOL m_single_or_tile ;

	IOptionListener * m_listener ;

};
