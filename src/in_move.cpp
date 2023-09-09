#include "in_move.h"
#include "client_input.h"
#include "msg_manager.h"

InMove::InMove() : OpcodeIn(ClientOpcode::MOVE)
{
}

InMove::~InMove()
{
}

void InMove::Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input)
{
	client_input->target_city = msg_manager->ReadByte();
	client_input->requested_move = true;
}
