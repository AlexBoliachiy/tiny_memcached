#include "stdafx.h"
#include "tcp_server.h"

TcpServer::TcpServer(boost::asio::io_service & _io_service, u16 _port)
: acceptor(_io_service, tcp::endpoint(tcp::v4(), _port))
, socket(_io_service)
, service(_io_service)
{}

TcpServer::~TcpServer()
{
	std::cout << "server destroyed" << std::endl;
}

void TcpServer::Start()
{
	ListenForConnections();
}

void TcpServer::SessionAdd(std::shared_ptr<Session> _session)
{
	sessions.push_back(_session);
}

void TcpServer::SessionDelete(std::shared_ptr<Session> _session)
{
	auto it = std::find_if(sessions.begin(), sessions.end(), [_session](const std::shared_ptr<Session>& _another) -> bool
	{
		return _another->SessionId() == _session->SessionId();
	});
	sessions.erase(it);
}

u32 TcpServer::GenerateSessionId()
{
	static u32 id = 0;
	return id++;
}

void TcpServer::OnDisconnect(std::shared_ptr<Session> _session)
{
	_session->Close();
	SessionDelete(_session);
}

void TcpServer::OnOverflow(std::shared_ptr<Session> _session)
{
	_session->Close();
	SessionDelete(_session);
}

void TcpServer::ListenForConnections()
{
	acceptor.async_accept(socket,
						   [this](boost::system::error_code ec)
	{
		if (!ec)
		{
			std::shared_ptr<Session> session_t = SessionFactory();
			SessionAdd(session_t);
			session_t->Start();
		}

		ListenForConnections();
	});
}