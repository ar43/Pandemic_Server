#pragma once

#include "opcode_out.h"
#include <string>

class OutTreatDisease : public OpcodeOut
{
public:
	OutTreatDisease(uint8_t pid, uint8_t type, uint8_t new_count);
	~OutTreatDisease();

protected:
	void WriteBody(std::shared_ptr<MsgManager> const& msg_manager);
private:
	uint8_t pid, type, new_count;
};