#include "out_begin_game.h"
#include "msg_manager.h"

OutBeginGame::OutBeginGame(uint8_t num_players, uint8_t player_id) : OpcodeOut(2)
{
	this->num_players = num_players;
	this->player_id = player_id;
}

OutBeginGame::~OutBeginGame()
{
}

void OutBeginGame::Send(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteOpcode(GetId());
	msg_manager->WriteOpcode(this->num_players);
	msg_manager->WriteOpcode(this->player_id);
}
