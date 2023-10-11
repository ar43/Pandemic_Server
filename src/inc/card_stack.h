#pragma once
#include <cstdint>
#include <vector>

class CardStack
{
public:
	CardStack(uint8_t num_cards);
	~CardStack();

	uint8_t AddCard(uint8_t card_id);
	void RemoveCard(uint8_t card_id);
	uint8_t Draw();
	void Shuffle();
	size_t GetSize();

	uint8_t *GetPointer();

private:
	std::vector<uint8_t> stack;
};