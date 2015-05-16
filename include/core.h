#pragma once
#include "core_exports.h"

class NetEngine;
class IInvoke;
class IEtApi;

LIBCORE_EXPORT NetEngine *GetCoreNetEngine();
LIBCORE_EXPORT IInvoke *GetCoreInvoker();
LIBCORE_EXPORT IEtApi* GetCoreEtApi();
LIBCORE_EXPORT void ShutDown();


enum GlobalStringType
{
	DUMP_ID,
	CORE_VER,
	MAX_STRING_SIZE,
};
LIBCORE_EXPORT std::wstring & GetGlobalString(GlobalStringType type);


