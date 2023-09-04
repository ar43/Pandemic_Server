#include "in_client_message.h"
#include "spdlog/spdlog.h"
#include "client_input.h"

InClientMessage::InClientMessage() : OpcodeIn(2)
{
}

InClientMessage::~InClientMessage()
{
}

void InClientMessage::Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input)
{
	
}