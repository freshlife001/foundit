#pragma once
#include <map>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "utils/thread.h"

class ImageEx;
class CGifPictureMgr : public thread
{
public:
	CGifPictureMgr(void);
	~CGifPictureMgr(void);

	void TestForAnimatedGIF( ImageEx* picture );

	void RemoveAnimatedGif( ImageEx* picture );

	void Play( ImageEx* picture );
	void Stop( ImageEx* picture );
	void Pause( ImageEx* picture );

protected:
	virtual void thread_proc();

private:
	bool IsGifPicture( ImageEx* picture );

	int GetTotalFrameCount( ImageEx* picture );

	struct _GIF_PROPERTY 
	{
		int m_nFrameCount;
		int m_currentFrameIndex;
		boost::posix_time::ptime m_currentFrameAnimTime;
		Gdiplus::PropertyItem* m_propertyItem;
		_GIF_PROPERTY()
		{
			m_nFrameCount = 0;
			m_currentFrameIndex = 0;
			m_currentFrameAnimTime = boost::posix_time::microsec_clock::local_time();
			m_propertyItem = NULL;
		}
	};
	bool GetGifProperty( ImageEx* picture, _GIF_PROPERTY& gifProp );

	void GifAnimateThread();

private:
	std::map<ImageEx*, _GIF_PROPERTY> m_gifPicMap;

	HANDLE m_hExitEvent;
	//boost::mutex m_threadMutex;
};
