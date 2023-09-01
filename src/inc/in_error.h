#pragma once
#include "opcode_in.h"

class InError : public OpcodeIn
{
public:
	InError();
	~InError();

	void Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input);
private:
};