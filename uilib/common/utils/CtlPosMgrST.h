//////////////////////////////////////////////////////////////////////////
//
//	当前功能：实现能够在窗口大小发生变化时控件位置相应变化
//	预期功能：包括对话框控件变化
//			  对话框菜单及控件更新(类ON_UPDATE_COMMAND_UI功能)
//	思路：    用map记录所需信息，在OnSize中调整控件位置
//
//////////////////////////////////////////////////////////////////////////
//	日期：	2009/3/18
//	版本：	ST0000, ST0001
//			(前一个版本, 当前版本)
//	作者：	林秋岚
//
//	版权声明：您可以任意发布、使用、修改该代码，但必须保留已有版本信息
//			  并添加您的版本改动信息。
//
//////////////////////////////////////////////////////////////////////////
//	日期：	2009/3/20
//	版本：	ST0001, ST0002
//	修改：	主要改动是添加了类的内嵌功能，使之可以通过作为对话框的一个变量
//			来控制控件的行为。
//			其他改动则是添加了控件4种变化模式的枚举型X、Y、CX、CY。
//	修改人：林秋岚
//
//	感谢网友 vc-今夜雨 的宝贵意见。
//
//////////////////////////////////////////////////////////////////////////
//	日期：	2009/6/18
//	版本：	ST0002, ST0003
//	修改：	从ST0002版本的CExtDialogST类中提取出CCtlPosMgrST类，可在目标窗口
//			中内嵌此类对象，管理控件位置
//	修改人：林秋岚
//
//	
//  
// 															┏━┯━┓
// 															┃林│秋┃
// 															┠─┼─┨
// 															┃印│岚┃
// 															┗━┷━┛
//
//////////////////////////////////////////////////////////////////////////

// CtlPosMgrST.h: interface for the CCtlPosMgrST class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTLPOSMGRST_H__B19FC00C_2962_4A24_81A3_4BF20A12F04D__INCLUDED_)
#define AFX_CTLPOSMGRST_H__B19FC00C_2962_4A24_81A3_4BF20A12F04D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)
#include <map>

class CCtlPosMgrST  
{
public:
	CCtlPosMgrST();
	virtual ~CCtlPosMgrST();

// Data
private:
	struct STRETCH_ATTR 
	{
		BOOL _bLock ;
		UINT _nMask ;
		int _nPercentX ;
		int _nPercentY ;
		int _nPercentCX ;
		int _nPercentCY ;
		CRect _rcCtrl ;
		CSize _szParent ;
		STRETCH_ATTR() {}
		STRETCH_ATTR(
			UINT nMask,
			int nPercentX,
			int nPercentY,
			int nPercentCX,
			int nPercentCY,
			const CRect& rcCtrl,
			const CSize& szParent
			)
			: _bLock(FALSE), _nMask(nMask), _nPercentX(nPercentX), _nPercentY(nPercentY), _nPercentCX(nPercentCX), _nPercentCY(nPercentCY),
			_rcCtrl(rcCtrl), _szParent(szParent)
		{}
	};
//	typedef std::map<UINT, STRETCH_ATTR> SAMAP ;
	typedef std::map<HWND, STRETCH_ATTR> SAMAP ;
	SAMAP	m_mapSA ;
	
	HWND	m_hWndHost ;		// 记录宿主窗口句柄
	CSize	m_szHostClient ;	// 记录宿主窗口客户区大小
	
public:
	enum {
		X = 0x1, Y = 0x2, CX = 0x4, CY = 0x8
	} ;
		
// Implementation
public:
	// 设置所寄宿的宿主
	BOOL Create(CWnd *pParentWnd) 
	{
		ASSERT(pParentWnd != NULL) ;

		HWND hWnd = pParentWnd->GetSafeHwnd() ;
		if (::IsWindow(hWnd))
		{
			m_hWndHost = hWnd ;
			CRect rcClient ;
			::GetClientRect(m_hWndHost, rcClient) ;
			m_szHostClient = rcClient.Size() ;
			return TRUE ;
		}
		return FALSE ;
	}
	
	// 添加控件
	BOOL AddMapControl(
		UINT nIDControl,	// 控件ID
		UINT nMask,			// 控件变化掩码：(cy << 3) | (cx << 2) | (y << 1) | (x)
		...					// 按顺序依次提供x、y、cx、cy的变化百分比
		) ;

	BOOL AddMapControl(
		HWND hCtrl,			// 子窗口句柄
		UINT nMask,
		...
		) ;
	
/*	BOOL AddMapControl(
		UINT nIDControl,	// 控件ID
		BOOL bX,			// 控件位置的x是否随父窗口变化
		int nPercentX,		// 变化的百分比
		BOOL bY,			// 控件位置的y是否随父窗口变化
		int nPercentY,		// 变化的百分比
		BOOL bCX,			// 控件位置的cx是否随父窗口变化
		int nPercentCX,		// 变化的百分比
		BOOL bCY,			// 控件位置的cy是否随父窗口变化
		int nPercentCY		// 变化的百分比
		) ;

	BOOL AddMapControl(
		UINT nIDControl,
		UINT nMask,			// 控件变化掩码：(cy << 3) | (cx << 2) | (y << 1) | (x)
		int nPercentX,
		int nPercentY,
		int nPercentCX,
		int nPercentCY
		) ;*/

	BOOL AddMapControl(
		HWND hCtrl,
		UINT nMask,			// 控件变化掩码：(cy << 3) | (cx << 2) | (y << 1) | (x)
		int nPercentX,
		int nPercentY,
		int nPercentCX,
		int nPercentCY
		) ;

	// 移除控件
	BOOL RemoveMapControl(
		UINT nIDControl
		) ;

	// 移除控件
	BOOL RemoveMapControl(
		HWND hCtrl
		) ;

	// 锁定控件
	BOOL LockControl(
		UINT nIDControl
		) ;

	// 解锁控件
	BOOL UnLockControl(
		UINT nIDControl
		) ;

	// 锁定窗口
	BOOL LockControl(
		HWND hCtrl
		) ;

	// 解锁窗口
	BOOL UnLockControl(
		HWND hCtrl
		) ;

	// 更新控件位置
	BOOL UpdateMapControlPos(
		UINT nIDControl		// 控件ID
		) ;

	// 更新控件位置
	BOOL UpdateMapControlPos(
		HWND hCtrl			// 子窗口句柄
		) ;

	// 改变控件位置
	BOOL ChangeMapControlPos(
		UINT nIDControl, UINT nMask, ...
		) ;
	BOOL ChangeMapControlPos(
		HWND hCtrl, UINT nMask, ...
		) ;
	BOOL ChangeMapControlPos(
		HWND hCtrl, UINT nMask, int x, int y, int cx, int cy
		) ;
	
	// 寄宿的情况下响应WM_SIZE消息
 	void HandleOnSize(UINT nType, int cx, int cy) ;

};

#endif // !defined(AFX_CTLPOSMGRST_H__B19FC00C_2962_4A24_81A3_4BF20A12F04D__INCLUDED_)
