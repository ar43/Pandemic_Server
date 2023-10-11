#pragma once

#include "opcode_out.h"
#include "player_card.h"
#include <string>

class OutUpdatePlayerCard : public OpcodeOut
{
public:
	OutUpdatePlayerCard(uint8_t pid, bool remove, uint8_t num_cards, uint8_t* cards);
	~OutUpdatePlayerCard();

	void Send(std::shared_ptr<MsgManager> const& msg_manager);
private:
	uint8_t pid;
	bool remove;
	uint8_t num_cards;
	uint8_t* cards;
};