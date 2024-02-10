#pragma once
#include <deque>
#include <utility>
#include <string>
#include <memory>

#include "client_message_type.h"

struct Position;

struct ClientInput
{
	ClientInput();
	~ClientInput();
	void Reset();

public:
	bool requested_move = false;
	bool ready = false;
	uint8_t target_city = 0;

	std::unique_ptr<std::pair<ClientMessageType, std::string>> client_message;


	int num_actions;
	bool invalid_opcode;
};