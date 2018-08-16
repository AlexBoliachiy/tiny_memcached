#pragma once

class Record
{
public:
	Record()	= default;
	~Record()	= default;

	std::string data;
	u32			flag		   = 0;
	u64			expirationDate = -1;
	bool		needDump	   = true;
	bool		needDelete	   = false;
};