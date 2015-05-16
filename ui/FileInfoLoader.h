#pragma once

#include <map>
#include <boost/shared_ptr.hpp>
#include "utils/thread.h"

#include "uilib/common/utils/ImageEx/Shared_ImageEx.h"
#include "SignalHandler.h"


typedef struct _FileInfo {
	_FileInfo()
	{
		size = -1;
		exists = false;
		validmask = 0;
		imgIcon = NULL;
	}
	CString fullpath;
	CString filename;
	UINT64 size;
	time_t lastmodify;
	HICON imgIcon;
	unsigned int validmask;
	bool exists;
} FileInfo;


class CFileInfoLoader : public CSignalHandler, public thread
{
public:
	enum FileInfoMask
	{
		ICON = 0x01,
		SIZE = 0x02,
		DATE = 0x04,
	};

	CFileInfoLoader(void);
	virtual ~CFileInfoLoader(void);

	static CFileInfoLoader & Instance();

	void Init();

	void UnInit();

	//return true 表示数据都得到了吗, return false 表示正在获取其他数据，要等GotFileInfo返回
	bool LoadFileInfo(LPCTSTR fullPath, unsigned int mask, FileInfo& info);

	boost::signals2::signal<void ( const FileInfo & )> GotFileInfo;

protected:
	virtual void thread_proc();

private:
	void load_file_info(const std::wstring &fullpath, unsigned int mask);

private:
	boost::asio::io_service m_ios;
	bool m_running;
	boost::shared_ptr<void> m_work;

	std::map<std::wstring , FileInfo > m_fileInfoCache;

	//对于目录 key = "", 对于 exe key = filename(app.exe), 对于其他 key = extension(txt,doc)
	std::map<std::wstring, HICON> m_IconCache;
};
