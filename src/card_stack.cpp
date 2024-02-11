#include "inc/card_stack.h"
#include "spdlog/spdlog.h"

#include "randutils.hpp"

CardStack::CardStack(uint8_t num_cards)
{
	for (uint8_t i = 0; i < num_cards; i++)
	{
		stack.push_back(i);
	}

	if(num_cards > 0)
		Shuffle();
}

CardStack::~CardStack()
{
}

uint8_t CardStack::AddCard(uint8_t card_id)
{
	stack.push_back(card_id);
	return card_id;
}

void CardStack::RemoveCard(uint8_t card_id)
{
	stack.erase(std::remove(stack.begin(), stack.end(), card_id), stack.end());
}

uint8_t CardStack::Draw()
{
	uint8_t ret = stack.back();
	stack.pop_back();
	return ret;
}

void CardStack::Shuffle()
{
	randutils::mt19937_rng rng;

	rng.shuffle(stack.begin(), stack.end());
}

size_t CardStack::GetSize()
{
	if (stack.size() > 255)
	{
		spdlog::error("CardStack::GetSize > 255");
	}

	return stack.size();
}

bool CardStack::HasCard(uint8_t card_id)
{
	return std::find(stack.begin(), stack.end(), card_id) != stack.end();
}

uint8_t* CardStack::GetPointer()
{
	return stack.data();
}
