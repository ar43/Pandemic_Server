#pragma once

#include "opcode_out.h"
#include <string>

class OutJoinLobby : public OpcodeOut
{
public:
	OutJoinLobby(uint8_t response);
	~OutJoinLobby();

protected:
	void WriteBody(std::shared_ptr<MsgManager> const& msg_manager);
private:
	uint8_t response;
};