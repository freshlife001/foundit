#include "StdAfx.h"
#include "ImageEx/Shared_ImageEx.h"
#include "ImageExShell.h"
#include "ImageEx.h"
#include "ImageManager.h"
#include "../Graphics.h"
#include "logex.h"
#include "common/MemoryLeakChecker.h"

class Shared_ImageExImp: public boost::shared_ptr<ImageExShell>
{

};

Shared_ImageEx::Shared_ImageEx( ImageEx* pImg /*= NULL*/ )
{
	m_pImp = new Shared_ImageExImp;
	m_pImp->reset(new ImageExShell( pImg ));
}

Shared_ImageEx::Shared_ImageEx( ImageEx* pImg, boost::function<void()> fn )
{
	m_pImp = new Shared_ImageExImp;
	m_pImp->reset(new ImageExShell( pImg, fn ));
}

Shared_ImageEx::~Shared_ImageEx(void)
{
	delete m_pImp;
}

Shared_ImageEx::Shared_ImageEx( const Shared_ImageEx &rhs)
{
	m_pImp = new Shared_ImageExImp;
	*m_pImp = *rhs.m_pImp;
}

Shared_ImageEx & Shared_ImageEx::operator=( const Shared_ImageEx &rhs)
{
	if ( &rhs == this )
	{
		return *this;
	}
	*m_pImp = *rhs.m_pImp;
	return *this ;
}

Shared_ImageEx::operator ImageEx*() const
{
	return m_pImp->get()->get();
}

void Shared_ImageEx::Reset()
{
	m_pImp->reset( new ImageExShell( NULL ));
}

void Shared_ImageEx::SetCallBackFunction( boost::function<void()> fn )
{
	m_pImp->get()->SetCallbackFunction( fn );
}

bool Shared_ImageEx::IsNull()
{
	return ( m_pImp->get()->get() == NULL );
}


// shared image data, load image from dll
Shared_ImageEx Shared_ImageEx::FromId( UINT nSourceId )
{
	Shared_ImageEx img;
	LOG_TRACE(L"LoadImage From Id: " << nSourceId );
	img.m_pImp->reset( ImageExShell::FromId( nSourceId ));
	return img;
}

Shared_ImageEx Shared_ImageEx::FromFile( LPCTSTR lpFileName )
{
	Shared_ImageEx img;
	ASSERT( lpFileName );
	if ( lpFileName )
	{
		LOG_TRACE( L"LoadImage From File: " << lpFileName );
		img.m_pImp->reset(  ImageExShell::FromFile( lpFileName ));
	}
	return img;
}

Shared_ImageEx Shared_ImageEx::FromHICON( HICON hIcon )
{
	Shared_ImageEx img;
	ASSERT( hIcon );
	if ( hIcon )
	{
		LOG_TRACE( L"LoadImage From hIcon: " << hIcon );
		img.m_pImp->reset(  ImageExShell::FromHICON( hIcon ));
	}
	return img;
}

void Shared_ImageEx::FillBitmap(CBitmap &bitmap)
{
	ImageEx * img = m_pImp->get()->get();
	if (img)
	{
		int width = img->GetWidth(); // sourceImg is Image*
		int height = img->GetHeight();

		HDC dcDesktop = ::GetDC(NULL);
		HDC dcMem = ::CreateCompatibleDC(dcDesktop) ;

		HBITMAP hBitmap, hOldBitmap;

		hBitmap = CreateCompatibleBitmap(dcDesktop, width, height);

		hdutils::RenderBitmap(hBitmap, RGB(255,255,255));

		hOldBitmap = (HBITMAP)SelectObject(dcMem, hBitmap);
		Graphics gr(dcMem);
		gr.DrawImage(img,0,0,width,height);
		
		::SelectObject(dcMem, hOldBitmap);
		::DeleteDC(dcMem);
		::ReleaseDC(NULL, dcDesktop);
		
		bitmap.Attach(hBitmap);

	}
};