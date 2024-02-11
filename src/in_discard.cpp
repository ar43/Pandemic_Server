#include "in_discard.h"
#include "client_input.h"
#include "msg_manager.h"

InDiscard::InDiscard() : OpcodeIn(ClientOpcode::DISCARD)
{
}

InDiscard::~InDiscard()
{
}

void InDiscard::Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input)
{
	client_input->discard_card_id = (int)msg_manager->ReadByte();
}
