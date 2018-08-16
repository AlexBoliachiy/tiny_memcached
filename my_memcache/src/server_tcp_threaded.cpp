#include "stdafx.h"
#include "server_tcp_threaded.h"

ServerTcpThreaded::ServerTcpThreaded(boost::asio::io_service& _io_service, u16 _port, size_t _threadCount)
	: Inherited(_io_service, _port)
	, threadCount(_threadCount)
{
}

ServerTcpThreaded::~ServerTcpThreaded()
{
	for (auto& thread : threadPool)
	{
		thread.join();
	}
}

void ServerTcpThreaded::Start()
{
	Inherited::Start();

	for (size_t i = 0; i < threadCount; i++)
	{
		threadPool.emplace_back([this](){ service.run(); });
	}
}

void ServerTcpThreaded::SessionAdd(std::shared_ptr<Session> _session)
{
	mutex.lock();
	Inherited::SessionAdd(_session);
	mutex.unlock();
}

void ServerTcpThreaded::SessionDelete(std::shared_ptr<Session> _session)
{
	mutex.lock();
	Inherited::SessionDelete(_session);
	mutex.unlock();
}
