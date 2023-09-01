#pragma once
#include <stdint.h>
#include <memory>

class MsgManager;
struct ClientInput;

class OpcodeIn
{
public:
	OpcodeIn(uint8_t id);
	~OpcodeIn();

	virtual void Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input) = 0;
	//uint8_t GetId();
private:
	uint8_t id;
};
