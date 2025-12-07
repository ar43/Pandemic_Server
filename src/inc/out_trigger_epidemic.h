#pragma once

#include "opcode_out.h"

class OutTriggerEpidemic : public OpcodeOut
{
public:
	OutTriggerEpidemic(uint8_t pid);
	~OutTriggerEpidemic();

protected:
	void WriteBody(std::shared_ptr<MsgManager> const& msg_manager);
private:
	uint8_t pid;
};