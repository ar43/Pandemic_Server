#pragma once
#include <stdint.h>
#include <memory>

class MsgManager;

class OpcodeOut
{
public:
	OpcodeOut(uint8_t id);
	~OpcodeOut();

	virtual void Send(std::shared_ptr<MsgManager> const& msg_manager) = 0;
	uint8_t GetId();
private:
	uint8_t id;
};