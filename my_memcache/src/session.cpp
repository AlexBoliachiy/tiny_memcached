#include "stdafx.h"
#include "session.h"


Session::Session(tcp::socket		_socket,
				 u32				_sessionId,
				 const std::string&	_msgSeparateSymbol,
				 iSessionOwner&		_sessionOwner)
: socket_(std::move(_socket))
, sessionId(_sessionId)
, sessionOwner(_sessionOwner)
, MSG_SEPARATE_SYMBOL(_msgSeparateSymbol)
{
	std::cout << "new session" << std::endl;
}

Session::~Session()
{
	std::cout << "session deleted" << std::endl;
}

void Session::Start()
{
	Read();
}

void Session::Close()
{
	isClosing = true;
	socket_.shutdown(tcp::socket::shutdown_both);
	socket_.close();
}

void Session::Read()
{
	socket_.async_read_some(boost::asio::buffer(incomingBuffer, 1024),
							[this](boost::system::error_code ec, std::size_t length)
	{
		// Capture self for furthers safe execute; 
		// Guarantees that object will be destroyed not earlier than method exits
		auto self = shared_from_this();
		if(!ec)
		{
			msgBuffer.append(incomingBuffer.begin(), incomingBuffer.begin() + length);

			if(msgBuffer.size() > MAX_MSG_BUFFER_SIZE)
			{
				sessionOwner.OnOverflow(std::move(shared_from_this()));
			}

			size_t lastMsgSeparator = msgBuffer.rfind(MSG_SEPARATE_SYMBOL);

			if(std::string::npos != lastMsgSeparator)
			{
				std::string NconsistentMsgs = msgBuffer.substr(0, lastMsgSeparator);
				msgBuffer = msgBuffer.substr(lastMsgSeparator + MSG_SEPARATE_SYMBOL.size());
				TryDispatchMessages(std::move(NconsistentMsgs));
			}

			if(!isClosing.load())
			{
				Read();
			}
		}
		else if(boost::asio::error::eof == ec
			   || boost::asio::error::connection_reset == ec
			   )
		{
			std::cout << "disconnect" << std::endl;
			sessionOwner.OnDisconnect(std::move(shared_from_this()));
		}
	});
}

void Session::Send(const std::string& _msg)
{
	outcomingBuffer = _msg;
	outcomingBuffer += "\r\n";
	boost::asio::write(socket_, boost::asio::buffer(outcomingBuffer));
	outcomingBuffer.clear();
}

void Session::TryDispatchMessages(const std::string& _rawMessages)
{
	boost::split_regex(messages, _rawMessages, boost::regex(MSG_SEPARATE_SYMBOL));

	for(const std::string& msg : messages)
	{
		Dispatch(msg);
	}
	
	messages.clear();
}