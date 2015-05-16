#include "StdAfx.h"

#include "common/utils/ImageEx/ImageEx.h"
#include "common/utils/Graphics.h"
#include "common/utils/UIUtils.h"

#include "WLScrollBar.h"

#include "common/MemoryLeakChecker.h"

WLScrollBarImp::WLScrollBarImp(void)
:
//m_pComboImageOldBmp(NULL)
//, 
m_nChannelBegin(0)
, m_nChannelEnd(0)
, m_nMin(0)
, m_nMax(99)
, m_uPage(10)
, m_nPos(0)
, m_nTrackPos(0)
, m_uFirstLDownTimer(TIMERID_SCROLLBAR_CHANNEL_FIRSTDOWN)
, m_uKeepLDownTimer(TIMERID_SCROLLBAR_CHANNEL_KEEPDOWN)
, m_enLDownPos(LDOWN_NOTHING)
, m_enHighLightPart(HIGHLIGHT_NOTHING)
, m_nThumbHeadLength(4)
, m_nThumbTailLength(4)
// , m_cvComboImage(TRUE)
{
}

WLScrollBarImp::~WLScrollBarImp(void)
{
}

//////////////////////////////////////////////////////////////////////////
// Operation

// 设置之后就不能改变
BOOL WLScrollBarImp::SetImage(UINT uID) 
{
	m_imgCombo = Shared_ImageEx::FromId(uID) ;
	m_szComboImageSize.cx = m_imgCombo->GetWidth() ;
	m_szComboImageSize.cy = m_imgCombo->GetHeight() ;

	m_nButtonSide = m_szComboImageSize.cy / 3 ;

	return TRUE ;
}

int WLScrollBarImp::SetScrollInfo(LPCSCROLLINFO lpsi) 
{
	if (lpsi == NULL)
	{
		ASSERT(FALSE) ;
		return m_nPos ;
	}

	BOOL bThumbPosChange = FALSE ;
	BOOL bThumbLengthChange = FALSE ;

	if (lpsi->fMask & SIF_RANGE)
	{
		if (lpsi->nMin > lpsi->nMax)
		{
			TRACE(_T("SetScrollInfo Error Parameter: min is greater than max\n")) ;

			SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;
			return m_nPos ;
		}
		else
		{
			if (m_nMin != lpsi->nMin || m_nMax != lpsi->nMax)
			{
				bThumbPosChange = TRUE ;
				bThumbLengthChange = TRUE ;
			}

			m_nMin = lpsi->nMin ;
			m_nMax = lpsi->nMax ;
		}
	}

	if (lpsi->fMask & SIF_PAGE)
	{
		// 先调整nPage至合理值
		UINT uPage = lpsi->nPage ;
		if (uPage > (UINT)(m_nMax + 1 - m_nMin))
		{
			//TRACE(_T("SetScrollInfo Error Parameter: page is greater than the range\n")) ;
			//uPage = (UINT)(m_nMax + 1 - m_nMin) ;
		}

		if (m_uPage != uPage)
		{
			// 长度变化必定导致位置变化，因为要对误差进行调整
			bThumbPosChange = TRUE ;
			bThumbLengthChange = TRUE ;
			m_uPage = uPage ;
		}
	}

	if (lpsi->fMask & SIF_POS)
	{
		// 先调整nPos至合理值
        int nPos = lpsi->nPos ;

        if (nPos > (m_nMax + 1 - (int)m_uPage))
            nPos = m_nMax + 1 - (int)m_uPage ;

		if (nPos < m_nMin)
			nPos = m_nMin ;

// 		if (nPos < m_nMin || nPos > (m_nMax + 1 - (int)m_nPage))
// 		{
// 			TRACE(_T("SetScrollInfo Error Parameter: pos is less than min or greater than the (max + 1 - page)\n")) ;
// 		}

		m_nPos = nPos ;
		if (m_nTrackPos != nPos)
		{
			// 位置变化必定导致长度变化，因为要对误差进行调整
			bThumbPosChange = TRUE ;
			bThumbLengthChange = TRUE ;
			m_nTrackPos = nPos ;
		}
	}

	// 根据滚动条属性设置Thumb位置和长度
	if (m_nMin > m_nMax || (int)m_uPage >= (m_nMax + 1 - m_nMin))
	{
// 		if (m_dwStyle & STYLE_DISABLENOSCROLL)
// 			m_btnThumb.SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;
// 		else
		if (GetStyle() & WS_VISIBLE)
		{
			SetWndLessPos(0, 0, 0, 0, SWLP_HIDE) ;
			GetParent()->SendMessage(WM_SCROLLBAR_SHOW, (WPARAM)FALSE, (LPARAM)m_wrapper) ;
		}
	}
	else
	{
		if (!(GetStyle() & WS_VISIBLE))
		{
			SetWndLessPos(0, 0, 0, 0, SWLP_SHOW) ;
			GetParent()->SendMessage(WM_SCROLLBAR_SHOW, (WPARAM)TRUE, (LPARAM)m_wrapper) ;
		}

		if (m_nPos + (int)m_uPage > m_nMax + 1)
		{
			m_nPos = m_nMax + 1 - (int)m_uPage ;
		}
	}

	UINT uSwpFlags = 0 ;

	//int nThumbNewPos = ThumbPosFromScrollPos(m_nPos) ;
	//int nThumbLength = ThumbPosFromScrollPos(m_nPos + m_nPage) - nThumbNewPos ;

	m_nThumbLength = ThumbLengthFromScrollPage(m_uPage) ;
	if (m_nThumbLength < GetMinThumbLength())
		m_nThumbLength = GetMinThumbLength() ;

	int nThumbNewPos = ThumbPosFromScrollPos(m_nPos) ;

	if (GetStyle() & STYLE_VERT)
	{
		bThumbPosChange ? (uSwpFlags |= SWLP_Y) : 0 ;
		bThumbLengthChange ? (uSwpFlags |= SWLP_CY) : 0 ;

		m_btnThumb.SetWndLessPos(
			0, nThumbNewPos, 0, m_nThumbLength,
			uSwpFlags
			) ;
	}
	else
	{
		bThumbPosChange ? (uSwpFlags |= SWLP_X) : 0 ;
		bThumbLengthChange ? (uSwpFlags |= SWLP_CX) : 0 ;

		m_btnThumb.SetWndLessPos(
			nThumbNewPos, 0, m_nThumbLength, 0, 
			uSwpFlags
			) ;
	}

	return m_nPos ;
}

BOOL WLScrollBarImp::GetScrollInfo(
								LPSCROLLINFO lpScrollInfo,
								UINT nMask// = SIF_ALL 
								) 
{
	if (lpScrollInfo == NULL)
	{
		ASSERT(FALSE) ;
		return FALSE ;
	}

	if (nMask & SIF_RANGE)
	{
		lpScrollInfo->nMin = m_nMin ;
		lpScrollInfo->nMax = m_nMax ;
	}

	if (nMask & SIF_POS)
	{
		lpScrollInfo->nPos = m_nPos ;
	}

	if (nMask & SIF_PAGE)
	{
		lpScrollInfo->nPage = m_uPage ;
	}

	return TRUE ;
}

//////////////////////////////////////////////////////////////////////////
// IWndLess Implement

BOOL WLScrollBarImp::OnCreate( DWORD dwStyle, int /*x*/, int /*y*/, int cx, int cy, IWndLess * /*pParent*/ ) 
{
	// 创建组合图像dc
	WLDC dc(this) ;
	//if (!m_cvComboImage.CreateCompatibleDC(dc.GetSafeHdc(), m_szComboImageSize.cx, m_nButtonSide))
	if (!m_cvComboImage.Create32BitDC(dc.GetSafeHdc(), m_szComboImageSize.cx, m_nButtonSide))
		return FALSE ;
	{
		Graphics gph(m_cvComboImage) ;
		if (GetStyle() & STYLE_VERT)
		{
			gph.DrawImage(
				m_imgCombo,
				Rect(0, 0, m_szComboImageSize.cx, m_nButtonSide),
				0, 0, m_szComboImageSize.cx, m_nButtonSide,
				UnitPixel, NULL
				) ;
		}
		else
		{
			gph.DrawImage(
				m_imgCombo,
				Rect(0, 0, m_szComboImageSize.cx, m_nButtonSide),
				0, m_nButtonSide, m_szComboImageSize.cx, m_nButtonSide,
				UnitPixel, NULL
				) ;
		}
	}

	if (dwStyle & STYLE_VERT)		// 垂直
	{
		// 根据风格调整窗口位置大小
		SetWndLessPos(0 ,0, m_nButtonSide, 0, SWLP_CX|SWLP_DEFER);
		//m_szClient.cx = m_nButtonSide ;
		if (dwStyle & STYLE_RIGHTALIGN)
		{
			CPoint tlp;
			GetPos(&tlp);
			tlp.x += (cx - m_nButtonSide) ;
			SetWndLessPos(tlp.x ,0, 0, 0, SWLP_X|SWLP_DEFER);
		}
		//	m_ptTopLeftInParent.x += (cx - m_nButtonSide) ;

		if (GetStyle() & STYLE_NOARROW)
		{
			m_nChannelBegin = 0 ;
			m_nChannelEnd = cy ;
		}
		else
		{
			m_nChannelBegin = m_nButtonSide ;
			m_nChannelEnd = cy - m_nButtonSide ;
		}

		m_nThumbLength = ThumbLengthFromScrollPage(m_uPage) ;
		if (m_nThumbLength < GetMinThumbLength())
			m_nThumbLength = GetMinThumbLength() ;

		// 如果高度太小，则不创建向上向下按钮
		//if (cy > m_szComboImageSize.cy)
		{
			if (!(GetStyle() & STYLE_NOARROW))
			{
				if (!m_btnUp.Create(WS_VISIBLE, 0, 0, m_nButtonSide, m_nButtonSide, this, IDC_BTN_UP))
					return FALSE ;
                //m_btnUp.set_request_focus_on_press(FALSE) ;

				if (!m_btnDown.Create(WS_VISIBLE, 0, m_nChannelEnd, m_nButtonSide, m_nButtonSide, this, IDC_BTN_DOWN))
                    return FALSE ;
                //m_btnDown.set_request_focus_on_press(FALSE) ;
			}
			if (!m_btnThumb.Create(
				WS_VISIBLE, 
				0, m_nChannelBegin, m_nButtonSide, m_nThumbLength,
				this, IDC_BTN_THUMB
				)
				)
                return FALSE ;
           // m_btnThumb.set_request_focus_on_press(FALSE) ;
		}
	}
	else						// 水平
	{
		// 根据风格调整窗口位置大小
		SetWndLessPos(0 ,0, 0, m_nButtonSide, SWLP_CY|SWLP_DEFER);
		//m_szClient.cy = m_nButtonSide ;
		if (GetStyle() & STYLE_BOTTOMALIGN)
		{
			CPoint tlp;
			GetPos(&tlp);
			tlp.y += (cy - m_nButtonSide) ;
			SetWndLessPos(0 ,tlp.y, 0, 0, SWLP_Y|SWLP_DEFER);
		}
			//m_ptTopLeftInParent.y += (cy - m_nButtonSide) ;

		if (GetStyle() & STYLE_NOARROW)
		{
			m_nChannelBegin = 0 ;
			m_nChannelEnd = cx ;
		}
		else
		{
			m_nChannelBegin = m_nButtonSide ;
			m_nChannelEnd = cx - m_nButtonSide ;
		}

		m_nThumbLength = ThumbLengthFromScrollPage(m_uPage) ;
		if (m_nThumbLength < GetMinThumbLength())
			m_nThumbLength = GetMinThumbLength() ;

		// 如果宽度太小，则不创建向左向右按钮
		//if (cx > m_szComboImageSize.cy)
		{
			if (!(GetStyle() & STYLE_NOARROW))
			{
				if (!m_btnUp.Create(WS_VISIBLE, 0, 0, m_nButtonSide, m_nButtonSide, this, IDC_BTN_UP))
                    return FALSE ;
               // m_btnUp.set_request_focus_on_press(FALSE) ;

				if (!m_btnDown.Create(WS_VISIBLE, m_nChannelEnd, 0, m_nButtonSide, m_nButtonSide, this, IDC_BTN_DOWN))
                    return FALSE ;
               // m_btnDown.set_request_focus_on_press(FALSE) ;
			}
			if (!m_btnThumb.Create(
				WS_VISIBLE,
				m_nChannelBegin, 0, m_nThumbLength, m_nButtonSide,
				this, IDC_BTN_THUMB
				)
				)
                return FALSE ;
           // m_btnThumb.set_request_focus_on_press(FALSE) ;
		}
	}

	return TRUE ;
}

void WLScrollBarImp::OnDestroy() 
{
// 	if (m_dcComboImage.GetSafeHdc() != NULL)
// 	{
// 		m_dcComboImage.SelectObject(m_pComboImageOldBmp) ;
// 	}
}

void WLScrollBarImp::_InternalOnDraw(HDC hDC, RECT const &rcUpdate) 
{
	//hdutils::CPUPerformance cp(_T("WLScrollBar::_InternalOnDraw")) ;
	__super::_InternalOnDraw(hDC, rcUpdate) ;
}

void WLScrollBarImp::OnDraw(HDC hDC, RECT const &rcUpdate) 
{
	//hdutils::CPUPerformance cp(_T("WLScrollBar::OnDraw")) ;

	CRect rc ;
	GetRectInHost(rc) ;

	int nUnitSide = m_nButtonSide ;
	int nFrontImageIndex = 11, nBackImageIndex = 11 ;
	switch (m_enHighLightPart)
	{
	case HIGHLIGHT_NOTHING :
		nFrontImageIndex = 11 ;
		nBackImageIndex = 11 ;
		break ;

	case HIGHLIGHT_FRONT :
		nFrontImageIndex = 12 ;
		nBackImageIndex = 11 ;
		break ;

	case HIGHLIGHT_BACK :
		nFrontImageIndex = 11 ;
		nBackImageIndex = 12 ;
		break ;
	}

	CRect rcThumb ;
	m_btnThumb.GetRectInParent(rcThumb) ;
	if (GetStyle() & STYLE_VERT)
	{
		int nFrontBegin = max(rc.top + m_nChannelBegin, rcUpdate.top) ;
		int nFrontEnd = min(rc.top + rcThumb.top, rcUpdate.bottom) ;

		int nLength = nFrontEnd - nFrontBegin ;
		if (nLength > 0)
		{
			hdutils::AlphaLineTile(
				hDC,
				rc.left, nFrontBegin, nUnitSide, nLength,
				m_cvComboImage,
				nFrontImageIndex * nUnitSide, 0, nUnitSide, nUnitSide - 1, 
				0, 0, FALSE
				) ;
		}

		int nBackBegin = max(rc.top + rcThumb.bottom, rcUpdate.top) ;
		int nBackEnd = min(rc.top + m_nChannelEnd, rcUpdate.bottom) ;
		nLength = nBackEnd - nBackBegin ;
		if (nLength > 0)
		{
			hdutils::AlphaLineTile(
				hDC,
				rc.left, nBackBegin, nUnitSide, nLength,
				m_cvComboImage,
				nBackImageIndex * nUnitSide, 0, nUnitSide, nUnitSide - 1, 
				0, 0, FALSE
				) ;
		}
	}
	else
	{
		int nFrontBegin = max(rc.left + m_nChannelBegin, rcUpdate.left) ;
		int nFrontEnd = min(rc.left + rcThumb.left, rcUpdate.right) ;

		int nLength = nFrontEnd - nFrontBegin ;
		if (nLength > 0)
		{
			hdutils::AlphaLineTile(
				hDC,
				nFrontBegin, rc.top, nLength, nUnitSide,
				m_cvComboImage,
				nFrontImageIndex * nUnitSide, 0, nUnitSide - 1, nUnitSide, 
				0, 0, TRUE
				) ;
		}

		int nBackBegin = max(rc.left + rcThumb.right, rcUpdate.left) ;
		int nBackEnd = min(rc.left + m_nChannelEnd, rcUpdate.right) ;
		nLength = nBackEnd - nBackBegin ;
		if (nLength > 0)
		{
			hdutils::AlphaLineTile(
				hDC,
				nBackBegin, rc.top, nLength, nUnitSide,
				m_cvComboImage,
				nBackImageIndex * nUnitSide, 0, nUnitSide - 1, nUnitSide, 
				0, 0, TRUE
				) ;
		}
	}
}

BOOL WLScrollBarImp::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult) 
{
	BOOL bRes = FALSE ;
	*pResult = 0 ;

	switch (message)
	{
	case WM_NOTIFY :
		{
			UINT uID = wParam ;
			LPNMWLHDR pNMWLHDR = (LPNMWLHDR)lParam ;

			switch (uID)
			{
			case IDC_BTN_UP :
				{
					switch (pNMWLHDR->code)
					{
					case NM_CUSTOMDRAW :
						OnBtnUpCustomDraw(pNMWLHDR, pResult) ;
						bRes = TRUE ;
						break ;

					case NM_LDOWN :
						OnBtnUpLDown(pNMWLHDR, pResult) ;
						bRes = TRUE ;
						break ;
					}
				}
				break ;

			case IDC_BTN_DOWN :
				{
					switch (pNMWLHDR->code)
					{
					case NM_CUSTOMDRAW :
						OnBtnDownCustomDraw(pNMWLHDR, pResult) ;
						bRes = TRUE ;
						break ;

					case NM_LDOWN :
						OnBtnDownLDown(pNMWLHDR, pResult) ;
						bRes = TRUE ;
						break ;
					}
				}
				break ;

			case IDC_BTN_THUMB :
				{
					switch (pNMWLHDR->code)
					{
					case NM_CUSTOMDRAW :
						{
							OnBtnThumbCustomDraw(pNMWLHDR, pResult) ;
							bRes = TRUE ;
						}
						break ;

					case WLScrollBarThumbImp::NM_BEGINDRAG :
						{
							OnBtnThumbBeginDrag(pNMWLHDR, pResult) ;
							bRes = TRUE ;
						}
						break ;

					case WLScrollBarThumbImp::NM_DRAGGING :
						{
							OnBtnThumbDragging(pNMWLHDR, pResult) ;
							bRes = TRUE ;
						}
						break ;

					case WLScrollBarThumbImp::NM_ENDDRAG :
						{
							OnBtnThumbEndDrag(pNMWLHDR, pResult) ;
							bRes = TRUE ;
						}
						break ;
					}
				}
				break ;
			}
		}
		break ;

	case WM_MOUSEMOVE :
		{
			OnMouseMove(wParam, CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_LBUTTONDOWN :
	case WM_LBUTTONDBLCLK :
		{
			OnLButtonDown(wParam, CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_LBUTTONUP :
		{
			OnLButtonUp(wParam, CPoint(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_CAPTURECHANGED :
		{
			OnCaptureChanged(HWL(lParam)) ;
			bRes = TRUE ;
		}
		break ;

	case WM_TIMER :
		{
			OnTimer(wParam) ;
			bRes = TRUE ;
		}
		break ;
	}

	return bRes ;
}

void WLScrollBarImp::OnSized(SIZE const &szOld, SIZE const &szNew) 
{
	if (GetStyle() & STYLE_VERT)
	{
		if (szNew.cy != szOld.cy)
		{
			if (GetStyle() & STYLE_NOARROW)
			{
				m_nChannelEnd = szNew.cy ;
			}
			else
			{
				m_nChannelEnd = szNew.cy - m_nButtonSide ;
				m_btnDown.SetWndLessPos(0, m_nChannelEnd, 0, 0, SWLP_Y) ;
			}

			m_nThumbLength = ThumbLengthFromScrollPage(m_uPage) ;
			if (m_nThumbLength < GetMinThumbLength())
				m_nThumbLength = GetMinThumbLength() ;

			int nThumbNewPos = ThumbPosFromScrollPos(m_nPos) ;
			m_btnThumb.SetWndLessPos(0, nThumbNewPos, 0, m_nThumbLength, SWLP_Y | SWLP_CY) ;
		}
	}
	else
	{
		if (szNew.cx != szOld.cx)
		{
			if (GetStyle() & STYLE_NOARROW)
			{
				m_nChannelEnd = szNew.cx ;
			}
			else
			{
				m_nChannelEnd = szNew.cx - m_nButtonSide ;
				m_btnDown.SetWndLessPos(m_nChannelEnd, 0, 0, 0, SWLP_X) ;
			}

			m_nThumbLength = ThumbLengthFromScrollPage(m_uPage) ;
			if (m_nThumbLength < GetMinThumbLength())
				m_nThumbLength = GetMinThumbLength() ;

			int nThumbNewPos = ThumbPosFromScrollPos(m_nPos) ;
			m_btnThumb.SetWndLessPos(nThumbNewPos, 0, m_nThumbLength, 0, SWLP_X | SWLP_CX) ;
		}
	}
}

void WLScrollBarImp::OnBtnUpCustomDraw(LPNMWLHDR pNMWLHDR, LRESULT * pResult) 
{
	*pResult = 1 ;
	WLButton::LPNMCDWLBTNINFO pBtnInfo = (WLButton::LPNMCDWLBTNINFO)pNMWLHDR ;

	//CDC dc ;
	//dc.Attach(pBtnInfo->hDC) ;

	//Graphics gph(pBtnInfo->hDC) ;

	HDC &hDC = pBtnInfo->hDC ;

	CRect rcBtn(pBtnInfo->rc) ;

	int nImageIndex = 0 ;
	if (pBtnInfo->dwStatus & WLButton::BS_SELECTED)
		nImageIndex = 2 ;
	else if (pBtnInfo->dwStatus & WLButton::BS_HOT)
		nImageIndex = 1 ;

// 	::BitBlt(
// 		hDC,
// 		rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(),
// 		m_cvComboImage,
// 		nImageIndex * m_nButtonSide, 0,
// 		SRCCOPY
// 		) ;

	BLENDFUNCTION bfn ;
	bfn.BlendOp = AC_SRC_OVER ;
	bfn.BlendFlags = 0 ;
	bfn.SourceConstantAlpha = 255 ;
	bfn.AlphaFormat = AC_SRC_ALPHA ;
	::AlphaBlend(
		hDC,
		rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(),
		m_cvComboImage,
		nImageIndex * m_nButtonSide, 0, m_nButtonSide, m_nButtonSide,
		bfn
		) ;

/*	if (m_dwStyle & STYLE_VERT)		// 垂直
	{
// 		dc.BitBlt(
// 			rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(),
// 			&m_dcComboImage,
// 			nImageIndex * m_nButtonSide, 0,
// 			SRCCOPY
// 			) ;

		gph.DrawImage(
			m_imgCombo,
			Rect(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height()),
			nImageIndex * m_nButtonSide, 0, m_nButtonSide, m_nButtonSide, UnitPixel, NULL
			) ;
	}
	else							// 水平
	{
// 		dc.BitBlt(
// 			rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(),
// 			&m_dcComboImage,
// 			nImageIndex * m_nButtonSide, m_nButtonSide,
// 			SRCCOPY
// 			) ;

		gph.DrawImage(
			m_imgCombo,
			Rect(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height()),
			nImageIndex * m_nButtonSide, m_nButtonSide, m_nButtonSide, m_nButtonSide, UnitPixel, NULL
			) ;
	}*/

	//dc.Detach() ;
}

void WLScrollBarImp::OnBtnDownCustomDraw(LPNMWLHDR pNMWLHDR, LRESULT * pResult) 
{
	*pResult = 1 ;
	WLButton::LPNMCDWLBTNINFO pBtnInfo = (WLButton::LPNMCDWLBTNINFO)pNMWLHDR ;

	//CDC dc ;
	//dc.Attach(pBtnInfo->hDC) ;

	//Graphics gph(pBtnInfo->hDC) ;

	HDC &hDC = pBtnInfo->hDC ;

	CRect rcBtn(pBtnInfo->rc) ;

	int nImageIndex = 0 ;
	if (pBtnInfo->dwStatus & WLButton::BS_SELECTED)
		nImageIndex = 2 ;
	else if (pBtnInfo->dwStatus & WLButton::BS_HOT)
		nImageIndex = 1 ;

	BLENDFUNCTION bfn ;
	bfn.BlendOp = AC_SRC_OVER ;
	bfn.BlendFlags = 0 ;
	bfn.SourceConstantAlpha = 255 ;
	bfn.AlphaFormat = AC_SRC_ALPHA ;

	::AlphaBlend(
		hDC,
		rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(),
		m_cvComboImage,
		(nImageIndex + 4) * m_nButtonSide, 0, m_nButtonSide, m_nButtonSide,
		bfn
		) ;

/*	if (m_dwStyle & STYLE_VERT)		// 垂直
	{
// 		dc.BitBlt(
// 			rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(),
// 			&m_dcComboImage,
// 			(nImageIndex + 4) * m_nButtonSide, 0,
// 			SRCCOPY
// 			) ;

		gph.DrawImage(
			m_imgCombo,
			Rect(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height()),
			(nImageIndex + 4) * m_nButtonSide, 0, m_nButtonSide, m_nButtonSide, UnitPixel, NULL
			) ;
	}
	else							// 水平
	{
// 		dc.BitBlt(
// 			rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(),
// 			&m_dcComboImage,
// 			(nImageIndex + 4) * m_nButtonSide, m_nButtonSide,
// 			SRCCOPY
// 			) ;

		gph.DrawImage(
			m_imgCombo,
			Rect(rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height()),
			(nImageIndex + 4) * m_nButtonSide, m_nButtonSide, m_nButtonSide, m_nButtonSide, UnitPixel, NULL
			) ;
	}*/

	//dc.Detach() ;
}

void WLScrollBarImp::OnBtnThumbCustomDraw(LPNMWLHDR pNMWLHDR, LRESULT * pResult) 
{
	*pResult = 1 ;
	WLButton::LPNMCDWLBTNINFO pBtnInfo = (WLButton::LPNMCDWLBTNINFO)pNMWLHDR ;

    CRect rcBtn(pBtnInfo->rc) ;

	int nImageIndex = 0 ;
	if (pBtnInfo->dwStatus & WLButton::BS_SELECTED)
		nImageIndex = 2 ;
	else if (pBtnInfo->dwStatus & WLButton::BS_HOT)
		nImageIndex = 1 ;

	if (GetStyle() & STYLE_VERT)		// 垂直
	{
		hdutils::AlphaLineTile(
			pBtnInfo->hDC,
			rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(),
			m_cvComboImage,
			(nImageIndex + 8) * m_nButtonSide, 0, m_nButtonSide, m_nButtonSide,
			m_nThumbHeadLength, m_nThumbTailLength,
			FALSE
			) ;
	}
	else							// 水平
	{
		hdutils::AlphaLineTile(
			pBtnInfo->hDC,
			rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(),
			m_cvComboImage,
			(nImageIndex + 8) * m_nButtonSide, 0, m_nButtonSide, m_nButtonSide,
			m_nThumbHeadLength, m_nThumbTailLength,
			TRUE
			) ;
	}
}

void WLScrollBarImp::OnBtnUpLDown(LPNMWLHDR /*pNMWLHDR*/, LRESULT * pResult) 
{
	*pResult = 1 ;
	SendScrollMessage(SB_LINEUP, 0) ;

/*	int nPos(m_nPos) ;
	nPos -= 1 ;
	if (nPos < m_nMin)
		nPos = m_nMin ;
	if (m_nPos == nPos)
		return ;
	m_nPos = nPos ;

	int nThumbPos = ThumbPosFromScrollPos(m_nPos) ;
	int nThumbLength = ThumbPosFromScrollPos(m_nPos + m_nPage) - nThumbPos ;
	if (m_dwStyle & STYLE_VERT)
	{
		m_btnThumb.SetWndLessPos(
			0, nThumbPos, 0, nThumbLength,
			SWLP_Y | SWLP_CY
			) ;
	}
	else
	{
		m_btnThumb.SetWndLessPos(
			nThumbPos, 0, nThumbLength, 0,
			SWLP_X | SWLP_CX
			) ;
	}*/
}

void WLScrollBarImp::OnBtnDownLDown(LPNMWLHDR /*pNMWLHDR*/, LRESULT * pResult) 
{
	*pResult = 1 ;
	SendScrollMessage(SB_LINEDOWN, 0) ;

/*	int nPos(m_nPos) ;
	nPos += 1 ;
	if (nPos > m_nMax + 1 - (int)m_nPage)
		nPos = m_nMax + 1 - (int)m_nPage ;
	if (m_nPos == nPos)
		return ;
	m_nPos = nPos ;

	int nThumbPos = ThumbPosFromScrollPos(m_nPos) ;
	int nThumbLength = ThumbPosFromScrollPos(m_nPos + m_nPage) - nThumbPos ;
	if (m_dwStyle & STYLE_VERT)
	{
		m_btnThumb.SetWndLessPos(
			0, nThumbPos, 0, nThumbLength,
			SWLP_Y | SWLP_CY
			) ;
	}
	else
	{
		m_btnThumb.SetWndLessPos(
			nThumbPos, 0, nThumbLength, 0,
			SWLP_X | SWLP_CX
			) ;
	}*/
}

void WLScrollBarImp::OnBtnThumbBeginDrag(LPNMWLHDR pNMWLHDR, LRESULT * pResult) 
{
	*pResult = 1 ;
	WLScrollBarThumbImp::LPNMDRAGINFO pDragInfo = (WLScrollBarThumbImp::LPNMDRAGINFO)pNMWLHDR ;

	CRect rc ;
	m_btnThumb.GetRectInHost(rc) ;

	m_ptBeginDragCursorInThumb = pDragInfo->point ;
	m_ptBeginDragCursorInThumb.x -= rc.left ;
	m_ptBeginDragCursorInThumb.y -= rc.top ;
}

void WLScrollBarImp::OnBtnThumbDragging(LPNMWLHDR pNMWLHDR, LRESULT * pResult) 
{
	*pResult = 1 ;
	WLScrollBarThumbImp::LPNMDRAGINFO pDragInfo = (WLScrollBarThumbImp::LPNMDRAGINFO)pNMWLHDR ;

	CRect rc ;
	GetRectInHost(rc) ;

	CRect rcThumb ;
	m_btnThumb.GetRectInParent(rcThumb) ;

	int nThumbLength = (int)m_uPage * (m_nChannelEnd - m_nChannelBegin) / (m_nMax + 1 - m_nMin) ;
	if (nThumbLength < GetMinThumbLength())
		nThumbLength = GetMinThumbLength() ;

	if (GetStyle() & STYLE_VERT)
	{
		int nY = pDragInfo->point.y - m_ptBeginDragCursorInThumb.y - rc.top ;
//		int nMaxThumbPos = m_nChannelEnd - rcThumb.Height() ;
		int nMaxThumbPos = m_nChannelEnd - nThumbLength ;

		if (nY < m_nChannelBegin)
			nY = m_nChannelBegin ;
		else if (nY > nMaxThumbPos)
			nY = nMaxThumbPos ;

		// 判断位置是否变化
		if (rcThumb.top == nY)
			return ;

		m_btnThumb.SetWndLessPos(
			0, nY, 0, nThumbLength,
			SWLP_Y | SWLP_CY
			) ;

		// 向父窗口发送滚动消息
		int uPos = ScrollPosFromThumbPos(nY) ;
		if (m_nTrackPos != uPos)
		{
			m_nTrackPos = uPos ;
			//TRACE(_T("Drag_Vscroll: %u\n"), uPos) ;
			SendScrollMessage(SB_THUMBTRACK, uPos) ;
		}
	}
	else
	{
		int nX = pDragInfo->point.x - m_ptBeginDragCursorInThumb.x - rc.left ;
		//int nMaxThumbPos = m_nChannelEnd - rcThumb.Width() ;
		int nMaxThumbPos = m_nChannelEnd - nThumbLength ;

		if (nX < m_nChannelBegin)
			nX = m_nChannelBegin ;
		else if (nX > nMaxThumbPos)
			nX = nMaxThumbPos ;

		// 判断位置是否变化
		if (rcThumb.left == nX)
			return ;

		m_btnThumb.SetWndLessPos(
			nX, 0, nThumbLength, 0,
			SWLP_X | SWLP_CX
			) ;

		// 向父窗口发送滚动消息
		int nPos = ScrollPosFromThumbPos(nX) ;
		if (m_nTrackPos != nPos)
		{
			m_nTrackPos = nPos ;
			TRACE(_T("Drag_Hscroll: %u\n"), nPos) ;
			SendScrollMessage(SB_THUMBTRACK, nPos) ;
		}
	}
}

void WLScrollBarImp::OnBtnThumbEndDrag(LPNMWLHDR /*pNMWLHDR*/, LRESULT * pResult) 
{
	*pResult = 1 ;
	//WLScrollBarThumb::LPNMDRAGINFO pDragInfo = (WLScrollBarThumb::LPNMDRAGINFO)pNMWLHDR ;

	CRect rc ;
	GetRectInHost(rc) ;

	CRect rcThumb ;
	m_btnThumb.GetRectInParent(rcThumb) ;

	if (GetStyle() & STYLE_VERT)
	{
		// 向父窗口发送滚动消息
		SendScrollMessage(SB_THUMBPOSITION, m_nTrackPos) ;

		// 如果滚动条位置和Thumb位置不一致，则调整Thumb位置
		//if (m_nPos != m_nTrackPos)
		{
		//	m_nTrackPos = m_nPos ;

			int nThumbNewPos = ThumbPosFromScrollPos(m_nPos) ;
			m_btnThumb.SetWndLessPos(
				0, nThumbNewPos, 0, 0,
				SWLP_Y
				) ;
		}
	}
	else
	{
		// 向父窗口发送滚动消息
		SendScrollMessage(SB_THUMBPOSITION, m_nTrackPos) ;

		// 如果滚动条位置和Thumb位置不一致，则调整Thumb位置
		//if (m_nPos != m_nTrackPos)
		{
		//	m_nTrackPos = m_nPos ;

			int nThumbNewPos = ThumbPosFromScrollPos(m_nPos) ;
			m_btnThumb.SetWndLessPos(
				nThumbNewPos, 0, 0, 0, 
				SWLP_X
				) ;
		}
	}
}

void WLScrollBarImp::OnMouseMove(UINT /*nFlags*/, CPoint point) 
{
	if (m_enLDownPos != LDOWN_NOTHING)
	{
		CRect rcInHost ;
		GetRectInHost(rcInHost) ;
		point.x -= rcInHost.left ;
		point.y -= rcInHost.top ;
		LDOWNPOS enLDownPos = HitTestLDownPos(point) ;

		HIGHLIGHTPART enHighLightPart ;
		if (m_enLDownPos == enLDownPos)
		{
			if (m_enLDownPos == LDOWN_FRONT)
			{
				enHighLightPart = HIGHLIGHT_FRONT ;
			}
			else if (m_enLDownPos == LDOWN_BACK)
			{
				enHighLightPart = HIGHLIGHT_BACK ;
			}
			else
			{
				enHighLightPart = HIGHLIGHT_NOTHING ;
			}
		}
		else
		{
			enHighLightPart = HIGHLIGHT_NOTHING ;
		}

		if (m_enHighLightPart != enHighLightPart)
		{
			m_enHighLightPart = enHighLightPart ;
			UpdateView() ;
		}
	}
}

void WLScrollBarImp::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
	if (GetCapture() != m_wrapper)
	{
		SetCapture() ;
	}

	// 判断鼠标按下的位置在Thumb前还是后
	CRect rcThumb ;
	m_btnThumb.GetRectInHost(rcThumb) ;

	if (GetStyle() & STYLE_VERT)
	{
		if (point.y < rcThumb.top)
		{
			m_enLDownPos = LDOWN_FRONT ;

			// 标记高亮
			m_enHighLightPart = HIGHLIGHT_FRONT ;
			UpdateView() ;

			SendScrollMessage(SB_PAGEUP, 0) ;
		}
		else if (point.y >= rcThumb.bottom)
		{
			m_enLDownPos = LDOWN_BACK ;

			// 标记高亮
			m_enHighLightPart = HIGHLIGHT_BACK ;
			UpdateView() ;

			SendScrollMessage(SB_PAGEDOWN, 0) ;
		}
	}
	else
	{
		if (point.x < rcThumb.left)
		{
			m_enLDownPos = LDOWN_FRONT ;

			// 标记高亮
			m_enHighLightPart = HIGHLIGHT_FRONT ;
			UpdateView() ;

			SendScrollMessage(SB_PAGEUP, 0) ;
		}
		else if (point.x >= rcThumb.right)
		{
			m_enLDownPos = LDOWN_BACK ;

			// 标记高亮
			m_enHighLightPart = HIGHLIGHT_BACK ;
			UpdateView() ;

			SendScrollMessage(SB_PAGEDOWN, 0) ;
		}
	}

	/*m_uFirstLDownTimer = */SetTimer(m_uFirstLDownTimer, 400, NULL) ;
	if (m_uFirstLDownTimer == 0)
		TRACE(_T("WLScrollBar SetTimer Error\n")) ;
}

void WLScrollBarImp::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
	if (GetCapture() == m_wrapper)
	{
		ReleaseCapture() ;
	}
}

void WLScrollBarImp::OnCaptureChanged(HWL /*hWnd*/) 
{
	m_enLDownPos = LDOWN_NOTHING ;

	// 标记高亮
	m_enHighLightPart = HIGHLIGHT_NOTHING ;
	UpdateView() ;

	if (m_uFirstLDownTimer != -1)
	{
		KillTimer(m_uFirstLDownTimer) ;
	}
	if (m_uKeepLDownTimer != -1)
	{
		KillTimer(m_uKeepLDownTimer) ;
	}
}

void WLScrollBarImp::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == m_uFirstLDownTimer)
	{
		KillTimer(m_uFirstLDownTimer) ;

		if (m_enLDownPos != LDOWN_NOTHING)
		{
			/*m_uKeepLDownTimer = */SetTimer(m_uKeepLDownTimer, 40, NULL) ;
			if (m_uKeepLDownTimer == 0)
				TRACE(_T("WLScrollBar SetTimer Error\n")) ;
		}
	}
	else if (nIDEvent == m_uKeepLDownTimer)
	{
		CPoint point ;
		GetCursorPos(&point) ;
		ScreenToClient(&point) ;
		LDOWNPOS enLDownPos = HitTestLDownPos(point) ;

		if (m_enLDownPos == LDOWN_FRONT)
		{
			if (m_enLDownPos == enLDownPos)
				SendScrollMessage(SB_PAGEUP, 0) ;
		}
		else if (m_enLDownPos == LDOWN_BACK)
		{
			if (m_enLDownPos == enLDownPos)
				SendScrollMessage(SB_PAGEDOWN, 0) ;
		}
		else
			KillTimer(m_uKeepLDownTimer) ;
	}
}

//////////////////////////////////////////////////////////////////////////
// Inner Function

int WLScrollBarImp::ScrollPosFromThumbPos(int nThumbPos) 
{
	if (nThumbPos < m_nChannelBegin)
		nThumbPos = m_nChannelBegin ;
	if (nThumbPos > m_nChannelEnd - m_nThumbLength)
		nThumbPos = m_nChannelEnd - m_nThumbLength ;

	//int nRes = m_nMin + (nThumbPos - m_nChannelBegin) * (m_nMax + 1 - m_nMin) / (m_nChannelEnd - m_nChannelBegin) ;

	int nRes = m_nMin
		+ (nThumbPos - m_nChannelBegin) * (m_nMax + 1 - (int)m_uPage - m_nMin)
		/ (m_nChannelEnd - m_nThumbLength - m_nChannelBegin) ;

	if (nRes > m_nMax + 1 - (int)m_uPage)
		nRes = m_nMax + 1 - (int)m_uPage ;

	return nRes ;
}

int WLScrollBarImp::ThumbPosFromScrollPos(int nScrollPos) 
{
	if ((int)m_uPage >= m_nMax + 1 - m_nMin)
		return 0 ;

	if (nScrollPos < m_nMin)
		nScrollPos = m_nMin ;
	if (nScrollPos > m_nMax + 1 - (int)m_uPage)
		nScrollPos = m_nMax + 1 - (int)m_uPage ;

	return m_nChannelBegin
		+ (nScrollPos - m_nMin) * (m_nChannelEnd - m_nChannelBegin - m_nThumbLength) / (m_nMax + 1 - (int)m_uPage - m_nMin) ;

	//return m_nChannelBegin + (nScrollPos - m_nMin) * (m_nChannelEnd - m_nChannelBegin) / (m_nMax + 1 - m_nMin) ;
}

int WLScrollBarImp::ThumbLengthFromScrollPage(UINT nPage) 
{
	return (int)nPage * (m_nChannelEnd - m_nChannelBegin) / (m_nMax + 1 - m_nMin) ;
}

int inline WLScrollBarImp::GetMinThumbLength() const 
{
	return m_nThumbHeadLength + m_nThumbTailLength ;
}

/*
int WLScrollBar::ThumbLengthFromScrollPage(int nScrollPage) 
{
	return (m_nChannelEnd - m_nChannelBegin) * nScrollPage / (m_nMax + 1 - m_nMin) ;
}

int WLScrollBar::ThumbLengthFromScrollPosAndPage(int nScrollPos, int nScrollPage) 
{
	return ThumbPosFromScrollPos(nScrollPos + nScrollPage) - ThumbPosFromScrollPos(nScrollPos) ;
}
*/
void WLScrollBarImp::SendScrollMessage(UINT nSBCode, UINT nPos) 
{
	UINT message = (GetStyle() & STYLE_VERT) ? WM_VSCROLL : WM_HSCROLL ;
	//WPARAM wParam = MAKEWPARAM(nSBCode, nPos) ;
	SCROLLMSGINFO smi ;
	smi.nSBCode = nSBCode ;
	smi.nPos = nPos ;
	GetParent()->SendMessage(message, WPARAM(&smi), (LPARAM)m_wrapper) ;
}

WLScrollBarImp::LDOWNPOS WLScrollBarImp::HitTestLDownPos(CPoint const &point) 
{
	CRect rcThumb ;
	m_btnThumb.GetRectInParent(rcThumb) ;

	CSize szClient;
	GetSize(&szClient);

	if (GetStyle() & STYLE_VERT)
	{
		if (point.x >= 0 && point.x < szClient.cx)
		{
			if (point.y >= m_nChannelBegin && point.y < rcThumb.top)
			{
				return LDOWN_FRONT ;
			}
			else if (point.y >= rcThumb.bottom && point.y < m_nChannelEnd)
			{
				return LDOWN_BACK ;
			}
			else
			{
				return LDOWN_NOTHING ;
			}
		}
		else
		{
			return LDOWN_NOTHING ;
		}
	}
	else
	{
		if (point.y >= 0 && point.y < szClient.cy)
		{
			if (point.x >= m_nChannelBegin && point.x < rcThumb.left)
			{
				return LDOWN_FRONT ;
			}
			else if (point.x >= rcThumb.right && point.x < m_nChannelEnd)
			{
				return LDOWN_BACK ;
			}
			else
			{
				return LDOWN_NOTHING ;
			}
		}
		else
		{
			return LDOWN_NOTHING ;
		}
	}

	return LDOWN_NOTHING ;
}


template<>
UILIB_EXPORT WLScrollBarImpBase *CreateDefaultImp< WLScrollBarImpBase >(IWLScrollBar * wrapper)

{
	WLScrollBarImp *ret = new WLScrollBarImp();
	ret->SetWrapper(wrapper);
	return ret;
};
