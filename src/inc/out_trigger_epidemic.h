#pragma once

#include "opcode_out.h"

class OutTriggerEpidemic : public OpcodeOut
{
public:
	OutTriggerEpidemic(int temp);
	~OutTriggerEpidemic();

	void Send(std::shared_ptr<MsgManager> const& msg_manager);
private:
	int temp;
};