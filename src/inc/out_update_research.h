#pragma once

#include "opcode_out.h"
#include <string>

class OutUpdateResearch : public OpcodeOut
{
public:
	OutUpdateResearch(uint8_t cures, uint8_t num_stations, uint8_t* locations);
	~OutUpdateResearch();

protected:
	void WriteBody(std::shared_ptr<MsgManager> const& msg_manager);
private:
	uint8_t cures;
	uint8_t num_stations;
	uint8_t* locations;
};