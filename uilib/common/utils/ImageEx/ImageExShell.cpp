#include "StdAfx.h"
#include "ImageExShell.h"
#include "ImageEx.h"
#include <boost/function.hpp>
#include "ImageManager.h"
#include "common/MemoryLeakChecker.h"


ImageExShell::ImageExShell( ImageEx* pImg, bool bIsShared )
:m_pImageEx( pImg ),
m_bIsShared( bIsShared ),
m_nImgId(0)
{
}

ImageExShell::ImageExShell( ImageEx* pImg, boost::function<void()> fn, bool bIsShared )
:m_pImageEx( pImg ),
m_bIsShared( bIsShared ),
m_nImgId(0)
{
	m_conn = m_pImageEx->AnimUpdatedEvent.connect( fn );
}

ImageExShell::~ImageExShell(void)
{
	m_conn.disconnect();
	if ( m_bIsShared )
	{
		ImageManager::get_mutable_instance().ReleaseImage( m_pImageEx );
	}
	else
	{
		delete m_pImageEx;
	}
	m_pImageEx = NULL;
}

void ImageExShell::SetCallbackFunction( boost::function<void()> fn )
{
	ASSERT( m_pImageEx );
	if ( m_pImageEx )
	{
		m_conn.disconnect();
		m_conn = m_pImageEx->AnimUpdatedEvent.connect( fn );
	}
}

/////////// create shared imageex
ImageExShell* ImageExShell::FromId( UINT nImgId )
{
	ImageExShell* pImgShell = new ImageExShell( ImageManager::get_mutable_instance().GetImageFromId( nImgId ), true );
	pImgShell->m_nImgId = nImgId;
	return pImgShell;
}
//////create shared imageex
ImageExShell* ImageExShell::FromFile( LPCTSTR lpFileName )
{
	ImageExShell* pImgShell = new ImageExShell( ImageManager::get_mutable_instance().GetImageFromPath( lpFileName ), true );
	return pImgShell;
}

ImageExShell* ImageExShell::FromHICON( HICON hIcon)
{
	ImageExShell* pImgShell = new ImageExShell( ImageManager::get_mutable_instance().GetImageFromHICON( hIcon ), true );
	return pImgShell;
}