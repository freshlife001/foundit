#pragma once

#include "IWLButton.h"

class IWLButtonAL : public IWLButton
{
public:
	virtual ~IWLButtonAL(void){};

public:
	// 按钮类型
	enum BTN_TYPE
	{
		PUSH		= 0x0000,
		CHECK		= 0x0001,
		RADIO		= 0x0002,
		LINK		= 0x0003,		// 按钮按下时不移动文本，鼠标移上去时变为手状
	} ;

	// 按钮风格
	enum BTN_STYLE
	{
		NORMAL			= 0x0000,
		PUSHLIKE		= 0x0001,		// Check和Radio风格时有效
		FITFRGNDSIZE	= 0x0002,		// 根据前景自动调整大小
		FITTEXTSIZE		= 0x0004,		// 根据文字自动调整大小
	} ;

	// 背景风格
	enum BKGND_STYLE
	{
		NOBKGND		= 0x0000,
		HANDPAINT	= 0x0001,		// 手绘背景，即指定各种颜色进行填充
		TRANSLUCENT	= 0x0002,		// 背景透明，使用父窗口背景作为背景，
	} ;

	// 前景风格
	enum FRGND_STYLE
	{
		NOFRGND		= 0x0000,		// 无前景
		FIXED		= 0x0001,		// 固定整个前景
		FITX		= 0x0002,		// 固定前景高度，x方向采用01...12拆分绘制
		FITY		= 0x0004,		// 固定前景宽度，y方向采用01...12拆分绘制
		FITXY		= 0x0006,		// x、y方向均采用01...12绘制(未实现)
	} ;

	// 文本风格，文本包括icon和text
	// BTN_STYLE不自动调整大小时，CENTER、LEFT、RIGHT、TOP、BOTTOM有效
	// FITTEXTSIZE
	//	FITFRGNDSIZE + FITX: TOP, BOTTOM 有效
	//	FITFRGNDSIZE + FITY: LEFT, RIGHT 有效
	enum TEXT_STYLE
	{
		// 		TS_NEAR		= 0x0001,
		// 		TS_CENTER	= 0x0002,
		// 		TS_FAR		= 0x0004,
		// 		TS_VERTICAL	= 0x0008,		// 垂直绘制(未实现)

		TS_LEFT		= 0x0001,
		TS_HCENTER	= 0x0000,
		TS_RIGHT	= 0x0002,
		TS_TOP		= 0x0010,
		TS_VCENTER	= 0x0000,
		TS_BOTTOM	= 0x0020,

		TS_CENTER	= 0x0000,
	} ;

	enum BKGND_COLOR_INDEX
	{
		BCI_NORMAL				= 0x0001,
		BCI_HOT					= 0x0002,
		BCI_PUSHED				= 0x0004,
		BCI_DEFAULT				= 0x0008,
		BCI_DISABLE				= 0x0010,

		BCI_UNCHECKED			= 0x0001,
		BCI_UNCHECKED_HOT		= 0x0002,
		BCI_UNCHECKED_PUSHED	= 0x0004,
		BCI_UNCHECKED_DISABLE	= 0x0008,
		BCI_CHECKED				= 0x0010,
		BCI_CHECKED_HOT			= 0x0020,
		BCI_CHECKED_PUSHED		= 0x0040,
		BCI_CHECKED_DISABLE		= 0x0080,

		BCI_ALL					= 0xFFFF,
	};


public:
	// 设置前景
	virtual void SetFrgnd(
		Image * pImgFrgnd,
		int nStateCount,
		DWORD dwFrgndStyle,
		BOOL bDefer = FALSE
		)  = 0;

	// 设置符号
	virtual void SetSign(
		Image * pImgCheckSign,
		BOOL bDefer = FALSE
		) = 0;

	// 设置图标
	virtual void SetIcon(
		Image * pImgIcon,
		BOOL bDefer = FALSE,
		BOOL bAction = TRUE
		) = 0;

	// 设置类型
	virtual void SetType(
		DWORD dwBtnType,
		BOOL bDefer = FALSE
		) = 0;

	// 设置风格
	virtual void SetStyle(
		DWORD dwBtnStyle,
		BOOL bDefer = FALSE
		) = 0;

	// 设置背景风格
	virtual void SetBkgndStyle(
		DWORD dwBkgndStyle,
		BOOL bUpdate = FALSE
		)  = 0;

	// 设置文本风格
	virtual void SetTextStyle(
		DWORD dwTextStyle,
		BOOL bDefer = FALSE
		) = 0;

	virtual void SetMargin(int nLeft, int nTop, int nRight, int nBottom, BOOL bDefer = FALSE)  = 0;
	virtual void SetSignTextSpace(int nSignTextSpace, BOOL bDefer = FALSE)  = 0;
	virtual void SetIconTextSpace(int nIconTextSpace, BOOL bDefer = FALSE)  = 0;

	virtual void SetBkgndColor(DWORD dwMask, ...)   = 0;
	virtual void SetTextColor(DWORD dwMask, ...)  = 0;

	virtual void EndDeferRecalcLayout()  = 0;

	virtual void SetUrl(CString const & sUrl)  = 0;

};

typedef CWndLessImpBase< CWLButtonShell<IWLButtonAL> >  WLButtonALImpBase;

template <class IT = IWLButtonAL, class IMPT = WLButtonALImpBase > 
class CWLButtonALShell : public CWLButtonShell<IT, IMPT> 
{
public:
	CWLButtonALShell() {}
	virtual ~CWLButtonALShell(){}

	// 设置前景
	virtual void SetFrgnd(
		Image * pImgFrgnd,
		int nStateCount,
		DWORD dwFrgndStyle,
		BOOL bDefer = FALSE
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetFrgnd(pImgFrgnd, nStateCount, dwFrgndStyle, bDefer);
		}
	};

	// 设置符号
	virtual void SetSign(
		Image * pImgCheckSign,
		BOOL bDefer = FALSE
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetSign(pImgCheckSign, bDefer);
		}
	};

	// 设置图标
	virtual void SetIcon(
		Image * pImgIcon,
		BOOL bDefer = FALSE,
		BOOL bAction = TRUE
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetIcon(pImgIcon, bDefer, bAction);
		}
	};

	// 设置类型
	virtual void SetType(
		DWORD dwBtnType,
		BOOL bDefer = FALSE
		)  
	{
		if (m_wlimp)
		{
			return m_wlimp->SetType(dwBtnType, bDefer);
		}
	};

	// 设置风格
	virtual void SetStyle(
		DWORD dwBtnStyle,
		BOOL bDefer = FALSE
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetStyle(dwBtnStyle, bDefer);
		}
	};

	// 设置背景风格
	virtual void SetBkgndStyle(
		DWORD dwBkgndStyle,
		BOOL bUpdate  = FALSE
		)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetBkgndStyle(dwBkgndStyle, bUpdate);
		}
	};

	// 设置文本风格
	virtual void SetTextStyle(
		DWORD dwTextStyle,
		BOOL bDefer = FALSE
		)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetTextStyle(dwTextStyle, bDefer);
		}
	};

	virtual void SetMargin(int nLeft, int nTop, int nRight, int nBottom, BOOL bDefer = FALSE)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetMargin(nLeft, nTop, nRight, nBottom, bDefer);
		}
	};
	virtual void SetSignTextSpace(int nSignTextSpace, BOOL bDefer = FALSE)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetSignTextSpace(nSignTextSpace, bDefer);
		}
	};
	virtual void SetIconTextSpace(int nIconTextSpace, BOOL bDefer = FALSE)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetIconTextSpace(nIconTextSpace, bDefer);
		}
	};

	virtual void SetBkgndColor(DWORD dwMask, ...) 
	{
		if (m_wlimp)
		{
			va_list marker;
			va_start(marker, dwMask) ;
			int i ;
			COLORREF colors[6] = {0};
			for (i = 0 ; i < 6 ; ++ i)
			{
				if (dwMask & (1 << i))
				{
					colors[i] = va_arg(marker, COLORREF) ;
				}
			}
			va_end(marker);

			m_wlimp->SetBkgndColor(dwMask,colors[0],colors[1],colors[2],colors[3],colors[4],colors[5]);
			return;
		}
	};
	virtual void SetTextColor(DWORD dwMask, ...) 
	{
		if (m_wlimp)
		{
			va_list marker;
			va_start(marker, dwMask) ;
			int i ;
			COLORREF colors[8] = {0};
			for (i = 0 ; i < 8 ; ++ i)
			{
				if (dwMask & (1 << i))
				{
					colors[i] = va_arg(marker, COLORREF) ;
				}
			}
			va_end(marker);
			
			m_wlimp->SetTextColor(dwMask,colors[0],colors[1],colors[2],colors[3],colors[4],colors[5],colors[6],colors[7]);
			return;
		}
	};

	virtual void EndDeferRecalcLayout() 
	{
		if (m_wlimp)
		{
			return m_wlimp->EndDeferRecalcLayout();
		}
	};

	virtual void SetUrl(CString const & sUrl)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetUrl(sUrl);
		}
	};

};

typedef CWLButtonALShell<> WLButtonAL;

UILIB_EXPORT BOOL WLButtonALProcess(WLButtonAL & button, LPCTSTR sStyle) ;



