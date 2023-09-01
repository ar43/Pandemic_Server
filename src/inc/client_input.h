#pragma once
#include <deque>

struct Position;

struct ClientInput
{
	ClientInput();
	~ClientInput();
	void Reset();

public:
	bool requested_move = false;
	uint8_t target_city = 0;


	int num_actions;
	bool invalid_opcode;
};