#pragma once
#include "opcode_in.h"

class InIdle : public OpcodeIn
{
public:
	InIdle();
	~InIdle();

	void Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input);
private:
};