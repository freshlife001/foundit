#pragma once
#include "boost/signals2/connection.hpp"
class ImageEx;
class Shared_ImageEx;
class ImageExShell
{
	friend class Shared_ImageEx;
public:
	~ImageExShell(void);
private:
	ImageExShell( ImageEx* pImg, bool bIsShared = false );
	ImageExShell( ImageEx* pImg, boost::function<void()> fn, bool bIsShared = false );

	void SetCallbackFunction( boost::function<void()> fn );

	ImageEx* get() const { return m_pImageEx; }

	static ImageExShell* FromId( UINT nImgId );
	static ImageExShell* FromFile( LPCTSTR lpFileName );
	static ImageExShell* FromHICON( HICON hIcon);

private:
	boost::signals2::connection m_conn;
	UINT m_nImgId;
	ImageEx* m_pImageEx;
	bool m_bIsShared;
};
