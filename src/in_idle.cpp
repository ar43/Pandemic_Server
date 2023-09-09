#include "in_idle.h"

InIdle::InIdle() : OpcodeIn(ClientOpcode::IDLE)
{
}

InIdle::~InIdle()
{
}

void InIdle::Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input)
{
}
