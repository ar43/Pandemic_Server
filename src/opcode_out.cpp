#include "opcode_out.h"
OpcodeOut::OpcodeOut(ServerOpcode id)
{
	this->id = id;
}
OpcodeOut::~OpcodeOut() = default;

uint8_t OpcodeOut::GetId()
{
	return (uint8_t)id;
}
