#include "opcode_out.h"
OpcodeOut::OpcodeOut(uint8_t id)
{
	this->id = id;
}
OpcodeOut::~OpcodeOut() = default;

uint8_t OpcodeOut::GetId()
{
	return id;
}
