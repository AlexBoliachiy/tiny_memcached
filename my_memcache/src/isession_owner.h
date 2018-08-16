#pragma once

class Session;

class iSessionOwner
{
public:
	virtual void OnDisconnect(std::shared_ptr<Session>) = 0;
	virtual void OnOverflow(std::shared_ptr<Session>) = 0;
};

