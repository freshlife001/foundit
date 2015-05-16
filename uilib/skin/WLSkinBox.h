#pragma once

#include "common\windowless\WndLess.h"
#include "common\utils\ImageEx\Shared_ImageEx.h"

class ISkin ;
class WLSkinBox ;

class SkinBoxListener
{
public:
	virtual void OnSelectedSkinBox(WLSkinBox * skin_box) = 0 ;
	virtual void OnDblClickSkinBox(WLSkinBox * skin_box) = 0 ;
};

class WLSkinBox :
	public CWndLess
{
public:
	WLSkinBox(void);
	virtual ~WLSkinBox(void);

	enum SkinBoxType
	{
		ImageSkin = 0, ColorSkin = 1,
	};

public:
	// 该接口在创建窗口前调用
	void SetSkinBoxType(SkinBoxType skin_box_type) { m_skin_box_type = skin_box_type ; }

	// 该接口在创建窗口后调用
	void SetSkin(ISkin * skin) ;

	ISkin * GetSkin() const { return m_skin ; }

	// 选择
	void Select(BOOL select) ;

	BOOL IsSelected() const { return m_selected ; }

	void SetListener(SkinBoxListener * listener) { m_listener = listener ; }

protected:
	//////////////////////////////////////////////////////////////////////////
	// IWndLess interface
	virtual BOOL OnCreate( DWORD dwStyle, int x, int y, int cx, int cy, IWndLess * pParent ) ;
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;

protected:
	SkinBoxType m_skin_box_type ;

	Shared_ImageEx m_img_bkgnd ;
	Shared_ImageEx m_img_border ;

	int m_box_width ;
	int m_box_height ;
	int m_border_width ;

	ISkin * m_skin ;

	BOOL m_selected ;

	SkinBoxListener * m_listener ;

	static const int kImageSkinWidth ;
	static const int kImageSkinHeight ;
	static const int kImageSkinBorderWidth ;

	static const int kColorSkinWidth ;
	static const int kColorSkinHeight ;
	static const int kColorSkinBorderWidth ;

};
