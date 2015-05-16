#pragma once

#include <boost/serialization/singleton.hpp>
#include <map>
#include "ImageResource.h"

class ImageEx;
class ImageManager : public boost::serialization::singleton<ImageManager>
{
	friend class boost::serialization::singleton<ImageManager>;
	friend class boost::serialization::detail::singleton_wrapper<ImageManager>;
public:
	ImageEx* GetImageFromId( int nImageId );
	ImageEx* GetImageFromPath( const std::wstring& strImagePath );
	ImageEx* GetImageFromHICON( HICON icon );

	void ReleaseImage( ImageEx* pImage );
	void AddImageReference( ImageEx* pImage );

private:
	ImageEx* LoadImageFromResource( HMODULE hModule, LPCTSTR lpResType, DWORD nResourceID );
	ImageEx* LoadImageFromResource( HMODULE hModule, LPCTSTR lpResType, LPCTSTR pResourceName );

private:
	ImageManager();
	~ImageManager(void);

private:
	std::map<ImageEx*, int> m_mapReference;
	std::map<std::wstring, ImageEx*> m_mapImageStr;
	std::map<int,ImageEx*> m_mapImageId;

	HMODULE m_hResourceModule;
};
