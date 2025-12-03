#pragma once
#include "opcode_in.h"

class InJoinLobby : public OpcodeIn
{
public:
	InJoinLobby();
	~InJoinLobby();

	void Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input);
private:
};