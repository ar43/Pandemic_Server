#include "out_server_message.h"
#include "msg_manager.h"

OutServerMessage::OutServerMessage(ServerMessageType server_message_type, std::string msg) : OpcodeOut(0)
{
	this->msg = msg;
	this->server_message_type = server_message_type;
}

OutServerMessage::~OutServerMessage()
{
}

void OutServerMessage::Send(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteOpcode(GetId());
	auto len = (uint16_t)(msg.length());
	msg_manager->WriteByte((uint8_t)server_message_type);
	msg_manager->WriteShort(len);
	msg_manager->WriteString(msg);
}
