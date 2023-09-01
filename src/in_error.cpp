#include "in_error.h"
#include "spdlog/spdlog.h"
#include "client_input.h"

InError::InError() : OpcodeIn(1)
{
}

InError::~InError()
{
}

void InError::Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input)
{
	client_input->invalid_opcode = true;
}
