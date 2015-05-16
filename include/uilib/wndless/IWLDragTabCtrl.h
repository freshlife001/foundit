#pragma once

#include "IWLTabCtrl.h"

#define TCN_TAB_DRAG_BEGIN         (TCN_FIRST - 5)
#define TCN_DROP_TAB_ITEM         (TCN_FIRST - 6)
#define TCN_TAB_DRAG_ENTER		(TCN_FIRST - 7)
#define TCN_TAB_DRAG_LEAVE         (TCN_FIRST - 8)
#define TCN_ADD_BN_CLICKED			( TCN_FIRST - 9 )
#define TCN_CLOSE_BN_CLICKED			( TCN_FIRST - 30 )

class IWLDragTabCtrl :  virtual public IWLTabCtrl
{
public:
	virtual ~IWLDragTabCtrl(void){};

public:
	struct DRAGPARAM
	{
		CSize m_ptOffset;
		//TCHAR	m_sText[MAX_PATH] ;
		//TCHAR m_sTipText[MAX_PATH] ;
		//Image * m_pImgIcon ;
		//UINT m_uTextStyle;
		DWORD_PTR m_dwData ;
	};
	typedef struct tagNMWLTABDRAGITEM
	{
		NMWLHDR hdr ;
		DRAGPARAM dragParam;
	}NMWLTABDRAGITEM, * LPNMWLTABDRAGITEM;

	typedef struct tagNMWLTABDRAGEVENT
	{
		NMWLTABDRAGITEM hdr ;
		UINT nEnterIndex;
	}NMWLTABDRAGEVENT, * LPNMWLTABDRAGEVENT;

	virtual void SetAddBtnSpace( int nAddBtnSpace ) = 0;
	virtual void SetAddBtnTipString( const CString& tipString ) = 0;

	virtual void SetTabItemMinSize( int nMinSize ) = 0;

	virtual UINT GetMinimizeSize( ) = 0;

};

typedef CWndLessImpBase< CWndLessShell<IWLDragTabCtrl> >  WLDragTabCtrlImpBase;

template <class IT = IWLDragTabCtrl, class IMPT = WLDragTabCtrlImpBase > 
class CWLDragTabCtrlShell : public CWLTabCtrlShell<IT, IMPT> 
{
public:
	CWLDragTabCtrlShell() {}
	virtual ~CWLDragTabCtrlShell(){}

	virtual void SetAddBtnSpace( int nAddBtnSpace ) 
	{
		if (m_wlimp)
		{
			m_wlimp->SetAddBtnSpace(nAddBtnSpace);
		}
	};
	virtual void SetAddBtnTipString( const CString& tipString )
	{
		if (m_wlimp)
		{
			m_wlimp->SetAddBtnTipString(tipString);
		}
	};

	virtual void SetTabItemMinSize( int nMinSize ) 
	{
		if (m_wlimp)
		{
			m_wlimp->SetTabItemMinSize(nMinSize);
		}
	};


	virtual UINT GetMinimizeSize( )  
	{
		if (m_wlimp)
		{
			return m_wlimp->GetMinimizeSize();
		}
		return 0;
	};
	
	
};

typedef CWLDragTabCtrlShell<> WLDragTabCtrl;



