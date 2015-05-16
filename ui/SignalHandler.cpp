#include "StdAfx.h"

#include <boost/bind.hpp>

#include "SignalHandler.h"
#include "logex.h"


CSignalHandler::CSignalHandler(void)
{
	Create();
	m_gui_ios.reset(new boost::asio::io_service);
}

CSignalHandler::~CSignalHandler(void)
{
	Stop();
}

void CSignalHandler::Stop()
{
	m_gui_ios->stop();
	m_gui_ios.reset();
}

IMPLEMENT_DYNAMIC(CSignalHandler, CWndMsgOnly)


BEGIN_MESSAGE_MAP(CSignalHandler, CWndMsgOnly)
	ON_WM_CREATE()
	ON_MESSAGE(WM_SIGNAL, &CSignalHandler::OnSignal)
END_MESSAGE_MAP()


LRESULT CSignalHandler::OnSignal(WPARAM wParam, LPARAM lParam)
{
	Process();
	return TRUE;
}


void CSignalHandler::Process()
{
	boost::system::error_code ec;
	if (!m_gui_ios)
	{
		LOG_ERROR("m_gui_ios is empty");
		return;
	}
	size_t ret = m_gui_ios->poll(ec);
	if (ret>0 || ec)
	{
		LOG_DEBUG("gui_ios ret " << ret << ", ec: " << ec);
	}

	m_gui_ios->reset();

}

CSignalHandler &GetSignalHandler()
{
	static CSignalHandler sig;
	return sig;

}