#pragma once
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>


//everything api through http
class IEtApi
{
public:
	enum EtApiError
	{
		EAE_SERVICE_NOT_READY = 1000,

		EAE_RPC_FAIL = 10000, // > 10000È«²¿ÎªRPC_FAIL

	};
	class SearchListener
	{
		public:
			//fullPaths : dir ends with /
			virtual void HandleResult(const std::wstring &key, unsigned int total, unsigned int offset, const std::vector<std::wstring>& fullPaths) = 0;

			virtual void HandleError(unsigned int errorCode) = 0;
	};
	typedef boost::shared_ptr<SearchListener> SearchListenerPtr;
	virtual void Search(const std::wstring &key, unsigned int offset, SearchListenerPtr listener) = 0;
	virtual void Init() = 0;

};





