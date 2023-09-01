#pragma once

#include "opcode_out.h"
#include <string>

class OutSendMessage : public OpcodeOut
{
public:
	OutSendMessage(std::string msg);
	~OutSendMessage();

	void Send(std::shared_ptr<MsgManager> const& msg_manager);
private:
	std::string msg;
};