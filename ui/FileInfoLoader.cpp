#include "StdAfx.h"
#include <iostream>
#include <sstream>
#include <time.h>

#include <boost/bind.hpp>
#include <boost/system/error_code.hpp>
#include <boost/foreach.hpp>

#include "utils/file_ex.h"
#include "uilib/common/utils/ImageEx/ImageEx.h"

#include "FileInfoLoader.h"

#include "logex.h"

CFileInfoLoader::CFileInfoLoader(void): m_running(false)
{
}

CFileInfoLoader::~CFileInfoLoader(void)
{
}

void CFileInfoLoader::Init()
{

	LOG_FUNC_SCOPE_C();
	m_running = true;
	m_ios.reset();
	m_work.reset(new boost::asio::io_service::work(m_ios));
	start();


}

void CFileInfoLoader::UnInit()
{
	LOG_FUNC_SCOPE_C();

	m_work.reset();
	m_ios.stop();
	if (is_started())
	{
		stop(1000);
	}
	m_running = false;
	std::map<std::wstring, HICON>::iterator it;
	for (it = m_IconCache.begin(); it != m_IconCache.end(); it ++)
	{
		::DestroyIcon(it->second);
	}
	m_IconCache.clear();
	m_fileInfoCache.clear();

}

CFileInfoLoader & CFileInfoLoader::Instance()
{
	static CFileInfoLoader s_loader;
	return s_loader;
}


bool CFileInfoLoader::LoadFileInfo(LPCTSTR fullPath, unsigned int mask, FileInfo& info)
{
	info.fullpath = fullPath;
	info.filename = file_ex::get_file_name(std::wstring(fullPath)).c_str();

	if (m_running)
	{
		std::wstring path = fullPath;
		m_ios.dispatch(boost::bind(&CFileInfoLoader::load_file_info, this, path, mask));
	}

	

	return false;
}

void CFileInfoLoader::load_file_info(const std::wstring &fullpath, unsigned int mask)
{
	FileInfo & info = m_fileInfoCache[fullpath];
	info.fullpath = fullpath.c_str();
	info.filename = file_ex::get_file_name(fullpath).c_str();

	if (!info.exists)
	{
		//check existance
		if (file_ex::exists_w(fullpath))
		{
			info.exists = true;
		}
		else
		{
			info.exists = false;
			GotFileInfo(info);
			return;
		}
	}
	if ( (mask & ICON) || !(info.validmask & ICON ))
	{
		info.validmask |= ICON;
		std::wstring icon_key;
		if (file_ex::is_directory(fullpath))
		{
			icon_key = _T("");
		}
		else
		{
			icon_key = file_ex::get_file_ext(fullpath);
		}
		if (icon_key == _T("exe"))
		{
			icon_key = fullpath;
		}
		if (m_IconCache.count(icon_key))
		{
			info.imgIcon = m_IconCache[icon_key];
		}
		else
		{
			if (info.imgIcon == NULL)
			{
				HICON hIcon =NULL;
				if (icon_key == L"")
				{
					//dir
					HMODULE hModule = LoadLibrary(_T("SHELL32.dll"));
					if(hModule)
					{
						hIcon = LoadIcon(hModule, MAKEINTRESOURCE(4));
					}
				}
				else
				{
					//file
					//try load icon
					SHFILEINFO shfi;
					SHGetFileInfo(fullpath.c_str(),
						FILE_ATTRIBUTE_NORMAL,
						&shfi, sizeof(SHFILEINFO),
						SHGFI_ICON|SHGFI_USEFILEATTRIBUTES);
					hIcon = shfi.hIcon;
				}

				if (hIcon)
				{
					//draw icon
					info.imgIcon = hIcon;
					m_IconCache[icon_key] = info.imgIcon;
				}

				
			}
		}

	}

	if ( (mask & DATE) || !(info.validmask & DATE ))
	{
		info.validmask |= DATE;
		info.lastmodify = file_ex::get_last_write_time(fullpath);
	}

	if ( (mask & SIZE) || !(info.validmask & SIZE ))
	{
		info.validmask |= ICON;
		if (file_ex::is_directory(fullpath))
		{
			info.size = 0;
		}
		else
		{
			file_ex::get_file_size(fullpath, info.size);
		}
		
	}

	GotFileInfo(info);


}


void CFileInfoLoader::thread_proc()
{

	LOG_INFO_C("start run");


	m_ios.run();
	m_running = false;
	m_ios.stop();

}
