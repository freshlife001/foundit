#pragma once
#include "IWndLess.h"
#include "common/utils/ImageEx/Shared_ImageEx.h"

class IWLAnimation : public IWndLess
{
public:
	virtual ~IWLAnimation(void){};

public:
	enum STYLE
	{
		AS_AUTOFITIMAGE		= 0x00000001,	// 自适应图像大小
		AS_CMBD				= 0x00000002,	// 组合动画
	};
	//////////////////////////////////////////////////////////////////////////
	// Operation
	virtual BOOL Open(LPCTSTR lpszPath) = 0;
	virtual BOOL Open(UINT nID) = 0;
	virtual BOOL Open(Shared_ImageEx &imgAnimation) = 0;

	virtual BOOL Open(LPCTSTR lpszPath, UINT uCol, UINT uRow) = 0;
	virtual BOOL Open(UINT nID, UINT uCol, UINT uRow) = 0;
	virtual BOOL Open(Shared_ImageEx &imgAnimation, UINT uCol, UINT uRow) = 0;

	virtual void Close() = 0;

	virtual UINT SetPlayElapse(UINT uPlayElapse) = 0;

	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual BOOL IsValid() = 0;

	virtual BOOL MakeGray(BOOL make_gray) = 0;
};

typedef CWndLessImpBase< CWndLessShell<IWLAnimation> >  WLAnimationImpBase;

template <class IT = IWLAnimation, class IMPT = WLAnimationImpBase > 
class CWLAnimationShell : public CWndLessShell<IT, IMPT> 
{
public:
	CWLAnimationShell() {}
	virtual ~CWLAnimationShell(){}

public:
	virtual BOOL Open(LPCTSTR lpszPath)
	{
		if(m_wlimp)
		{
			return m_wlimp->Open(lpszPath);
		}
		return FALSE;
	}
	virtual BOOL Open(UINT nID)
	{
		if(m_wlimp)
		{
			return m_wlimp->Open(nID);
		}
		return FALSE;
	}
	virtual BOOL Open(Shared_ImageEx &imgAnimation)
	{
		if(m_wlimp)
		{
			return m_wlimp->Open(imgAnimation);
		}
		return FALSE;
	}

	virtual BOOL Open(LPCTSTR lpszPath, UINT uCol, UINT uRow)
	{
		if(m_wlimp)
		{
			return m_wlimp->Open(lpszPath, uCol, uRow);
		}
		return FALSE;
	}
	virtual BOOL Open(UINT nID, UINT uCol, UINT uRow)
	{
		if(m_wlimp)
		{
			return m_wlimp->Open(nID, uCol, uRow);
		}
		return FALSE;
	}
	virtual BOOL Open(Shared_ImageEx &imgAnimation, UINT uCol, UINT uRow)
	{
		if(m_wlimp)
		{
			return m_wlimp->Open(imgAnimation, uCol, uRow);
		}
		return FALSE;
	}

	virtual void Close()
	{
		if(m_wlimp)
		{
			return m_wlimp->Close();
		}
		return;
	}

	virtual UINT SetPlayElapse(UINT uPlayElapse)
	{
		if(m_wlimp)
		{
			return m_wlimp->SetPlayElapse(uPlayElapse);
		}
		return 0;
	}

	virtual void Play()
	{
		if(m_wlimp)
		{
			return m_wlimp->Play();
		}
		return;
	}
	virtual void Stop()
	{
		if(m_wlimp)
		{
			return m_wlimp->Stop();
		}
		return;
	}
	virtual BOOL IsValid()
	{
		if(m_wlimp)
		{
			return m_wlimp->IsValid();
		}
		return FALSE;
	}

	virtual BOOL MakeGray(BOOL make_gray)
	{
		if(m_wlimp)
		{
			return m_wlimp->MakeGray(make_gray);
		}
		return FALSE;
	}
};

typedef CWLAnimationShell<> WLAnimation;