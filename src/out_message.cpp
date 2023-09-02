#include "out_message.h"
#include "msg_manager.h"

OutMessage::OutMessage(std::string msg) : OpcodeOut(0)
{
	this->msg = msg;
}

OutMessage::~OutMessage()
{
}

void OutMessage::Send(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteOpcode(GetId());
	auto len = (uint8_t)(msg.length());
	msg_manager->WriteByte(len);
	msg_manager->WriteString(msg);
}
