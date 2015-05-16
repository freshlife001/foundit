#include "StdAfx.h"
#include <sstream>
#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "boost/filesystem/operations.hpp"
#include "html/HTMLParser.h"

#include "netlib/IInvoke.h"
#include "netlib/NetEngine.h"
#include "netlib/HttpHeader.h"
#include "utils/strutils.h"
#include "utils/IPropertyTree.h"
#include "core.h"
#include "EtApi.h"

#include "logex.h"

namespace fs = boost::filesystem;

#define HTTP_AUTH_MECHANISM_BASIC "Basic"
#define HTTP_WEB_AUTH_REQUEST_HEADER_NAME "Authorization"
#define HTTP_WEB_AUTH_RESPONSE_HEADER_NAME "WWW-Authenticate"
#define EVERY_THING_INI_NAME L"Everything.ini"

CEtApi::Task::Task()
: _fetcher(*GetCoreNetEngine()), _st_error(0), _st_start(0), _st_stop(0),_offset(0),_total(0)
{
}

CEtApi::Task::~Task()
{
	if(_handle)
	{
		_fetcher.cancel(_handle);
	}
}


CEtApi::CEtApi(void) :m_port(80)
{
	update_auth();
}

CEtApi::~CEtApi(void)
{
}



void CEtApi::Search(const std::wstring &key, unsigned int offset, SearchListenerPtr listener)
{

	//call from gui thread
	GetCoreInvoker()->post(boost::bind(&CEtApi::search, this, key, offset, listener), m_tracker.get());
	
}

void CEtApi::search(const std::wstring &key, unsigned int offset, SearchListenerPtr listener)
{
	Task *task = new Task();
	task->_key = key;
	task->_offset = offset;


	std::string fixed_key = WTOU8(key);
	fixed_key = urlEncode(fixed_key, "@*._-");

	std::wstringstream ss;
	ss << L"http://127.0.0.1:" << m_port << L"/?s=" << U8TOW(fixed_key);
	
	if (offset > 0) 
	{
		ss << L"&o=" << offset;
	}
	task->_req_url = ss.str();


	static unsigned int id = 0;
	task->_task_id = id++;
	task->_task_handler = listener;
	m_tasks[task->_task_id] = task;

	start_task(task);
}


void CEtApi::start_task(Task *task)
{
	LOG_INFO_C("task_id: " << task->_task_id);

	task->_st_start = time(NULL);

	std::string encoded_url = WTOU8(task->_req_url);
	URLRequest req(encoded_url);
	req.headers().add(HTTP_WEB_AUTH_REQUEST_HEADER_NAME, mAuth);
	task->_handle = task->_fetcher.fetch(req, boost::bind(&CEtApi::handle_fetch, this, task->_task_id, _3, _4), m_tracker.get());
}



void CEtApi::handle_fetch(unsigned task_id, const URLResponse &resp, const boost::system::error_code &err)
{
	LOG_INFO_C("task_id: " << task_id << ", resp.status: " << resp.status() << ", resp.body: " << resp.body() << ", err: ");

	if (!m_tasks.count(task_id))
	{
		return;
	}
	Task *task = m_tasks[task_id];

	if(401 == resp.status())
	{
		LOG_ERROR_C("auth error!");
		update_auth();
		return;
	}

	if(err || (200 != resp.status()))
	{
		//task fail
		LOG_ERROR_C("task error! " << task->_req_url);
		if (err)
		{
			task->_st_error = err.value();
		}
		else
		{
			task->_st_error = resp.status();

		}
		handle_task_fail(task);
		return;
	}


	bool ok = false;

	const std::string &body = resp.body();
	if(!body.empty())
	{
		ok = handle_resp(task, body);
			
	}

	if (!ok)
	{
		LOG_ERROR_C("resp error : " << body);
		handle_task_fail(task);
	}

	delete_task(task_id);

}

void CEtApi::update_auth()
{
	std::wstring path = EVERY_THING_INI_NAME;
	try
	{
		path = fs::current_path().wstring() + L"\\dep\\" + path;
	}
	catch(boost::system::system_error &e)
	{
		(e);
	}
	wchar_t tcDescription[MAX_PATH] = { 0 }, tcTemp[MAX_PATH] = { 0 };
	std::string outputToken;
	if ( GetPrivateProfileStringW(L"Everything", L"http_server_username", NULL, tcTemp, MAX_PATH, path.c_str()) && tcTemp[0] != 0 )
	{
		std::wstring temp = tcTemp;
		outputToken += WTOU8(temp);
	}
	outputToken += ":";
	if ( GetPrivateProfileStringW(L"Everything", L"http_server_password", NULL, tcTemp, MAX_PATH, path.c_str()) && tcTemp[0] != 0 )
	{
		std::wstring temp = tcTemp;
		outputToken += WTOU8(temp);
	}

	mAuth = HTTP_AUTH_MECHANISM_BASIC " " + base64Encode(outputToken);


	if ( GetPrivateProfileStringW(L"Everything", L"http_server_port", NULL, tcTemp, MAX_PATH, path.c_str()) && tcTemp[0] != 0 )
	{
		std::wstring temp = tcTemp;
		m_port = decimalFromString(WTOU8(temp));
	}
	else
	{
		m_port = 80;
	}
	

	
}
bool CEtApi::handle_resp(Task *task, const std::string &resp)
{
	std::vector<std::wstring> result;

	using namespace htmlcxx::HTML;

	//parse result
	{
		HTMLParser::pair_citer_t its = HTMLParser::findTag(HTMLParser::pair_citer_t(resp.begin(),resp.end()),"table","<table class=\"maintable\" width=\"100%\" cellspacing=\"0\">");	//narrowing the tree built range, saving time and memory
		if (its.first==resp.end())
		{
			return false;
		}

		HTMLParser parser(its.first,its.second);
		HTMLParser::sibling_iterator it_table = parser.child(parser.begin(), 0);
		HTMLParser::sibling_iterator it_tr = parser.begin(it_table);
		HTMLParser::sibling_iterator it_tr_end = parser.end(it_table);
		for (;it_tr != it_tr_end ; ++it_tr)
		{
			bool is_file = false;
			HTMLParser::sibling_iterator it_td_name = parser.child(it_tr,0);
			HTMLParser::sibling_iterator it_td_path = parser.child(it_tr,1);
			if ( boost::starts_with(it_td_name->text(), "<td") && boost::starts_with(it_td_path->text(), "<td") )
			{
				HTMLParser::sibling_iterator it_img = parser.child(parser.child(it_td_name,0),0);
				std::string path_leaf;
				if (it_img->text() == "<img src=\"/file.png\" />")
				{
					is_file = true;
					path_leaf = parser.child(it_td_name,1)->text();
				}
				else if (it_img->text() == "<img src=\"/folder.png\" />")
				{
					path_leaf = parser.child(parser.child(it_td_name,1), 0)->text();
				}

				HTMLParser::sibling_iterator it_path = parser.child(it_td_path,0);
				std::string full_path;
				for(;it_path != NULL; ++it_path)
				{
					HTMLParser::sibling_iterator it_text = parser.child(it_path,0);
					if (it_text != NULL)
					{
						full_path += it_text->text();
						full_path += "\\";
					}

				}

				full_path += path_leaf;
				if (!is_file)
				{
					full_path += "\\";
				}
				result.push_back(U8TOW(htmlDecode(full_path)));
			};
		}
	}
	

	//parse count
	if (result.size() > 0)
	{
		HTMLParser::pair_citer_t its = HTMLParser::findTag(HTMLParser::pair_citer_t(resp.begin(),resp.end()),"table","<table class=\"cwdtable\" width=\"100%\" cellspacing=\"0\">");
		if (its.first!=resp.end())
		{
			static boost::regex RegexResultCount("of\\s*<b>(?<Count>.+)</b>\\s*for");
			boost::smatch what;
			if (regex_search(its.first, its.second, what, RegexResultCount, boost::match_default)) 
			{
				std::string count = what["Count"];
				boost::replace_all(count, ",", "");
				task->_total = decimalFromString(count);
			}
			else
			{
				task->_total = result.size();
			}

		}


	}
	task->_task_handler->HandleResult(task->_key, task->_total, task->_offset, result);

	return true;

}

void CEtApi::delete_task(unsigned task_id)
{
	LOG_INFO_C("task_id: " << task_id);

	TaskList::iterator it = m_tasks.find(task_id);
	if(m_tasks.end() == it)
	{
		LOG_INFO_C("can't find task: " << task_id);
		return;
	}
	delete it->second;
	m_tasks.erase(it);

}

void CEtApi::handle_task_fail(Task *task)
{
	LOG_WARN_C("task_id: " << task->_task_id << ", code :" << task->_st_error);
	task->_task_handler->HandleError(task->_st_error);
}



void CEtApi::Init()
{

}


IEtApi * g_core_api = NULL;

IEtApi* GetCoreEtApi()
{
	if (g_core_api == NULL)
	{
		g_core_api = new CEtApi();
	}

	return g_core_api;
}
