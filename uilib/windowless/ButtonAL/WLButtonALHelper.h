#pragma once
#include <map>
#include "common\utils\ImageEx\Shared_ImageEx.h"
#include "wndless\IWLButtonAL.h"

namespace windowless
{
	class IStyle ;
} ;

class WLButtonALHelper
{
private:
	WLButtonALHelper(void);
	~WLButtonALHelper(void);

protected:
	static WLButtonALHelper * s_pInstance ;

	typedef std::map<CString, windowless::IStyle*> MAP_STYLE ;
	MAP_STYLE m_mapStyles ;

public:
	static void Initialize() ;
	static void UnInitialize() ;
	UILIB_EXPORT static BOOL Process(WLButtonAL & button, LPCTSTR sStyle) ;
};

namespace windowless
{

class IStyle
{
public:
	virtual ~IStyle(){};
	virtual void Process(WLButtonAL & button) = 0 ;
};

class CDefPushButtnStyle : public IStyle
{
public:
	CDefPushButtnStyle() ;
	virtual ~CDefPushButtnStyle() ;

protected:
	//Image* m_pImgFrgnd ;
	Shared_ImageEx m_imgFrgnd ;

public:
	virtual void Process(WLButtonAL & button) ;
};

class CDefCheckBoxStyle : public IStyle
{
public:
	CDefCheckBoxStyle() ;
	virtual ~CDefCheckBoxStyle() ;

protected:
//	Image * m_pImgCheckSign ;
	//Image* m_pImgCheckSign ;
	Shared_ImageEx m_imgCheckSign ;

public:
	virtual void Process(WLButtonAL & button) ;
};

class CDefRadioBoxStyle : public IStyle
{
public:
	CDefRadioBoxStyle() ;
	virtual ~CDefRadioBoxStyle() ;

protected:
	//Image* m_pImgRadioSign ;
	Shared_ImageEx m_imgRadioSign ;

public:
	virtual void Process(WLButtonAL & button) ;
};

class CDefLinkStyle : public IStyle
{
public:
	CDefLinkStyle() ;
	virtual ~CDefLinkStyle() ;

protected:
public:
	virtual void Process(WLButtonAL & button) ;
};

};
