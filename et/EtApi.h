#pragma once
#include "IEtApi.h"
#include "netlib/URLFetch.h"
#include "utils/tracker.h"


class CEtApi : public  IEtApi
{
public:
	CEtApi(void);
	virtual ~CEtApi(void);

	virtual void Search(const std::wstring &key, unsigned int offset, SearchListenerPtr listener);

	void Init();
private:
	struct Task
	{
		unsigned int _task_id;

		URLFetch _fetcher;
		URLFetch::FetchHandle _handle;

		std::wstring _key;
		unsigned int _total;
		unsigned int _offset;

		std::wstring _req_url;
		SearchListenerPtr _task_handler;

		Task();
		~Task();

		//for statistics
		int _st_error;	//error code.
		time_t _st_start;	//start time.
		time_t _st_stop;		//stop time.
	};

private:
	void search(const std::wstring &key, unsigned int offset, SearchListenerPtr listener);
	void start_task(Task *task);
	void handle_fetch(unsigned task_id, const URLResponse &resp, const boost::system::error_code &err);
	void handle_task_fail(Task *task);
	void delete_task(unsigned task_id);
	bool handle_resp(Task *task, const std::string &resp);
	void update_auth();

	
	typedef std::map<unsigned, Task*> TaskList;
	TaskList m_tasks;

	tracker m_tracker;

	std::string mAuth;
	unsigned int m_port;

};
