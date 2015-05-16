#pragma once

#include "IWndLess.h"

// 定义删除子项通知消息
#define WLCBN_DELITEM		(NM_WLFIRST - 400U)

typedef struct _tagNMCBITEM 
{
	NMWLHDR hdr ;
	int nIndex ;
} NMCBITEM, * LPNMCBITEM ;


class IWLComboBox :  public IWndLess
{
public:
	virtual ~IWLComboBox(void){};


public:
	//////////////////////////////////////////////////////////////////////////
	// Operation

	virtual void LoadImage(
		Image * pImgFrame,
		Image * pImgDropBtn,
		Image * pImgListDelBtn
		)  = 0;

	virtual COLORREF SetFrameColor(COLORREF crFrame)   = 0;

	// 设置、获取提示文字
	virtual void SetCueBanner(LPCTSTR lpszText)  = 0;
	virtual CString GetCurBanner() const  = 0;

	// 添加字符串
	virtual int AddString(LPCTSTR lpszString, DWORD dwData = 0)  = 0;

	// 插入字符串
	virtual int InsertString(UINT nIndex, LPCTSTR lpszString, DWORD dwData = 0)  = 0;

	// 删除节点
	virtual int	DeleteString(UINT nIndex)  = 0;
	virtual void ResetContent()  = 0;
	virtual int GetCount()  = 0;
	virtual HWL SetFocus()  = 0;
	// 获取子项文本
	virtual void GetText(int nIndex, CString& rString) const  = 0;
	// 获得当前选择
	virtual int GetCurSel()  = 0;

	// 设置当前选择
	virtual int SetCurSel(int nSelect)  = 0;

	// 设置、获取子项数据
	virtual int SetItemData(int nIndex, DWORD dwItemData)  = 0;
	virtual DWORD GetItemData(int nIndex) const  = 0;

	// 寻找字符串
	virtual int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const  = 0;

	// 限制编辑框文字数量
	virtual BOOL LimitText(int nMaxChars)  = 0;

	// 设置编辑框选择文本
	virtual void SetEditSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE)  = 0;

	virtual BOOL CanAutoCompleteWord() const  = 0;

	virtual void SetEditMargin(
		int nLeftMargin,
		int nTopMargin,
		int nRightMargin,
		int nBottomMargin
		)  = 0;
	

};

typedef CWndLessImpBase< CWndLessShell<IWLComboBox> >  WLComboBoxImpBase;

template <class IT = IWLComboBox, class IMPT = WLComboBoxImpBase > 
class CWLComboBoxShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLComboBoxShell() {}
	virtual ~CWLComboBoxShell(){}


	virtual void LoadImage(
		Image * pImgFrame,
		Image * pImgDropBtn,
		Image * pImgListDelBtn
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->LoadImage(pImgFrame, pImgDropBtn, pImgListDelBtn);
		}
	};

	virtual COLORREF SetFrameColor(COLORREF crFrame)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetFrameColor(crFrame);
		}
		return 0;
	}

	// 设置、获取提示文字
	virtual void SetCueBanner(LPCTSTR lpszText)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetCueBanner(lpszText);
		}
	}
	virtual CString GetCurBanner() const 
	{
		if (m_wlimp)
		{
			return m_wlimp->GetCurBanner();
		}
		return _T("");
	}

	// 添加字符串
	virtual int AddString(LPCTSTR lpszString, DWORD dwData = 0)
	{
		if (m_wlimp)
		{
			return m_wlimp->AddString(lpszString, dwData);
		}
		return 0;
	}

	// 插入字符串
	virtual int InsertString(UINT nIndex, LPCTSTR lpszString, DWORD dwData = 0)
	{
		if (m_wlimp)
		{
			return m_wlimp->InsertString(nIndex, lpszString, dwData);
		}
		return 0;
	}

	// 删除节点
	virtual int	DeleteString(UINT nIndex)
	{
		if (m_wlimp)
		{
			return m_wlimp->DeleteString(nIndex);
		}
		return 0;
	}
	virtual void ResetContent()
	{
		if (m_wlimp)
		{
			return m_wlimp->ResetContent();
		}
	}
	virtual int GetCount()
	{
		if (m_wlimp)
		{
			return m_wlimp->GetCount();
		}
		return 0;
	}
	virtual HWL SetFocus()
	{
		if (m_wlimp)
		{
			return m_wlimp->SetFocus();
		}
		return NULL;
	}
	// 获取子项文本
	virtual void GetText(int nIndex, CString& rString) const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetText(nIndex, rString);
		}
	}
	// 获得当前选择
	virtual int GetCurSel()
	{
		if (m_wlimp)
		{
			return m_wlimp->GetCurSel();
		}
		return 0;
	}

	// 设置当前选择
	virtual int SetCurSel(int nSelect)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetCurSel(nSelect);
		}
		return 0;
	}

	// 设置、获取子项数据
	virtual int SetItemData(int nIndex, DWORD dwItemData)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetItemData(nIndex, dwItemData);
		}
		return 0;
	}
	virtual DWORD GetItemData(int nIndex) const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetItemData(nIndex);
		}
		return 0;
	}

	// 寻找字符串
	virtual int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
	{
		if (m_wlimp)
		{
			return m_wlimp->FindStringExact(nIndexStart, lpszFind);
		}
		return 0;
	}

	// 限制编辑框文字数量
	virtual BOOL LimitText(int nMaxChars)
	{
		if (m_wlimp)
		{
			return m_wlimp->LimitText(nMaxChars);
		}
		return FALSE;
	}

	// 设置编辑框选择文本
	virtual void SetEditSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetEditSel(nStartChar, nEndChar, bNoScroll);
		}
	}

	virtual BOOL CanAutoCompleteWord() const
	{
		if (m_wlimp)
		{
			return m_wlimp->CanAutoCompleteWord();
		}
		return FALSE;
	}

	virtual void SetEditMargin(
		int nLeftMargin,
		int nTopMargin,
		int nRightMargin,
		int nBottomMargin
		) 
	{
		if (m_wlimp)
		{
			return m_wlimp->SetEditMargin(nLeftMargin, nTopMargin, nRightMargin, nBottomMargin);
		}
	}
	
};

typedef CWLComboBoxShell<> WLComboBox;



