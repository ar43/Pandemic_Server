#pragma once

#include "opcode_out.h"
#include <string>

class OutMessage : public OpcodeOut
{
public:
	OutMessage(std::string msg);
	~OutMessage();

	void Send(std::shared_ptr<MsgManager> const& msg_manager);
private:
	std::string msg;
};