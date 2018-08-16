#pragma once

#include "server_tcp_threaded.h"
#include "session_mem_cac.h"
#include "storage.h"

class ServerMemCached : public ServerTcpThreaded
{
	using Inherited = ServerTcpThreaded;
public:
	ServerMemCached(boost::asio::io_service& io_service, u16 port, size_t threadCount);
	virtual ~ServerMemCached() = default;

	virtual void						Start()				override;

private:
	virtual std::shared_ptr<Session>	SessionFactory();

	Storage								storage;
};