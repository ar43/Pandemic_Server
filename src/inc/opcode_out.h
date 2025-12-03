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

	void Send(std::shared_ptr<MsgManager> const& msg_manager);
	uint8_t GetId();
protected:
	virtual void WriteBody(std::shared_ptr<MsgManager> const& msg_manager) = 0;
private:
	ServerOpcode id;
	void WriteHeader(std::shared_ptr<MsgManager> const& msg_manager);
};