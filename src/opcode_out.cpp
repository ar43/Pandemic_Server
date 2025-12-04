#include "opcode_out.h"
#include "msg_manager.h"
OpcodeOut::OpcodeOut(ServerOpcode id)
{
	this->id = id;
}
OpcodeOut::~OpcodeOut() = default;

void OpcodeOut::Send(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->ClearTempOutput();
	WriteBody(msg_manager);
	WriteHeader(msg_manager);
	msg_manager->MergeOutput();
}

uint8_t OpcodeOut::GetId()
{
	return (uint8_t)id;
}

void OpcodeOut::WriteHeader(std::shared_ptr<MsgManager> const& msg_manager)
{
	//reverse order
	msg_manager->WriteSize(OpcodeOut::HEADER_SIZE);
	msg_manager->WriteOpcode((uint8_t)id);
}
