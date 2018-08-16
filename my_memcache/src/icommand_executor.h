#pragma once

#include "session_mem_cac.h"
#include "Record.h"

class iCommandExecutor
{
public:
	virtual void OnGetCmdReceived(const std::vector<std::string>& keys, SessionMemCac*)						= 0;
	virtual void OnSetCmdReceived(const std::string& key, Record record, bool isNoReply, SessionMemCac*)	= 0;
	virtual void OnDeleteCmdReceived(const std::string& key, bool isNoReply, SessionMemCac*)				= 0;
};

