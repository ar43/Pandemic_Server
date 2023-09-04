#pragma once

#include "opcode_out.h"
#include <string>
#include "server_message_type.h"

class OutServerMessage : public OpcodeOut
{
public:
	OutServerMessage(ServerMessageType server_message_type, std::string msg);
	~OutServerMessage();

	void Send(std::shared_ptr<MsgManager> const& msg_manager);
private:
	std::string msg;
	ServerMessageType server_message_type;
};