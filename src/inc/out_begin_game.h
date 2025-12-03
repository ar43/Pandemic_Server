#pragma once

#include "opcode_out.h"
#include "player_info.h"
#include <string>
#include <vector>

class OutBeginGame : public OpcodeOut
{
public:
	OutBeginGame(uint8_t num_players, uint8_t player_id, std::vector<std::string> *player_names, std::vector<PlayerRole> *player_roles);
	~OutBeginGame();

protected:
	void WriteBody(std::shared_ptr<MsgManager> const& msg_manager);
private:
	std::vector<std::string> *player_names;
	std::vector<PlayerRole> *player_roles;
	uint8_t num_players;
	uint8_t player_id;
};