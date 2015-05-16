#include "StdAfx.h"
#include <boost/scoped_array.hpp>

#include "GifPictureMgr.h"
#include "ImageEx.h"
#include "logex.h"

#include "common/MemoryLeakChecker.h"

CGifPictureMgr::CGifPictureMgr(void)
{
	m_hExitEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	//m_gifAnimThread.reset(new boost::thread(&CGifPictureMgr::GifAnimateThread, this ));
	start();
}

CGifPictureMgr::~CGifPictureMgr(void)
{
	LOG_FUNC_SCOPE();
	::SetEvent( m_hExitEvent );
	//m_gifAnimThread->join();
	//m_gifAnimThread.reset();
	stop(1000);
	::CloseHandle( m_hExitEvent );
}

void CGifPictureMgr::TestForAnimatedGIF( ImageEx* picture )
{
	LOG_TRACE_C("pic_ptr:" << picture);
	_GIF_PROPERTY gifPro;
	if ( GetGifProperty( picture, gifPro ))
	{
		//boost::mutex::scoped_lock lock( m_threadMutex );
		picture->m_bIsGif = true;
		std::map<ImageEx*,_GIF_PROPERTY>::iterator itFind = m_gifPicMap.find( picture );
		if ( m_gifPicMap.end() == itFind )
		{
			m_gifPicMap.insert( std::pair<ImageEx*, _GIF_PROPERTY>( picture, gifPro ));
		}
	}
}

void CGifPictureMgr::RemoveAnimatedGif( ImageEx* picture )
{
	LOG_TRACE_C("pic_ptr:" << picture);
	//boost::mutex::scoped_lock lock( m_threadMutex );
	std::map<ImageEx*,_GIF_PROPERTY>::iterator itFind = m_gifPicMap.find( picture );
	if ( m_gifPicMap.end() != itFind )
	{
		free( itFind->second.m_propertyItem );
		m_gifPicMap.erase( itFind );
	}
	else
	{
		ASSERT( !picture->m_bIsGif );
	}
}

bool CGifPictureMgr::IsGifPicture( ImageEx* picture )
{
	return GetTotalFrameCount( picture ) > 1;
}

// get total count of frames
int CGifPictureMgr::GetTotalFrameCount( ImageEx* picture ) 
{
	UINT count = 0;
	count = picture->GetFrameDimensionsCount();
	boost::scoped_array<GUID> pDimensionIDs( new GUID[count] );
	picture->GetFrameDimensionsList( pDimensionIDs.get(), count );
	return picture->GetFrameCount(&pDimensionIDs[0]);
}

void CGifPictureMgr::thread_proc()
{
	GifAnimateThread();
}

void CGifPictureMgr::GifAnimateThread()
{
	while( true )
	{
		DWORD waitRes = ::WaitForSingleObject( m_hExitEvent, 50 ); // playing animation precision
		if ( WAIT_OBJECT_0 == waitRes )
		{
			break;
		}
		else if ( WAIT_TIMEOUT == waitRes )
		{
			//boost::mutex::scoped_lock lock( m_threadMutex );
			if ( m_gifPicMap.empty())
			{
				continue;
			}
			boost::posix_time::ptime curTime = boost::posix_time::microsec_clock::local_time();
			std::map<ImageEx*,_GIF_PROPERTY>::iterator it = m_gifPicMap.begin();
			for ( ; it != m_gifPicMap.end(); ++it )
			{
				if ( it->first->m_gifStatus != ImageEx::STATUS_PLAY )
				{
					continue;
				}
				_GIF_PROPERTY& gifProp = it->second;
				Gdiplus::PropertyItem* propItem = gifProp.m_propertyItem;
				UINT curFrameDelayTime = ((UINT*)propItem->value)[gifProp.m_currentFrameIndex];
				boost::posix_time::time_duration td = curTime -gifProp.m_currentFrameAnimTime;
				UINT nDuraMicroSec = (UINT)td.total_milliseconds();
				if ( curFrameDelayTime < 5 )
				{
					curFrameDelayTime = 10;
				}
				if(  nDuraMicroSec > curFrameDelayTime * 10 )
				{
					gifProp.m_currentFrameIndex++;
					if ( gifProp.m_currentFrameIndex == gifProp.m_nFrameCount )
					{
						gifProp.m_currentFrameIndex = 0;
					}
					gifProp.m_currentFrameAnimTime += td;
					GUID pageGuid = Gdiplus::FrameDimensionTime;
					it->first->SelectActiveFrame( &pageGuid, gifProp.m_currentFrameIndex );
					it->first->AnimUpdatedEvent();
				}
			}
		}
		else
		{
			ASSERT( FALSE );
			break;
		}
	}
}

bool CGifPictureMgr::GetGifProperty( ImageEx* picture, _GIF_PROPERTY& gifProp )
{
	// get get total
	gifProp.m_nFrameCount = GetTotalFrameCount( picture );
	if ( gifProp.m_nFrameCount > 1 )
	{
		int nSize = picture->GetPropertyItemSize(PropertyTagFrameDelay);
		gifProp.m_propertyItem = (Gdiplus::PropertyItem*) malloc( nSize );
		picture->GetPropertyItem(PropertyTagFrameDelay, nSize, gifProp.m_propertyItem );
	}
	return gifProp.m_nFrameCount > 1;
}

void CGifPictureMgr::Play( ImageEx* picture )
{
	if ( picture && picture->m_bIsGif )
	{
		//boost::mutex::scoped_lock lock( m_threadMutex );
		picture->m_gifStatus = ImageEx::STATUS_PLAY;
		std::map<ImageEx*, _GIF_PROPERTY>::iterator itFind = m_gifPicMap.find( picture );
		if ( itFind != m_gifPicMap.end())
		{
			itFind->second.m_currentFrameAnimTime = boost::posix_time::microsec_clock::local_time();
		}
	}
}

void CGifPictureMgr::Stop( ImageEx* picture )
{
	if ( picture && picture->m_bIsGif )
	{
		//boost::mutex::scoped_lock lock( m_threadMutex );
		picture->m_gifStatus = ImageEx::STATUS_STOP;
		GUID pageGuid = Gdiplus::FrameDimensionTime;
		picture->SelectActiveFrame( &pageGuid, 0 );
		picture->AnimUpdatedEvent();
	}
}

void CGifPictureMgr::Pause( ImageEx* picture )
{
	//boost::mutex::scoped_lock lock( m_threadMutex );
	picture->m_gifStatus = ImageEx::STATUS_PAUSE;
}