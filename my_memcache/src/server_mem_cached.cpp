#include "stdafx.h"
#include "server_mem_cached.h"

ServerMemCached::ServerMemCached(boost::asio::io_service& _io_service, u16 _port, size_t _threadCount)
: Inherited(_io_service, _port, _threadCount)
{}

void ServerMemCached::Start()
{
	Inherited::Start();
	storage.Start();
}

std::shared_ptr<Session> ServerMemCached::SessionFactory() 
{
	return std::make_shared<SessionMemCac>(std::move(socket), GenerateSessionId(), SEPARATE_SYMBOL, *this, &this->storage);
}
