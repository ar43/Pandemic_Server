#pragma once
#include "opcode_in.h"

class InReady : public OpcodeIn
{
public:
	InReady();
	~InReady();

	void Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input);
private:
};