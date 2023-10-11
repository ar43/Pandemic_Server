#include "out_update_player_card.h"
#include "msg_manager.h"

OutUpdatePlayerCard::OutUpdatePlayerCard(uint8_t pid, bool remove, uint8_t num_cards, uint8_t *cards) : OpcodeOut(ServerOpcode::UPDATE_PLAYER_CARD)
{
	this->remove = remove;
	this->num_cards = num_cards;
	this->cards = cards;
	this->pid = pid;
}

OutUpdatePlayerCard::~OutUpdatePlayerCard()
{
}

void OutUpdatePlayerCard::Send(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteOpcode(GetId());
	msg_manager->WriteByte(pid);
	msg_manager->WriteByte((uint8_t)remove);
	msg_manager->WriteByte(num_cards);
	for (int i = 0; i < num_cards; i++)
	{
		msg_manager->WriteByte((uint8_t)cards[i]);
	}
}
