#pragma once

#include "isession_owner.h"
#include <memory>
#include <boost/algorithm/string/regex.hpp>

// basic abstract class
// handles incoming messages and dispatches them

class Session : public std::enable_shared_from_this<Session>
{
	using buffer = std::array<char, 1024>;
public:
	Session(tcp::socket socket, u32 sessionId, const std::string& msgSeparateSymbol, iSessionOwner&);
	Session(Session&&)					= default;
	Session& operator=(Session&&)		= default;
	virtual ~Session();

	Session(const Session&)				= delete;
	Session& operator=(const Session&)	= delete;

	u32							SessionId()			const { return sessionId; }

	void						Start();
	void						Close();

	void						Send(const std::string& msg);
private:
	void						Read();

	void						TryDispatchMessages(const std::string& _rawMessages);
	virtual void				Dispatch(const std::string& message) = 0;

	tcp::socket					socket_;
	std::string					msgBuffer;
	std::string					outcomingBuffer;
	buffer						incomingBuffer;
	u32							sessionId;
	iSessionOwner&				sessionOwner;

	const std::string&			MSG_SEPARATE_SYMBOL;
	const size_t				MAX_MSG_BUFFER_SIZE = 10000;
	std::atomic<bool>			isClosing			= {false};

	std::vector<std::string>	messages;
};