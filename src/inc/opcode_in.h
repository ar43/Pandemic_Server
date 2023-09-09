#pragma once
#include <stdint.h>
#include <memory>
#include "client_opcode.h"

class MsgManager;
struct ClientInput;

class OpcodeIn
{
public:
	OpcodeIn(ClientOpcode id);
	~OpcodeIn();

	virtual void Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input) = 0;
	//uint8_t GetId();
private:
	ClientOpcode id;
};
