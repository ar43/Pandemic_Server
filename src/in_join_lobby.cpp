#include "in_join_lobby.h"
#include "client_input.h"
#include "msg_manager.h"

InJoinLobby::InJoinLobby() : OpcodeIn(ClientOpcode::JOIN_LOBBY)
{
}

InJoinLobby::~InJoinLobby()
{
}

void InJoinLobby::Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input)
{
	client_input->requested_lobby = msg_manager->ReadInt();
	auto length = msg_manager->ReadShort();
	if (length > 256)
	{
		msg_manager->SetError();
		return;
	}
	auto msg = msg_manager->ReadString(length);
	client_input->requested_name = msg;
}
