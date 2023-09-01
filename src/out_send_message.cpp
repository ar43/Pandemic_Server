#include "out_send_message.h"
#include "msg_manager.h"

OutSendMessage::OutSendMessage(std::string msg) : OpcodeOut(0)
{
	this->msg = msg;
}

OutSendMessage::~OutSendMessage()
{
}

void OutSendMessage::Send(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteOpcode(GetId());
	auto len = (uint8_t)(msg.length());
	msg_manager->WriteByte(len);
	msg_manager->WriteString(msg);
}
