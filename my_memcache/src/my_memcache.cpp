#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <utility>
#include "server_mem_cached.h"
#include <boost/algorithm/string.hpp>

int main(int argc, char* argv[])
{
	boost::asio::io_service io_service;

	TcpServer* s = new ServerMemCached(io_service, 2018, 20);
	s->Start();
	io_service.run();
	return 0;
}