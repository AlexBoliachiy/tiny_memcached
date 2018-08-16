#include "stdafx.h"
#include "storage.h"
#include <chrono>
#include <thread>

Storage::Storage()
: db("memcache.db")
, future(promise.get_future())
{
	db <<
		"CREATE TABLE IF NOT EXISTS storage("
		"key		TEXT	primary key,"
		"data		TEXT,"
		"flag		INTEGER,"
		"expdate	INTEGER"
		");";

	db <<
		"CREATE INDEX IF NOT EXISTS storage_key_index ON storage(key);";

	LoadFromDbNotTs();
}

Storage::~Storage()
{
	flagCaretakerStop = true;
	future.wait();
	SaveToDb(*this);
}

void Storage::LoadFromDbNotTs()
{
	db	<< 
		"SELECT * FROM storage;"
		>> [this](std::string key, std::string data, u32 flag, size_t expdate)
	{
		insert({ key, Record {std::move(data), flag, expdate, false, false} });
	};
}

void Storage::Start()
{
	caretaker = std::make_shared<std::thread>([this]() 
	{
		while(!flagCaretakerStop)
		{
			std::this_thread::sleep_for(std::chrono::seconds(10));
			Care();
		}
		promise.set_value_at_thread_exit(true);
	});

	caretaker->detach();
}

void Storage::Care()
{
	SessionMemCac::Pairs pairsToDelete;
	SessionMemCac::Pairs pairsToSave;
	mx.lock();

	std::cout << "[CARETAKER]: start to care" << std::endl;

	for(const auto& pair : *this)
	{
		if((pair.second.expirationDate < std::time(nullptr))
			|| pair.second.needDelete)
		{
			pairsToDelete.push_back(pair);
		}
		else if(pair.second.needDump)
		{
			pairsToSave.push_back(pair);
		}
	}
	
	for(const auto& pair : pairsToDelete)
	{
		std::cout << "[CARETAKER]: deleted item" << std::endl;
		erase(pair.first);
	}
	mx.unlock();
	
	SaveToDb(pairsToSave);
	
	for (const auto& pair : pairsToDelete)
	{
		db << "DELETE FROM storage where key = ?" << pair.first;
	}

}

void Storage::OnGetCmdReceived(const std::vector<std::string>& _keys, SessionMemCac* _session)
{
	SessionMemCac::Pairs pairs;

	mx.lock();
	for(const std::string& key : _keys)
	{
		auto it = find(key);
		if(it != end() 
			&& it->second.expirationDate > std::time(nullptr)
			&& !it->second.needDelete)
		{
			pairs.emplace_back(std::move(std::pair<std::string, Record>(key, it->second)));
		}
	}

	mx.unlock();
	_session->SendCmdGetResponse(pairs);
}

void Storage::OnSetCmdReceived(const std::string& _key, Record _record, bool _isNoReply, SessionMemCac* _session)
{
	mx.lock();

	if(find(_key) == end())
	{
		insert({ _key, std::move(_record) });
	}
	else
	{
		at(_key) = std::move(_record);
	}
	mx.unlock();

	if(!_isNoReply)
	{
		_session->SendCmdSetReponse();
	}
}

void Storage::OnDeleteCmdReceived(const std::string& key, bool isNoReply, SessionMemCac* _session)
{
	mx.lock();
	find(key)->second.needDelete = true;
	mx.unlock();

	if(!isNoReply)
	{
		_session->SendCmdDeleteResponse();
	}
}

