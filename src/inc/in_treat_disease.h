#pragma once
#include "opcode_in.h"

class InTreatDisease : public OpcodeIn
{
public:
	InTreatDisease();
	~InTreatDisease();

	void Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input);
private:
};