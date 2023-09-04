#include "in_client_message.h"
#include "spdlog/spdlog.h"
#include "client_input.h"
#include "msg_manager.h"

InClientMessage::InClientMessage() : OpcodeIn(2)
{
}

InClientMessage::~InClientMessage()
{
}

void InClientMessage::Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input)
{
	auto type = msg_manager->ReadByte();
	auto length = msg_manager->ReadShort();
	auto msg = msg_manager->ReadString(length);

	client_input->client_message = std::make_unique<std::pair<ClientMessageType, std::string>>((ClientMessageType)type, msg);
}