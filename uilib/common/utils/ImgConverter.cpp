#include "StdAfx.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/scoped_ptr.hpp>
//#include <boost/thread.hpp>


#include <sstream>

#include "ImgConverter.h"
#include "gdiplusnew.h"
#include "logex.h"

namespace fs = boost::filesystem;


using namespace Gdiplus;

class CImgConverter : public IImgConverter
{
public:
	CImgConverter();
	~CImgConverter();

	virtual void AsynConvert(const std::wstring& fromPath,const std::wstring& toPath, ImgType toImgType, int toImgWidth, int toImgHight);
	virtual bool Convert(const std::wstring& fromPath,const std::wstring& toPath, ImgType toImgType, int toImgWidth, int toImgHight);

private:
	bool Run();

private:
	//boost::shared_ptr<boost::thread> m_thread;
	bool m_running;
	std::wstring m_fromPath;
	std::wstring m_toPath;
	ImgType m_toImgType;
	int m_toImgWidth;
	int m_toImgHight;
};

ImgConverterPtr IImgConverter::new_instance()
{
	return ImgConverterPtr(new CImgConverter());
}

bool IImgConverter::IsFileSupported(const std::wstring& fromPath)
{
	try
	{
		boost::scoped_ptr<Image> image(new Image(fromPath.c_str()));
		return image && image->GetLastStatus() == Ok;
	}
	catch(std::exception &e)
	{
		LOG_WARN("exception: " << e.what());
	}

	return false;
}

bool IImgConverter::IsFileGif(const std::wstring& fromPath)
{
	try
	{
		fs::ifstream ifs(fromPath, std::ios::in|std::ios::binary);
		if (ifs)
		{
			std::string read_buf;
			read_buf.resize(3);
			ifs.read(&read_buf[0], 3);
			if ("GIF" == read_buf)
			{
				return true;
			}
		}

	}
	catch(boost::system::system_error &e)
	{
		LOG_ERROR(e.code() << e.what());
	}


	return false;
};

void IImgConverter::GenPreviewImg(std::string &preview, const std::wstring& fromPath)
{
	LOG_FUNC_SCOPE();
	boost::scoped_ptr<Image> image(new Image(fromPath.c_str()));
	if (!image || image->GetLastStatus() != Ok)
	{
		return;
	}
	float scale_w = (float)96/image->GetWidth();
	float scale_h = (float)96/image->GetHeight();
	float scale = scale_w;
	if (scale > scale_h) scale = scale_h;
	if (scale > 1) scale = 1;

	int width = int((float)image->GetWidth() * scale);
	int height = int((float)image->GetHeight() * scale);
	width = width > 0 ? width : 1;
	height = height > 0 ? height : 1;
	boost::scoped_ptr<Image> previewImage( image->GetThumbnailImage(width, height) );
	if (previewImage)
	{

		CLSID pngClsid;
		GetEncoderClsid(L"image/png", NULL, &pngClsid);

		std::wstring previewImagePath = fromPath+L".preview";

		// Save image1 as a stream in the compound file.
		int stat = previewImage->Save(previewImagePath.c_str(), &pngClsid, NULL);
		if(stat == Ok)
		{
			try
			{
				{
					fs::ifstream ifs;
					ifs.open(previewImagePath, std::ios_base::in|std::ios_base::binary);
					if (ifs)
					{
						std::ostringstream ostr;
						ostr << ifs.rdbuf();
						preview = ostr.str();
					}
				}

				fs::remove(previewImagePath);

			}
			catch(boost::system::system_error &e)
			{
				LOG_ERROR(e.code() << e.what());
			}
		}
	}
};


CImgConverter::CImgConverter(void):m_running(false)
{
	LOG_FUNC_SCOPE_C();
}

CImgConverter::~CImgConverter(void)
{
	LOG_FUNC_SCOPE_C();
// 	if (m_thread)
// 	{
// 		m_thread->join();
// 	}
}

int GetEncoderClsid(const wchar_t* mineFormat, const _GUID* guidFormat, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if(	mineFormat != NULL && wcscmp(pImageCodecInfo[j].MimeType, mineFormat) == 0
			|| guidFormat != NULL && *guidFormat == pImageCodecInfo[j].FormatID )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

void CImgConverter::AsynConvert(const std::wstring& fromPath,const std::wstring& toPath, ImgType toImgType, int toImgWidth, int toImgHight)
{
	//if (!m_thread)
	{
		m_running = true;
		m_fromPath = fromPath;
		m_toPath = toPath;
		m_toImgType = toImgType;
		m_toImgWidth = toImgWidth;
		m_toImgHight = toImgHight;

		//m_thread.reset(new boost::thread(boost::bind(&CImgConverter::Run, this)));
	}
};
bool CImgConverter::Convert(const std::wstring& fromPath,const std::wstring& toPath, ImgType toImgType, int toImgWidth, int toImgHight)
{
	m_running = true;
	m_fromPath = fromPath;
	m_toPath = toPath;
	m_toImgType = toImgType;
	m_toImgWidth = toImgWidth;
	m_toImgHight = toImgHight;
	return Run();
};

bool CImgConverter::Run()
{
	LOG_FUNC_SCOPE_C();
	Image img(m_fromPath.c_str(), TRUE);
	bool ret = SaveThumbnailImage(&img, m_toImgWidth, m_toImgHight, m_toPath.c_str());
	ConvertFinish(m_toPath, ret);
	return ret;
};

GDIPlusStartupShutdown::GDIPlusStartupShutdown()
{
	LOG_FUNC_SCOPE_C();
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

GDIPlusStartupShutdown::~GDIPlusStartupShutdown()
{
	LOG_FUNC_SCOPE_C();
	GdiplusShutdown(gdiplusToken);
}


bool CopyImagePropertyItems(Image* src, Image* dest)
{
	UINT  size = 0;
	UINT  count = 0;
	if(src->GetPropertySize(&size, &count) != S_OK)
		return false;
	PropertyItem* pPropBuffer =(PropertyItem*)malloc(size);

	if(src->GetAllPropertyItems(size, count, pPropBuffer) != S_OK)
	{
		free(pPropBuffer);
		return false;
	}

	for(UINT j = 0; j < count; ++j)
	{
		if(dest->SetPropertyItem(pPropBuffer + j) != S_OK)
		{
			free(pPropBuffer);
			return false;
		}
	}
	free(pPropBuffer);
	return true;
}

bool SaveThumbnailImage(Image* img, 
						unsigned int thumbWidth,
						unsigned int thumbHeight,
						const wchar_t* saveFileName)
{
	if (img == NULL
		||thumbWidth == 0
		||thumbHeight == 0
		||saveFileName == NULL)
	{
		return false;
	}
	GUID guidFormat;
	Status stat;
	CLSID EncoderClsid;
	if(img->GetRawFormat(&guidFormat) != S_OK 
		|| GetEncoderClsid(NULL, &guidFormat, &EncoderClsid) == -1)
	{
		return false;
	}

	Bitmap thumbBmp(thumbWidth, thumbHeight);
	Graphics* g = Graphics::FromImage(&thumbBmp);
	if (guidFormat != ImageFormatGIF &&
		guidFormat != ImageFormatTIFF)
	{
		g->DrawImage(img, 0, 0, thumbWidth, thumbHeight);
		CopyImagePropertyItems(img, &thumbBmp);
		stat = thumbBmp.Save(saveFileName, &EncoderClsid);
		delete g;
		return stat == S_OK;
	}
	else
	{
		/*
			Among all the image formats currently supported by GDI+,
			the only formats that support multiple-frame images are 
			GIF and TIFF. When you call the Image::SelectActiveFrame 
			method on a GIF image, you should use FrameDimensionTime. 
			When you call the Image::SelectActiveFrame method on a TIFF
			image, you should use FrameDimensionPage.
		*/
		GUID	dimensionID;
		if (guidFormat == ImageFormatGIF)
		{
			dimensionID = FrameDimensionTime;
		}
		else	//ImageFormatTIFF
		{
			dimensionID = FrameDimensionPage;
		}
		
		UINT	fCount = img->GetFrameCount(&dimensionID);

		EncoderParameters encoderParameters;
		ULONG             parameterValue;

		encoderParameters.Count = 1;
		encoderParameters.Parameter[0].Guid = EncoderSaveFlag;
		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParameters.Parameter[0].NumberOfValues = 1;
		encoderParameters.Parameter[0].Value = &parameterValue;
		
		SolidBrush brush(Color(255, 255, 255, 255));
		g->FillRectangle(&brush, 0, 0,thumbWidth, thumbHeight);
		g->DrawImage(img, 0, 0, thumbWidth, thumbHeight);
		delete g;

		// Save the first page (frame).
		CopyImagePropertyItems(img, &thumbBmp);
		parameterValue = EncoderValueMultiFrame;
		if(thumbBmp.Save(saveFileName, &EncoderClsid, &encoderParameters)!= S_OK)
		{
			return false;
		}
		if (fCount <= 1)
		{
			return true;
		}

		parameterValue = EncoderValueFrameDimensionTime;
		Bitmap tmpBmp(thumbWidth, thumbHeight);
		Graphics* tmpg = Graphics::FromImage(&tmpBmp);

		for (UINT i = 1; i < fCount; i++)	//Save remaining pages
		{
			img->SelectActiveFrame(&dimensionID, i);
			tmpg->FillRectangle(&brush, 0, 0,thumbWidth, thumbHeight);
			tmpg->DrawImage(img, 0, 0, thumbWidth, thumbHeight);
			thumbBmp.SaveAdd(&tmpBmp, &encoderParameters);
		}
		delete tmpg;
		return true;
	}
}
