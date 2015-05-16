#include "StdAfx.h"
#include "ImageEx.h"
#include "GifPictureMgr.h"
#include "common/utils/ImgConverter.h"
#include "common/MemoryLeakChecker.h"


CGifPictureMgr* ImageEx::m_GifMgr = NULL;

ImageEx::~ImageEx(void)
{
	if ( m_GifMgr )
	{
		m_GifMgr->RemoveAnimatedGif( this );
	}
}

ImageEx::ImageEx(
			 IN const WCHAR* filename,
			 IN GIF_STATUS initStatus /*= STATUS_PLAY*/,
			 IN BOOL useEmbeddedColorManagement /*= FALSE*/
			 )
			 : Gdiplus::Image( filename, useEmbeddedColorManagement ), m_bIsGif( false ),m_gifStatus( initStatus )
{
	if ( m_GifMgr == NULL )
	{
		m_GifMgr = new CGifPictureMgr( );
	}
	if ( m_GifMgr )
	{
		m_GifMgr->TestForAnimatedGIF( this );
	}
}

ImageEx::ImageEx(
			 IN IStream* stream,
			 IN GIF_STATUS initStatus /*= STATUS_PLAY*/,
			 IN BOOL useEmbeddedColorManagement/* = FALSE*/
			 )
			 : Gdiplus::Image( stream, useEmbeddedColorManagement ), m_bIsGif( false ), m_gifStatus( initStatus )
{
	if ( m_GifMgr == NULL )
	{
		m_GifMgr = new CGifPictureMgr( );
	}
	if ( m_GifMgr )
	{
		m_GifMgr->TestForAnimatedGIF( this );
	}
}

ImageEx* ImageEx::FromFile(
							  IN const WCHAR* filename,
							  IN GIF_STATUS initStatus/* = STATUS_PLAY*/,
							  IN BOOL useEmbeddedColorManagement /*= FALSE*/
							  )
{
	return new ImageEx( filename, initStatus, useEmbeddedColorManagement );
}

ImageEx* ImageEx::FromStream(
								IN IStream* stream,
								IN GIF_STATUS initStatus /*= STATUS_PLAY*/,
								IN BOOL useEmbeddedColorManagement /*= FALSE*/
								)
{
	return new ImageEx( stream, initStatus, useEmbeddedColorManagement );
}

bool ImageEx::IsGif()
{
	return m_bIsGif;
}

void ImageEx::Play()
{
	if ( m_GifMgr )
	{
		m_GifMgr->Play( this );
	}
}

void ImageEx::Stop()
{
	if ( m_GifMgr )
	{
		m_GifMgr->Stop( this );
	}
}

void ImageEx::Pause()
{
	if ( m_GifMgr )
	{
		m_GifMgr->Pause( this );
	}
}

bool ImageEx::IsPlaying()
{
	return m_gifStatus == STATUS_PLAY;
}

void ImageEx::ExitGifManager()
{
	if ( m_GifMgr )
	{
		delete m_GifMgr;
		m_GifMgr = NULL;
	}
}

int ImageEx::GetEncoderClsid(const WCHAR* mineFormat, const GUID* guidFormat, CLSID* pClsid)
{
	return ::GetEncoderClsid(mineFormat, guidFormat, pClsid);
}
