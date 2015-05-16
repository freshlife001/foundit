//////////////////////////////////////////////////////////////////////////
//
//	描述:   动画控件，用于显示动画，当然，该动画可以是静止的
//
//	
//	日期:   2011/02/21
// 															┏━┯━┓
// 															┃林│珽┃
// 															┠─┼─┨
// 															┃印│震┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "wndless/IWndLess.h"
#include "IWLAnimation.h"

class WLAnimationImp :
	public WLAnimationImpBase
{
public:
	WLAnimationImp(void);
	virtual ~WLAnimationImp(void);

public:
	//////////////////////////////////////////////////////////////////////////
	// Operation
	virtual BOOL Open(LPCTSTR lpszPath) ;
	virtual BOOL Open(UINT nID) ;
	virtual BOOL Open(Shared_ImageEx &imgAnimation) ;

	virtual BOOL Open(LPCTSTR lpszPath, UINT uCol, UINT uRow) ;
	virtual BOOL Open(UINT nID, UINT uCol, UINT uRow) ;
	virtual BOOL Open(Shared_ImageEx &imgAnimation, UINT uCol, UINT uRow) ;

	virtual void Close() ;

	virtual UINT SetPlayElapse(UINT uPlayElapse) ;

	virtual void Play() ;
	virtual void Stop() ;
	virtual BOOL IsValid() 
	{
		return !(m_imgAnimation.IsNull()) ;
	}

    virtual BOOL MakeGray(BOOL make_gray) 
    {
        if (m_make_gray == make_gray)
            return m_make_gray ;

        BOOL old = m_make_gray ;
        m_make_gray = make_gray ;
        UpdateView() ;

        return old ;
    }

protected:
	//////////////////////////////////////////////////////////////////////////
	// IWndLess Implement
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) ;
	virtual void OnDraw(HDC hDC, RECT const &rcUpdate) ;

	//////////////////////////////////////////////////////////////////////////
	// Message Handler
	void OnTimer(UINT_PTR nIDEvent) ;

	//////////////////////////////////////////////////////////////////////////
	// Inner Function

	void PlayCallbackFucntion() ;

// Data
protected:
	Shared_ImageEx m_imgAnimation ;

	UINT m_uImageColCount ;		// 图像列数
	UINT m_uImageRowCount ;		// 图像行数
	UINT m_uPlayElapse ;		// 播放间隔时间
	UINT m_uFrameIndex ;		// 动画帧索引
	UINT m_uFrameWidth ;
	UINT m_uFrameHeight ;

    BOOL m_make_gray ;          // 标记是否灰度化

};
