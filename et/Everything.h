
#ifndef _EVERYTHING_DLL_
#define _EVERYTHING_DLL_

#ifndef _INC_WINDOWS
#include <windows.h>
#endif

#define EVERYTHING_OK						0
#define EVERYTHING_ERROR_MEMORY				1
#define EVERYTHING_ERROR_IPC				2
#define EVERYTHING_ERROR_REGISTERCLASSEX	3
#define EVERYTHING_ERROR_CREATEWINDOW		4
#define EVERYTHING_ERROR_CREATETHREAD		5
#define EVERYTHING_ERROR_INVALIDINDEX		6
#define EVERYTHING_ERROR_INVALIDCALL		7

#ifndef EVERYTHINGAPI
#define EVERYTHINGAPI __stdcall
#endif

// write search state
 VOID EVERYTHINGAPI Everything_SetSearchW(LPCWSTR lpString);
 VOID EVERYTHINGAPI Everything_SetSearchA(LPCSTR lpString);
 VOID EVERYTHINGAPI Everything_SetMatchPath(BOOL bEnable);
 VOID EVERYTHINGAPI Everything_SetMatchCase(BOOL bEnable);
 VOID EVERYTHINGAPI Everything_SetMatchWholeWord(BOOL bEnable);
 VOID EVERYTHINGAPI Everything_SetRegex(BOOL bEnable);
 VOID EVERYTHINGAPI Everything_SetMax(DWORD dwMax);
 VOID EVERYTHINGAPI Everything_SetOffset(DWORD dwOffset);
 VOID EVERYTHINGAPI Everything_SetReplyWindow(HWND hWnd);
 VOID EVERYTHINGAPI Everything_SetReplyID(DWORD nId);

// read search state
 BOOL EVERYTHINGAPI Everything_GetMatchPath(VOID);
 BOOL EVERYTHINGAPI Everything_GetMatchCase(VOID);
 BOOL EVERYTHINGAPI Everything_GetMatchWholeWord(VOID);
 BOOL EVERYTHINGAPI Everything_GetRegex(VOID);
 DWORD EVERYTHINGAPI Everything_GetMax(VOID);
 DWORD EVERYTHINGAPI Everything_GetOffset(VOID);
 LPCSTR EVERYTHINGAPI Everything_GetSearchA(VOID);
 LPCWSTR EVERYTHINGAPI Everything_GetSearchW(VOID);
 DWORD EVERYTHINGAPI Everything_GetLastError(VOID);
 HWND EVERYTHINGAPI Everything_GetReplyWindow(VOID);
 DWORD EVERYTHINGAPI Everything_GetReplyID(VOID);

// execute query
 BOOL EVERYTHINGAPI Everything_QueryA(BOOL bWait);
 BOOL EVERYTHINGAPI Everything_QueryW(BOOL bWait);

// query reply
BOOL EVERYTHINGAPI Everything_IsQueryReply(UINT message,WPARAM wParam,LPARAM lParam,DWORD nId);

// write result state
 VOID EVERYTHINGAPI Everything_SortResultsByPath(VOID);

// read result state
 int EVERYTHINGAPI Everything_GetNumFileResults(VOID);
 int EVERYTHINGAPI Everything_GetNumFolderResults(VOID);
 int EVERYTHINGAPI Everything_GetNumResults(VOID);
 int EVERYTHINGAPI Everything_GetTotFileResults(VOID);
 int EVERYTHINGAPI Everything_GetTotFolderResults(VOID);
 int EVERYTHINGAPI Everything_GetTotResults(VOID);
 BOOL EVERYTHINGAPI Everything_IsVolumeResult(int nIndex);
 BOOL EVERYTHINGAPI Everything_IsFolderResult(int nIndex);
 BOOL EVERYTHINGAPI Everything_IsFileResult(int nIndex);
 LPCWSTR EVERYTHINGAPI Everything_GetResultFileNameW(int nIndex);
 LPCSTR EVERYTHINGAPI Everything_GetResultFileNameA(int nIndex);
 LPCWSTR EVERYTHINGAPI Everything_GetResultPathW(int nIndex);
 LPCSTR EVERYTHINGAPI Everything_GetResultPathA(int nIndex);

#ifdef UNICODE
#define Everything_SetSearch Everything_SetSearchW
#define Everything_GetSearch Everything_GetSearchW
#define Everything_Query Everything_QueryW
#define Everything_GetResultFileName Everything_GetResultFileNameW
#define Everything_GetResultPath Everything_GetResultPathW
#else
#define Everything_SetSearch Everything_SetSearchA
#define Everything_GetSearch Everything_GetSearchA
#define Everything_Query Everything_QueryA
#define Everything_GetResultFileName Everything_GetResultFileNameA
#define Everything_GetResultPath Everything_GetResultPathA
#endif


#endif

