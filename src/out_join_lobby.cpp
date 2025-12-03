#include "out_join_lobby.h"
#include "msg_manager.h"
#include "spdlog/spdlog.h"

OutJoinLobby::OutJoinLobby(uint8_t response) : OpcodeOut(ServerOpcode::JOIN_LOBBY)
{
	this->response = response;
}

OutJoinLobby::~OutJoinLobby()
{
}

void OutJoinLobby::WriteBody(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteByte((uint8_t)response);
}
