#pragma once
#include <memory>
class MsgManager;
class OpcodeOut;
class OpcodeIn;
struct ClientInput;
struct Position;

class OpcodeManager
{
public:
	OpcodeManager(std::shared_ptr<MsgManager> msg_manager);
	~OpcodeManager();

	void Send(OpcodeOut& opcode);

	void Receive(std::shared_ptr<ClientInput> const& client_input, uint16_t pid);

private:
	std::shared_ptr<MsgManager> msg_manager;
	std::unique_ptr<OpcodeIn> GetOpcode(uint8_t id);
};