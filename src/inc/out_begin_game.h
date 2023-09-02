#pragma once

#include "opcode_out.h"
#include <string>

class OutBeginGame : public OpcodeOut
{
public:
	OutBeginGame(uint8_t num_players, uint8_t player_id);
	~OutBeginGame();

	void Send(std::shared_ptr<MsgManager> const& msg_manager);
private:
	uint8_t num_players;
	uint8_t player_id;
};