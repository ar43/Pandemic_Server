#include "in_ready.h"
#include "client_input.h"

InReady::InReady() : OpcodeIn(ClientOpcode::READY)
{
}

InReady::~InReady()
{
}

void InReady::Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input)
{
	client_input->ready = true;
}
