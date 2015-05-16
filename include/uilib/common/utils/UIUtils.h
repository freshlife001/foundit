#pragma once
#include <string>
#include <map>
#include "uilib\uilib_exports.h"

namespace hdutils
{

//helper function

// 在新开的默认浏览器中打开某个网址
UILIB_EXPORT void OpenURL(const std::wstring & url);

//在explorer中打开一个文件夹
UILIB_EXPORT void OpenFolder(const std::wstring &path);

// 检查一个用户ID是否合法, 并把ID正规化(trim并且to_lower)
UILIB_EXPORT bool CheckUserID(std::wstring & wstrID);

// 检查一个用户密码是否合法
UILIB_EXPORT bool CheckPwd(const std::wstring & wstrPwd);

// 检查一个字符是否可在编辑框中被显示
UILIB_EXPORT bool IsEditPrint(TCHAR c) ;

// 获取当前程序路径
UILIB_EXPORT bool GetModuleFileName( std::wstring& fileName );

// 获取当前程序所在目录
UILIB_EXPORT bool GetAppDirectory( std::wstring& moudleDir );


// 用于字典序的排序算子
class UILIB_EXPORT DictionarySorter
{
public:
	bool operator () ( const std::wstring& lc, const std::wstring& rc);

} ;

// 判断一个中文字符串str是否匹配了某个输入字符串match，汉字支持匹配首字母
// 例如 IsStringMarch(L"cs", L"测试") = true
UILIB_EXPORT bool IsStringMarch( const std::wstring& match, const std::wstring& originString );

UILIB_EXPORT void SearchTarget(LPCWSTR lpszFilePath);
UILIB_EXPORT void SearchTarget2(LPCWSTR lpszFilePath);
UILIB_EXPORT void CopyToClipBoard(const std::wstring &wstrID); //copy到粘贴板



//获得某个系统目录地址，例如:
//GetWindowsDir(CSIDL_LOCAL_APPDATA, dir);// dir = &appdata&
UILIB_EXPORT bool GetWindowsDir(int dirid,std::wstring &wstrPath);


// std::wstring GetStatusStr(LibIM::ContactStatus status);

UILIB_EXPORT std::wstring GetTimeString(time_t t);


class UILIB_EXPORT CPUPerformance
{
public:
	CPUPerformance(LPCTSTR lpszTag) ;
	~CPUPerformance() ;

	BOOL Start() ;
	BOOL Tick() ;

protected:
	LPCTSTR m_sTag ;

	LARGE_INTEGER m_liFrequency ;

	LARGE_INTEGER m_liStart ;

private:
};


//用户行为统计接口
void UILIB_EXPORT TrackUIOperation(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam);
void UILIB_EXPORT GetUIOperationCounter( std::map<std::string , unsigned int> &counts);

} ;
