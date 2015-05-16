#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/detail/bind_handler.hpp>
#include "WndMsgOnly.h"


#define  WM_SIGNAL WM_USER + 1



class CSignalHandler;

namespace boost {
	namespace asio {

template <typename Dispatcher, typename Handler>
class gui_wrapped_handler
{
public:
	typedef void result_type;

	gui_wrapped_handler(Dispatcher dispatcher, Handler handler, CSignalHandler & signalHandler)
		: dispatcher_(dispatcher),
		handler_(handler),
		signalHandler_(signalHandler)
	{
	}

	void operator()()
	{
		dispatcher_.dispatch(handler_);
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	void operator()() const
	{
		dispatcher_.dispatch(handler_);
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	template <typename Arg1>
	void operator()(const Arg1& arg1)
	{
		dispatcher_.dispatch(boost::bind(handler_, arg1));
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	template <typename Arg1>
	void operator()(const Arg1& arg1) const
	{
		dispatcher_.dispatch(detail::bind_handler(handler_, arg1));
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	template <typename Arg1, typename Arg2>
	void operator()(const Arg1& arg1, const Arg2& arg2)
	{
		dispatcher_.dispatch(detail::bind_handler(handler_, arg1, arg2));
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	template <typename Arg1, typename Arg2>
	void operator()(const Arg1& arg1, const Arg2& arg2) const
	{
		dispatcher_.dispatch(detail::bind_handler(handler_, arg1, arg2));
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	template <typename Arg1, typename Arg2, typename Arg3>
	void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3)
	{
		dispatcher_.dispatch(detail::bind_handler(handler_, arg1, arg2, arg3));
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	template <typename Arg1, typename Arg2, typename Arg3>
	void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3) const
	{
		dispatcher_.dispatch(detail::bind_handler(handler_, arg1, arg2, arg3));
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3,
		const Arg4& arg4)
	{
		dispatcher_.dispatch(
			detail::bind_handler(handler_, arg1, arg2, arg3, arg4));
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3,
		const Arg4& arg4) const
	{
		dispatcher_.dispatch(
			detail::bind_handler(handler_, arg1, arg2, arg3, arg4));
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
		typename Arg5>
		void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3,
		const Arg4& arg4, const Arg5& arg5)
	{
		dispatcher_.dispatch(
			detail::bind_handler(handler_, arg1, arg2, arg3, arg4, arg5));
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
		typename Arg5>
		void operator()(const Arg1& arg1, const Arg2& arg2, const Arg3& arg3,
		const Arg4& arg4, const Arg5& arg5) const
	{
		dispatcher_.dispatch(
			detail::bind_handler(handler_, arg1, arg2, arg3, arg4, arg5));
		signalHandler_.PostMessage(WM_SIGNAL);
	}

	//private:
	Dispatcher dispatcher_;
	Handler handler_;
	CSignalHandler & signalHandler_;
};
}
}


class CSignalHandler :
	public CWndMsgOnly
{
	DECLARE_DYNAMIC(CSignalHandler)
public:
	CSignalHandler(void);
	virtual ~CSignalHandler(void);
	boost::shared_ptr<boost::asio::io_service> GetGuiIos()
	{
		return m_gui_ios;
	}
	void Stop();

	template <typename Handler>
	boost::asio::gui_wrapped_handler<boost::asio::io_service&, Handler> GuiWrapper(Handler handler)
	{
		return boost::asio::gui_wrapped_handler<boost::asio::io_service&, Handler> (*(GetGuiIos().get()), handler, *this);
	}

protected:
	DECLARE_MESSAGE_MAP()
	LRESULT OnSignal(WPARAM wParam, LPARAM lParam);

	virtual void Process();



private:
	boost::shared_ptr<boost::asio::io_service> m_gui_ios;
};


CSignalHandler &GetSignalHandler();


template <typename Handler>
boost::asio::gui_wrapped_handler<boost::asio::io_service&, Handler> GuiWrapper(Handler handler)
{
	return boost::asio::gui_wrapped_handler<boost::asio::io_service&, Handler> (*(GetSignalHandler().GetGuiIos().get()), handler, GetSignalHandler());
}