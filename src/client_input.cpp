#include "client_input.h"
#include "spdlog/spdlog.h"

ClientInput::ClientInput()
{
	Reset();
}

ClientInput::~ClientInput()
{
}

void ClientInput::Reset()
{
	num_actions = 0;
	invalid_opcode = false;
	requested_move = false;
	target_city = 0;
	client_message = nullptr;
	ready = false;
	discard_card_id = -1;
	requested_lobby = -1;
	requested_name = "";
}

