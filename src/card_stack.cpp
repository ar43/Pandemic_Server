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
	auto iter = std::find(stack.begin(), stack.end(), card_id);
	if (iter != stack.end())
	{
		stack.erase(iter);
	}
}

uint8_t CardStack::Draw(bool bottom)
{
	if (GetSize() == 0)
		throw std::out_of_range("CardStack::Draw");
	if (!bottom)
	{
		uint8_t ret = stack.back();
		stack.pop_back();
		return ret;
	}
	else
	{
		uint8_t ret = stack.front();
		stack.erase(stack.begin());
		return ret;
	}
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

void CardStack::Split(std::unique_ptr<CardStack> &bottom, std::unique_ptr<CardStack> &top)
{
	if (stack.size() < 2)
	{
		spdlog::error("CardStack::Split - stack.size() < 2");
		return;
	}

	for (size_t i = 0; i < stack.size(); i++)
	{
		if (i < stack.size() / 2)
		{
			bottom->AddCard(stack.at(i));
		}
		else
		{
			top->AddCard(stack.at(i));
		}
	}
}

void CardStack::Combine(std::unique_ptr<CardStack>& other, bool put_on_top)
{
	size_t original_size = other->GetSize();
	if (put_on_top)
	{
		for (size_t i = 0; i < original_size; i++)
		{
			AddCard(other->Draw(true));
		}
	}
	else
	{
		for (size_t i = 0; i < original_size; i++)
		{
			stack.insert(stack.begin(), other->Draw());
		}
	}
}

uint8_t* CardStack::GetPointer()
{
	return stack.data();
}
