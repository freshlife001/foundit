#include "StdAfx.h"
#include "WLButtonALHelper.h"
#include "WLButtonAL.h"
#include "common/utils/ImageEx/ImageResource.h"
#include "common/utils/ImageEx/ImageEx.h"
#include "common/utils/Graphics.h"
#include "common/MemoryLeakChecker.h"

WLButtonALHelper::WLButtonALHelper(void)
{
	m_mapStyles.insert(std::make_pair(_T("DefPushButton"), new windowless::CDefPushButtnStyle())) ;
	m_mapStyles.insert(std::make_pair(_T("DefCheckBox"), new windowless::CDefCheckBoxStyle())) ;
	m_mapStyles.insert(std::make_pair(_T("DefRadioBox"), new windowless::CDefRadioBoxStyle())) ;
	m_mapStyles.insert(std::make_pair(_T("DefLink"), new windowless::CDefLinkStyle())) ;
}

WLButtonALHelper::~WLButtonALHelper(void)
{
	MAP_STYLE::iterator pos = m_mapStyles.begin() ;
	for ( ; pos != m_mapStyles.end() ; ++ pos)
	{
		delete pos->second ;
	}
}

WLButtonALHelper * WLButtonALHelper::s_pInstance = NULL ;

void WLButtonALHelper::Initialize() 
{
	if (s_pInstance == NULL)
	{
		s_pInstance = new WLButtonALHelper() ;
	}
}

void WLButtonALHelper::UnInitialize() 
{
	if (s_pInstance != NULL)
	{
		delete s_pInstance ;
		s_pInstance = NULL ;
	}
}

BOOL WLButtonALHelper::Process(WLButtonAL & button, LPCTSTR sStyle) 
{
	if (s_pInstance == NULL)
		return FALSE ;

	MAP_STYLE::iterator pos = s_pInstance->m_mapStyles.find(sStyle) ;
	if (pos == s_pInstance->m_mapStyles.end())
		return FALSE ;

	pos->second->Process(button) ;

	return TRUE ;
}
BOOL WLButtonALProcess(WLButtonAL & button, LPCTSTR sStyle)
{
	 return WLButtonALHelper::Process(button, sStyle);
}

namespace windowless
{

// 缺省按钮
CDefPushButtnStyle::CDefPushButtnStyle() 
{
	m_imgFrgnd = Shared_ImageEx::FromId(IDI_PUSHBUTTON_PNG) ;
	//m_pImgFrgnd = new Image(_T("F:\\Program\\Wndless\\ButtonDemo\\pushbutton.png")) ;
}

CDefPushButtnStyle::~CDefPushButtnStyle() 
{
	//delete m_pImgFrgnd ;
	//m_pImgFrgnd = NULL ;
}

void CDefPushButtnStyle::Process(WLButtonAL & button)  
{
	button.SetStyle(WLButtonAL::FITFRGNDSIZE, TRUE) ;
	button.SetTextColor(
		WLButtonAL::BCI_ALL,
		hdutils::crTextColor,
		hdutils::crTextColor,
		hdutils::crTextColor,
		hdutils::crTextColor,
		RGB(161, 161, 146)		// disable
		) ;
	button.SetFrgnd(m_imgFrgnd, 5, WLButtonAL::FITX, FALSE) ;
}

// 缺省checkbox
CDefCheckBoxStyle::CDefCheckBoxStyle() 
{
//	m_pImgCheckSign = new Image(_T("skin\\check.bmp")) ;
	m_imgCheckSign = Shared_ImageEx::FromId(IDI_CHECKBOX_BMP) ;
	//m_pImgCheckSign = new Image(_T("F:\\Program\\Wndless\\ButtonDemo\\checkbox.bmp")) ;
}

CDefCheckBoxStyle::~CDefCheckBoxStyle() 
{
	//delete m_pImgCheckSign ;
	//m_pImgCheckSign = NULL ;
}

void CDefCheckBoxStyle::Process(WLButtonAL & button)  
{
	button.SetSign(m_imgCheckSign, TRUE) ;
	button.SetBkgndStyle(WLButtonAL::NOBKGND, FALSE) ;
	button.SetTextColor(
		WLButtonAL::BCI_ALL,
		hdutils::crTextColor,			// unchecked
		hdutils::crTextColor,			// unchecked hot
		hdutils::crTextColor,			// unchecked pushed
		RGB(161, 161, 146),				// unchecked disable
		hdutils::crTextColor,			// checked
		hdutils::crTextColor,			// checked hot
		hdutils::crTextColor,			// checked pushed
		RGB(161, 161, 146)				// checked disable
		) ;
	button.EndDeferRecalcLayout() ;
}

// 缺省radiobox
CDefRadioBoxStyle::CDefRadioBoxStyle() 
{
	m_imgRadioSign = Shared_ImageEx::FromId(IDI_RADIOBOX_PNG) ;
	//m_pImgRadioSign = new Image(_T("F:\\Program\\Wndless\\ButtonDemo\\radiobox.png")) ;
}

CDefRadioBoxStyle::~CDefRadioBoxStyle() 
{
	//delete m_pImgRadioSign ;
	//m_pImgRadioSign = NULL ;
}

void CDefRadioBoxStyle::Process(WLButtonAL & button)  
{
	button.SetSign(m_imgRadioSign, TRUE) ;
	button.SetBkgndStyle(WLButtonAL::NOBKGND, FALSE) ;
	button.SetTextColor(
		WLButtonAL::BCI_ALL,
		hdutils::crTextColor,			// unchecked
		hdutils::crTextColor,			// unchecked hot
		hdutils::crTextColor,			// unchecked pushed
		RGB(161, 161, 146),				// unchecked disable
		hdutils::crTextColor,			// checked
		hdutils::crTextColor,			// checked hot
		hdutils::crTextColor,			// checked pushed
		RGB(161, 161, 146)				// checked disable
		) ;
	button.EndDeferRecalcLayout() ;
}

// 缺省链接
CDefLinkStyle::CDefLinkStyle()
{

}

CDefLinkStyle::~CDefLinkStyle()
{

}

void CDefLinkStyle::Process(WLButtonAL & button) 
{
	button.SetType(WLButtonAL::LINK, TRUE) ;
	button.SetTextColor(
		WLButtonAL::BCI_ALL,
		RGB(0x00, 0x88, 0xdc),
		RGB(0x00, 0x88, 0xdc),
		RGB(0x00, 0x88, 0xdc),
		RGB(0x00, 0x88, 0xdc),
		RGB(161, 161, 146)		// disable
		) ;
	button.EndDeferRecalcLayout() ;
}

} ;
