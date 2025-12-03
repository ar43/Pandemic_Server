#include "out_update_players.h"
#include "msg_manager.h"
#include "spdlog/spdlog.h"

OutUpdatePlayers::OutUpdatePlayers(uint8_t num_players, uint8_t *locations) : OpcodeOut(ServerOpcode::UPDATE_PLAYERS)
{
	this->num_players = num_players;
	if (locations == NULL)
		spdlog::error("OutUpdatePlayers: null locations");
	this->locations = locations;
}

OutUpdatePlayers::~OutUpdatePlayers()
{
}

void OutUpdatePlayers::WriteBody(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteByte(num_players);
	for (int i = 0; i < num_players; i++)
	{
		msg_manager->WriteByte(locations[i]);
	}
}
