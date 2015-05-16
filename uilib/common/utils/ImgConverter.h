#pragma once
#include <boost/signals2/signal.hpp>
#include <boost/shared_ptr.hpp>
#include "uilib_exports.h"

namespace boost
{
	class thread;
}

typedef boost::shared_ptr<class IImgConverter> ImgConverterPtr;
class IImgConverter
{
public:
	virtual ~IImgConverter() { }

	UILIB_EXPORT static ImgConverterPtr new_instance();
	UILIB_EXPORT static bool IsFileSupported(const std::wstring& fromPath);
	UILIB_EXPORT static void GenPreviewImg(std::string& preview, const std::wstring& fromPath);
	UILIB_EXPORT static bool IsFileGif(const std::wstring& fromPath);

	enum ImgType
	{
		IT_PNG,
		IT_JPG,
		IT_BMP,
		IT_GIF,
	};
	virtual void AsynConvert(const std::wstring& fromPath,const std::wstring& toPath, ImgType toImgType, int toImgWidth, int toImgHight) = 0;
	virtual bool Convert(const std::wstring& fromPath,const std::wstring& toPath, ImgType toImgType, int toImgWidth, int toImgHight) = 0;

	typedef boost::signals2::signal<void (const std::wstring &, bool)>  event_cref_wstr_bool_t;
	event_cref_wstr_bool_t ConvertFinish;
};

class UILIB_EXPORT GDIPlusStartupShutdown
{
public:
	GDIPlusStartupShutdown();
	~GDIPlusStartupShutdown();
protected:
	unsigned long gdiplusToken;
};

struct _GUID;
typedef _GUID CLSID;
namespace Gdiplus
{
	class Image;
}

UILIB_EXPORT int GetEncoderClsid(const wchar_t* mineFormat, const _GUID* guidFormat, CLSID* pClsid);

UILIB_EXPORT bool CopyImagePropertyItems(Gdiplus::Image* src, Gdiplus::Image* dest);

UILIB_EXPORT bool SaveThumbnailImage(Gdiplus::Image* img, unsigned int thumbWidth, unsigned int thumbHeight, const wchar_t* saveFileName);
