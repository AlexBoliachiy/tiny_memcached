#pragma once

#include "Record.h"
#include "icommand_executor.h"
#include "session_mem_cac.h"
#include <unordered_map>
#include <mutex>
#include <sqlite_modern_cpp.h>

//store items and care them
class Storage : protected std::unordered_map<std::string, Record>
			  , public	  iCommandExecutor
{
public:
	Storage();
	virtual ~Storage();

	virtual void					Start();

	virtual void					LoadFromDbNotTs();

	// save new items and deletes expired
	// supposed to call from detached thread
	virtual void					Care();

	// container must hold std::pair<std::string, Record>
	// ACTUALLY msvc compiler allows me to put forward declaration of this function here and get definition
	// out of class but in this file, but GCC don't. So it's here
	template<class T>
	void SaveToDb(const T& _container)
	{
		for(const auto& pair : _container)
		{
			db <<
				"INSERT OR REPLACE INTO storage(key, data, flag, expdate)"
				"VALUES(?, ?, ?, ?)"
				<< pair.first << pair.second.data << pair.second.flag << pair.second.expirationDate;
		}
	}

private:
	virtual void					OnGetCmdReceived(const std::vector<std::string>& keys, SessionMemCac*);
	virtual void					OnSetCmdReceived(const std::string& key, Record record, bool isNoReply, SessionMemCac*);
	virtual void					OnDeleteCmdReceived(const std::string& key, bool isNoReply, SessionMemCac*);
	
	//actually much nicer would look boost/smart_ptr/detail/spinlock.hpp
	// because it don't makes kernel mode calls
	// but in task details recommended not to use anything that can be substituted from STL
	std::mutex						mx;
	sqlite::database				db;
	std::shared_ptr<std::thread>	caretaker;
	std::atomic<bool>				flagCaretakerStop = {false};
	std::promise<bool>				promise;
	std::future<bool>				future;
};

