#include "stdafx.h"
#include "session_mem_cac.h"
#include "icommand_executor.h"
#include <boost/algorithm/string.hpp>

SessionMemCac::SessionMemCac(tcp::socket		_socket,
							 u32				_sessionId,
							 const std::string&	_msgSeparateSymbol,
							 iSessionOwner&		_sessionOwner,
							 iCommandExecutor*  _commandExecutor)
: Inherited(std::move(_socket)
			, _sessionId
			, _msgSeparateSymbol
			, _sessionOwner)
, executor(_commandExecutor)
{}

void SessionMemCac::Dispatch(const std::string& _message)
{
	if(isDataBlockExpected)
	{
		tempRecord.data = _message.substr(0, tempLength);
		executor->OnSetCmdReceived(tempKey, tempRecord, tempIsNoReply, this);
		isDataBlockExpected = false;
		return;
	}

	std::vector<std::string> args;
	boost::split(args, _message, [](char c){ return c == ' '; });
	
	const std::string& cmd = args.front();

	if(cmd == CMD_SET)
	{
		DispatchCmdSet(args);
	}
	else if(cmd == CMD_GET)
	{
		DispatchCmdGet(args);
	}
	else if(cmd == CMD_DELETE)
	{
		DispatchCmdDelete(args);
	}
	else
	{
		SendErrorRequestMalformed();
	}
	return;
}

void SessionMemCac::DispatchCmdSet(std::vector<std::string> &args)
{
	if(args.size() > 6 || args.size() < 5)
	{
		SendErrorRequestMalformed();
		return;
	}

	const std::string& key			= args[1];

	u32				   flag;
	u64				   expTime;
	size_t			   length;
	bool			   isNoReply	= false;
	try
	{
		flag	= std::stol(args[2]);
		expTime = std::stoll(args[3]);
		length	= std::stoll(args[4]);

		expTime = expTime	== 0 ? -1 : expTime;
		length	= length	== 0 ? -1 : length;
	}
	catch(const std::invalid_argument&)
	{
		SendErrorRequestMalformed();
		return;
	}

	if (args.size() == 6)
	{
		if(args[5] == "noreply")
		{
			isNoReply = true;
		}
		else
		{
			SendErrorRequestMalformed();
			return;
		}
	}

	tempKey						= key;
	tempRecord.flag				= flag;
	tempRecord.expirationDate	= expTime == -1 ? -1 : expTime + std::time(nullptr);
	tempLength					= length;
	tempIsNoReply				= isNoReply;

	isDataBlockExpected			= true;
}

void SessionMemCac::DispatchCmdGet(std::vector<std::string>& _args)
{
	if(!EraseFirstAndSendErrorIfEmpty(_args))
	{
		return;
	}
	executor->OnGetCmdReceived(_args, this);
}

void SessionMemCac::DispatchCmdDelete(std::vector<std::string>& _args)
{

	if(!EraseFirstAndSendErrorIfEmpty(_args))
	{
		return;
	}

	if(_args.size() > 2)
	{
		SendErrorRequestMalformed();
		return;
	}
	
	bool isNoReply = false;
	if(_args.size() == 2)
	{
		if(FLAG_NO_REPLY == _args[1])
		{
			isNoReply = true;
		}
	}

	executor->OnDeleteCmdReceived(_args[0], isNoReply, this);
}

bool SessionMemCac::EraseFirstAndSendErrorIfEmpty(std::vector<std::string>& _args)
{
	_args.erase(_args.begin());
	if(_args.empty())
	{
		SendErrorRequestMalformed();
		return false;
	}
	return true;
}

void SessionMemCac::SendErrorRequestMalformed()
{
	Send("ERROR request malformed");
}

void SessionMemCac::SendCmdGetResponse(const Pairs& _pairs)
{
	for(const std::pair<std::string, Record>& pair : _pairs)
	{
		Send("VALUE " + pair.first + " " + std::to_string(pair.second.flag) + " " + std::to_string(pair.second.data.size()));
		Send(pair.second.data);
	}
	
}

void SessionMemCac::SendCmdSetReponse()
{
	Send("STORED");
}

void SessionMemCac::SendCmdDeleteResponse()
{
	Send("DELETED");
}
