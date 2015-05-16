#pragma once

#include "uilib_exports.h"
#include <string>
#include <map>

class UILIB_EXPORT Ch2PyConvertor
{
public:
	Ch2PyConvertor(void);
	~Ch2PyConvertor(void);

	static Ch2PyConvertor* GetInstance();
	static void DestroyInstance();
	void Load();
	void Convert( const std::wstring& ch, std::wstring& converResult );
	void Convert( const std::wstring& ch, std::wstring& convertResult, std::wstring& abbreviation );
	void Convert(TCHAR ch,std::wstring& convertResult);
	void ThreadLoad();
	static bool IsChChacracter( TCHAR ch );

	HANDLE m_hLoadThread;

	bool m_bLoadFinished;
private:

	static UINT WINAPI _ThreadLoad(LPVOID pParam);
private:
	std::map<TCHAR, std::wstring> m_pyTable;
	static Ch2PyConvertor* m_instance;


};
