#pragma once

#include "tcp_server.h"

class ServerTcpThreaded : public TcpServer
{
	using Inherited = TcpServer;
public:
	ServerTcpThreaded(boost::asio::io_service& io_service, u16 port, size_t threadCount);
	~ServerTcpThreaded();

	virtual void				Start()									override;

protected:
	virtual void				SessionAdd(std::shared_ptr<Session>)	override;
	virtual void				SessionDelete(std::shared_ptr<Session>) override;

private:
	std::vector<std::thread> 	threadPool;
	const size_t				threadCount;
	std::mutex					mutex;
};

