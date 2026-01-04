#pragma once
#include "opcode_in.h"

class InResearch : public OpcodeIn
{
public:
	InResearch();
	~InResearch();

	void Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input);
private:
};