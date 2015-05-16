#include "StdAfx.h"
#include "ImageManager.h"
#include "../ImageEx/ImageEx.h"
#include "utils/utils.h"
#include "utils/strutils.h"

#include "common/utils/graphics.h"
#include "common/utils/UIUtils.h"
#include "logex.h"
#include "common/MemoryLeakChecker.h"


ImageManager::ImageManager()
:m_hResourceModule( NULL )
{
}
ImageManager::~ImageManager(void)
{
	if ( m_hResourceModule )
	{
		FreeLibrary( m_hResourceModule );
	}
}

ImageEx* ImageManager::GetImageFromId( int nImageId )
{
	ImageEx* pic = NULL;
	std::map<int, ImageEx*>::iterator itFind = m_mapImageId.find( nImageId );
	if ( itFind != m_mapImageId.end() )
	{
		LOG_TRACE( L"GetImageFromId, Image already exist");
		pic = itFind->second;
		m_mapReference[pic]++;
		LOG_TRACE( L"ImageReference: Add-" << pic << L":" << m_mapReference[pic] );
	}
	else
	{
		LOG_MEMORY_SCOPE();
		if ( m_hResourceModule == NULL )
		{
			m_hResourceModule = LoadLibrary(L"res.dll");
		}
		ASSERT( m_hResourceModule );
		if ( m_hResourceModule == NULL )
		{
			AfxMessageBox( _T("缺少资源文件，请重新下载安装"));

			CString feedback_url;
			feedback_url.Format(_T("http://baidu.com/%s"), U8TOW(LibIM::GetMyVersionStr()).c_str());
			ShellExecute(NULL,_T("open"),feedback_url, NULL, NULL, SW_SHOW );
			::ExitProcess( -1 );
			return NULL;
		}
		pic = LoadImageFromResource( m_hResourceModule, L"IMAGE", nImageId );
		ASSERT( pic && pic->GetLastStatus() == Ok );
		if ( !( pic && pic->GetLastStatus() == Ok ))
		{
			AfxMessageBox( _T("缺少资源文件，请重新下载安装"));
			CString feedback_url;
			feedback_url.Format(_T("http://baidu.com/%s"), U8TOW(LibIM::GetMyVersionStr()).c_str());
			ShellExecute(NULL,_T("open"),feedback_url, NULL, NULL, SW_SHOW );
			::ExitProcess( -1 );
			return NULL;
		}
		if ( pic != NULL )
		{
			m_mapImageId.insert( std::pair<int,ImageEx*>( nImageId, pic ));
			m_mapReference[pic] = 1;
			LOG_TRACE( L"ImageReference: Add-" << pic << L":" << m_mapReference[pic] );
		}
	}
	return pic;
}

ImageEx* ImageManager::GetImageFromPath( const std::wstring& strImagePath )
{
	ImageEx* pic = NULL;
	std::map<std::wstring, ImageEx*>::iterator itFind = m_mapImageStr.find( strImagePath );
	if ( itFind != m_mapImageStr.end())
	{
		pic = itFind->second;
		m_mapReference[pic]++;
	}
	else
	{
		LOG_MEMORY_SCOPE();
		pic = ImageEx::FromFile( strImagePath.c_str() );
	//	ASSERT( pic && pic->GetLastStatus() == Gdiplus::Ok );
		if ( NULL != pic )
		{
			m_mapImageStr.insert( std::pair<std::wstring,ImageEx*>( strImagePath, pic ));
			m_mapReference[pic] = 1;
		}
	}
	return pic;
}
ImageEx* ImageManager::GetImageFromHICON( HICON hIcon )
{
	LOG_MEMORY_SCOPE();

	ICONINFO iconInfo;
	::GetIconInfo(hIcon, & iconInfo);
	::DeleteObject(iconInfo.hbmMask); 

	IStream* pIStream = NULL;
	HGLOBAL bitmapBuff = hdutils::ConvertDDB2DIB(iconInfo.hbmColor);
	//::DeleteObject(iconInfo.hbmColor);

	if(CreateStreamOnHGlobal(iconInfo.hbmColor, TRUE, &pIStream)==S_OK)
	{
		ImageEx *pImage = ImageEx::FromStream(pIStream);
		ASSERT( pImage->GetLastStatus() == Gdiplus::Ok );
		pIStream->Release();
		return pImage;
	}

	return NULL;

}

void ImageManager::ReleaseImage( ImageEx* pImage )
{
	std::map<ImageEx*, int>::iterator itFind = m_mapReference.find( pImage );
	if ( m_mapReference.end() != itFind )
	{
		if ( --(itFind->second) <= 0 )
		{
			bool bFound = false;
			std::map<std::wstring,ImageEx*>::iterator itLoop = m_mapImageStr.begin();
			for ( ; itLoop != m_mapImageStr.end(); ++itLoop )
			{
				if ( itLoop->second == pImage )
				{
					LOG_TRACE( L"Delete Image:" << L"File: "<< itLoop->first );
					delete pImage;
					m_mapImageStr.erase( itLoop );
					bFound = true;
					break;
				}
			}
			if ( !bFound )
			{
				std::map<int,ImageEx*>::iterator itLoop = m_mapImageId.begin();
				for ( ; itLoop != m_mapImageId.end(); ++itLoop )
				{
					if ( itLoop->second == pImage )
					{
						LOG_TRACE( L"Delete Image:" << L"File: " << itLoop->first );
						delete pImage;
						m_mapImageId.erase( itLoop );
						bFound = true;
						break;
					}
				}
			}
			m_mapReference.erase( itFind );
		}
	}
}

void ImageManager::AddImageReference( ImageEx* pImage )
{
	std::map<ImageEx*, int>::iterator itFind = m_mapReference.find( pImage );
	if ( m_mapReference.end() != itFind )
	{
		++(itFind->second);
	}
}

ImageEx* ImageManager::LoadImageFromResource( HMODULE hModule, LPCTSTR lpResType, DWORD nResourceID )
{
	LPCTSTR pResourceName = MAKEINTRESOURCE(nResourceID);
	return LoadImageFromResource( hModule, lpResType, pResourceName );
}

ImageEx* ImageManager::LoadImageFromResource( HMODULE hModule, LPCTSTR lpResType, LPCTSTR pResourceName )
{
	HRSRC hResource = FindResource( hModule,  pResourceName, lpResType );
	if(!hResource)
	{
		return NULL;
	}
	DWORD dwResourceSize = SizeofResource( hModule, hResource);
	if(!dwResourceSize)
	{
		return NULL;
	}
	const void* pResourceData = LockResource(LoadResource( hModule, hResource));
	if(!pResourceData)
	{
		return NULL;
	}
	HGLOBAL hResourceBuffer = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
	if(!hResourceBuffer)
	{
		GlobalFree(hResourceBuffer);
		return NULL;
	}
	void* pResourceBuffer = GlobalLock(hResourceBuffer);
	if(!pResourceBuffer)
	{
		GlobalUnlock(hResourceBuffer);
		GlobalFree(hResourceBuffer);
		return NULL;
	}

	CopyMemory(pResourceBuffer, pResourceData, dwResourceSize);
	GlobalUnlock(hResourceBuffer);

	IStream* pIStream = NULL;
	if(CreateStreamOnHGlobal(hResourceBuffer, TRUE, &pIStream)==S_OK)
	{
		ImageEx *pImage = ImageEx::FromStream(pIStream);
		ASSERT( pImage->GetLastStatus() == Gdiplus::Ok );
		pIStream->Release();
		//GlobalFree(hResourceBuffer);
		return pImage;
	}
	return NULL;
}