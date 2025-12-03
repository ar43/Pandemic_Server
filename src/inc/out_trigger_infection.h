#pragma once

#include "opcode_out.h"
#include "infection_card.h"
#include <string>
#include <vector>
#include <utility>

class OutTriggerInfection : public OpcodeOut
{
public:
	OutTriggerInfection(uint8_t card_id, std::vector<std::pair<uint8_t, uint8_t>> &infection_data);
	~OutTriggerInfection();

protected:
	void WriteBody(std::shared_ptr<MsgManager> const& msg_manager);
private:
	uint8_t card_id;
	std::vector<std::pair<uint8_t, uint8_t>>& infection_data;

};