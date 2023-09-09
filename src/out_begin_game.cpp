#include "out_begin_game.h"
#include "msg_manager.h"
#include "client.h"

OutBeginGame::OutBeginGame(uint8_t num_players, uint8_t player_id, std::vector<std::string> *player_names, std::vector<PlayerRole> *player_roles) : OpcodeOut(ServerOpcode::BEGIN_GAME)
{
	this->num_players = num_players;
	this->player_id = player_id;
	this->player_names = player_names;
	this->player_roles = player_roles;
}

OutBeginGame::~OutBeginGame()
{
}

void OutBeginGame::Send(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteOpcode(GetId());
	msg_manager->WriteOpcode(this->num_players);
	msg_manager->WriteOpcode(this->player_id);
	for (size_t i = 0; i < this->num_players; i++)
	{
		auto name_len = (uint8_t)player_names->at(i).length();
		msg_manager->WriteByte((uint8_t)this->player_roles->at(i));
		msg_manager->WriteByte(name_len);
		msg_manager->WriteString(player_names->at(i));
	}
}
