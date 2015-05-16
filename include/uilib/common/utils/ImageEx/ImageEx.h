#pragma once

#include <boost/signals2/signal.hpp>
class CGifPictureMgr;
class ImageEx : public Gdiplus::Image
{
	friend class CGifPictureMgr;
public:
	virtual ~ImageEx(void);

	enum GIF_STATUS
	{
		STATUS_PLAY = 0,
		STATUS_STOP,
		STATUS_PAUSE
	};

	ImageEx(
		IN const WCHAR* filename,
		IN GIF_STATUS initStatus = STATUS_PLAY,
		IN BOOL useEmbeddedColorManagement = FALSE
		);

	ImageEx(
		IN IStream* stream,
		IN GIF_STATUS initStatus = STATUS_PLAY,
		IN BOOL useEmbeddedColorManagement = FALSE
		);

	static ImageEx* FromFile(
		IN const WCHAR* filename,
		IN GIF_STATUS initStatus = STATUS_PLAY,
		IN BOOL useEmbeddedColorManagement = FALSE
		);

	static ImageEx* FromStream(
		IN IStream* stream,
		IN GIF_STATUS initStatus = STATUS_PLAY,
		IN BOOL useEmbeddedColorManagement = FALSE
		);

	static void ExitGifManager();

	static int GetEncoderClsid(const WCHAR* mineFormat, const GUID* guidFormat, CLSID* pClsid);

	boost::signals2::signal<void()> AnimUpdatedEvent;

public:
	bool IsPlaying();
	bool IsGif();
	void Play();
	void Stop();
	void Pause();

private:
	static CGifPictureMgr* m_GifMgr;
	GIF_STATUS m_gifStatus;
	bool m_bIsGif;
};
