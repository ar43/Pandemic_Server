#pragma once
#include <deque>
#include <utility>
#include <string>
#include <memory>

#include "client_message_type.h"
#include "movement_type.h"

struct Position;

struct ClientInput
{
	ClientInput();
	~ClientInput();
	void Reset();

public:
	MovementType requested_move = MovementType::MOVE_NONE;
	int treat_disease = -1;
	bool ready = false;
	uint8_t target_city = 0;

	std::unique_ptr<std::pair<ClientMessageType, std::string>> client_message;


	int num_actions;
	bool invalid_opcode;

	int discard_card_id;

	int requested_lobby = -1;

	std::string requested_name;
};