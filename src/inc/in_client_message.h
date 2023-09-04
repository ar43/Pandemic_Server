#pragma once
#include "opcode_in.h"

class InClientMessage : public OpcodeIn
{
public:
	InClientMessage();
	~InClientMessage();

	void Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input);
private:
};
