#pragma once
#include "opcode_in.h"

class InMove : public OpcodeIn
{
public:
	InMove();
	~InMove();

	void Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input);
private:
};