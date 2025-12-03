#pragma once

#include "opcode_out.h"
#include <string>

class OutUpdatePlayers : public OpcodeOut
{
public:
	OutUpdatePlayers(uint8_t num_players, uint8_t *locations);
	~OutUpdatePlayers();

protected:
	void WriteBody(std::shared_ptr<MsgManager> const& msg_manager);
private:
	uint8_t num_players;
	uint8_t* locations;
};