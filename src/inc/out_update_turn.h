#pragma once

#include "opcode_out.h"
#include "turn_update_type.h"
#include <string>

class OutUpdateTurn : public OpcodeOut
{
public:
	OutUpdateTurn(TurnUpdateType turn_update_type, uint8_t pid, uint8_t actions);
	~OutUpdateTurn();

protected:
	void WriteBody(std::shared_ptr<MsgManager> const& msg_manager);
private:
	TurnUpdateType turn_update_type;
	uint8_t pid, actions;
};