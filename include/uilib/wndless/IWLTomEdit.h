#pragma once
#include <RichOle.h>
#include "IWndLess.h"


class IWLTomEdit : public IWndLess
{

public:
	virtual ~IWLTomEdit(void){};

	// Operations
public:
	virtual bool SetBackgroundTransparent(bool transparent) = 0 ;

	virtual COLORREF SetBackgroundColor(bool use_system_color, COLORREF color)  = 0 ;
	virtual COLORREF GetBackgroundColor() const  = 0;
	virtual COLORREF SetHighLightTextColor(bool use_system_color, COLORREF color)  = 0 ;
	virtual COLORREF GetHighLightTextColor() const  = 0;
	virtual COLORREF SetHighLightTextBackgroundColor(bool use_system_color, COLORREF color)  = 0;
	virtual COLORREF GetHighLightTextBackgroundColor() const  = 0;

	virtual IRichEditOle * GetIRichEditOle()  = 0;
	virtual bool SetOLECallback(IRichEditOleCallback* pCallback)  = 0;

	virtual void EmptyUndoBuffer()  = 0;
	virtual WORD GetSelectionType() const  = 0;

	virtual HRESULT GetObject(LONG iob, REOBJECT * reobject, DWORD flags)  = 0;
	virtual BOOL InsertImageObject(LONG char_position, std::wstring const image_file_path)  = 0;

	virtual void ReplaceSel(wchar_t const * new_text, BOOL can_undo = FALSE)  = 0;

	virtual void RemoveObject(IOleObject * object)  = 0;

};

typedef CWndLessImpBase< CWndLessShell<IWLTomEdit> >  WLTomEditImpBase;

template <class IT = IWLTomEdit, class IMPT = WLTomEditImpBase > 
class CWLTomEditShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLTomEditShell() {}
	virtual ~CWLTomEditShell(){}
	bool SetBackgroundTransparent(bool transparent)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetBackgroundTransparent(transparent);
		}
		return false;
	};

	COLORREF SetBackgroundColor(bool use_system_color, COLORREF color)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetBackgroundColor(use_system_color, color);
		}
		return 0;
	};
	COLORREF GetBackgroundColor() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetBackgroundColor();
		}
		return 0;
	};
	COLORREF SetHighLightTextColor(bool use_system_color, COLORREF color)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetHighLightTextColor(use_system_color, color);
		}
		return 0;
	};
	COLORREF GetHighLightTextColor() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetHighLightTextColor();
		}
		return 0;
	};
	COLORREF SetHighLightTextBackgroundColor(bool use_system_color, COLORREF color)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetHighLightTextBackgroundColor(use_system_color, color);
		}
		return 0;
	};
	COLORREF GetHighLightTextBackgroundColor() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetHighLightTextBackgroundColor();
		}
		return 0;
	};

	IRichEditOle * GetIRichEditOle()
	{
		if (m_wlimp)
		{
			return m_wlimp->GetIRichEditOle();
		}
		return NULL;
	};
	bool SetOLECallback(IRichEditOleCallback* pCallback)
	{
		if (m_wlimp)
		{
			return m_wlimp->SetOLECallback(pCallback);
		}
		return false;
	};

	void EmptyUndoBuffer()
	{
		if (m_wlimp)
		{
			return m_wlimp->EmptyUndoBuffer();
		}
	};
	WORD GetSelectionType() const
	{
		if (m_wlimp)
		{
			return m_wlimp->GetSelectionType();
		}
		return 0;
	};

	HRESULT GetObject(LONG iob, REOBJECT * reobject, DWORD flags)
	{
		if (m_wlimp)
		{
			return m_wlimp->GetObject(iob, reobject, flags);
		}
		return S_FALSE;
	};
	BOOL InsertImageObject(LONG char_position, std::wstring const image_file_path)
	{
		if (m_wlimp)
		{
			return m_wlimp->InsertImageObject(char_position, image_file_path);
		}
		return FALSE;
	};

	void ReplaceSel(wchar_t const * new_text, BOOL can_undo)
	{
		if (m_wlimp)
		{
			return m_wlimp->ReplaceSel(new_text, can_undo);
		}
	};

	void RemoveObject(IOleObject * object)
	{
		if (m_wlimp)
		{
			return m_wlimp->RemoveObject(object);
		}
	}
};

typedef CWLTomEditShell<> WLTomEdit;



