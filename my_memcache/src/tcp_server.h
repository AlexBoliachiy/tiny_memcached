#pragma once

#include "session.h"
#include "isession_owner.h"

// basic abstract class
// handles sessions

class TcpServer : public iSessionOwner
{
public:
	TcpServer(boost::asio::io_service& _io_service, u16 _port);
	virtual ~TcpServer();

	virtual void						Start();

protected:
	virtual void						SessionAdd(std::shared_ptr<Session>);
	virtual void						SessionDelete(std::shared_ptr<Session>);

	u32									GenerateSessionId();

	boost::asio::io_service&			service;
	tcp::socket 						socket;
	const std::string					SEPARATE_SYMBOL = "\r\n";

private:
	virtual void						OnDisconnect(std::shared_ptr<Session>);
	virtual void						OnOverflow(std::shared_ptr<Session>);

	void								ListenForConnections();

	virtual std::shared_ptr<Session>	SessionFactory() = 0;

	list_ptr<Session>					sessions;
	tcp::acceptor						acceptor;
};