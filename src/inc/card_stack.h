#pragma once
#include <cstdint>
#include <vector>
#include <memory>

class CardStack
{
public:
	CardStack(uint8_t num_cards);
	~CardStack();

	uint8_t AddCard(uint8_t card_id);
	void RemoveCard(uint8_t card_id);
	uint8_t Draw(bool bottom = false);
	void Shuffle();
	size_t GetSize();
	bool HasCard(uint8_t card_id);
	void Split(std::unique_ptr<CardStack> &bottom, std::unique_ptr<CardStack> &top);
	void Combine(std::unique_ptr<CardStack>& other, bool put_on_top);

	uint8_t *GetPointer();

private:
	std::vector<uint8_t> stack;
};