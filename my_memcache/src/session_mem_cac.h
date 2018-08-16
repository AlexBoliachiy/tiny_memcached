#pragma once

#include "session.h"
#include "Record.h"

class iCommandExecutor;

class SessionMemCac : public Session
{
	using Inherited	  = Session;
public:
	using Pairs		  = std::vector<std::pair<std::string, Record>>;

	SessionMemCac(tcp::socket socket, u32 sessionId, const std::string& msgSeparateSymbol, iSessionOwner&, iCommandExecutor*);
	~SessionMemCac() = default;

	void				SendErrorRequestMalformed();
	void				SendCmdGetResponse(const Pairs&);
	void				SendCmdSetReponse();
	void				SendCmdDeleteResponse();

private:
	virtual void		Dispatch(const std::string& message) override;

	void				DispatchCmdSet(std::vector<std::string> &args);
	void				DispatchCmdGet(std::vector<std::string> &args);
	void				DispatchCmdDelete(std::vector<std::string>& args);

	bool				EraseFirstAndSendErrorIfEmpty(std::vector<std::string>& args);

	iCommandExecutor*	executor;
	bool				isDataBlockExpected = false;

	Record				tempRecord;
	std::string			tempKey;
	size_t				tempLength;
	bool				tempIsNoReply;
	// rename
	const std::string	CMD_GET				= "get";
	const std::string	CMD_SET				= "set";
	const std::string	CMD_DELETE			= "delete";
	const std::string	FLAG_NO_REPLY		= "noreply";
};