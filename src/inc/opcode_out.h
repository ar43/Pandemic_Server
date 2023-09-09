#pragma once
#include <stdint.h>
#include <memory>
#include "server_opcode.h"

class MsgManager;

class OpcodeOut
{
public:
	OpcodeOut(ServerOpcode id);
	~OpcodeOut();

	virtual void Send(std::shared_ptr<MsgManager> const& msg_manager) = 0;
	uint8_t GetId();
private:
	ServerOpcode id;
};