#include "msg_manager.h"

#include "spdlog/spdlog.h"

MsgManager::MsgManager(std::queue<char>* input, std::queue<char>* output)
{
	this->input = input;
	this->output = output;
}

MsgManager::~MsgManager() = default;

void MsgManager::InitEncryption(uint64_t client_session_key, uint64_t server_session_key)
{
	//packetDecryption = std::make_unique<Encryption>(client_session_key, server_session_key, true);
	//printf("!!!!decryption: %llx %llx\n", client_session_key, server_session_key);
	//packetEncryption = std::make_unique<Encryption>(client_session_key, server_session_key, false);
}

uint8_t MsgManager::ReadByte()
{
	if (!input->empty())
	{
		auto ret = (uint8_t)input->front();

		input->pop();
		return ret;
	}
	else
	{
		spdlog::warn("MsgManager::ReadByte: underflow");
		error = true;
		return 0;
	}
}

uint8_t MsgManager::ReadOpcode()
{
	auto opcode = (uint8_t)((ReadByte()) & 0xFF);
	return opcode;
}

uint16_t MsgManager::ReadShort()
{
	uint16_t a = (uint16_t)ReadByte();
	uint16_t b = (uint16_t)ReadByte();

	return (a << 8) + b;
}

uint32_t MsgManager::ReadInt()
{
	auto a = (uint32_t)ReadShort();
	auto b = (uint32_t)ReadShort();
	return (a << 16) + b;
}

uint64_t MsgManager::ReadLong()
{
	auto a = (uint64_t)ReadInt();
	auto b = (uint64_t)ReadInt();
	return (a << 32) + b;
}

std::string MsgManager::ReadString(uint16_t length)
{
	std::string str = "";
	for (int i = 0; i < length; i++)
	{
		char c = (char)ReadByte();
		std::string s{ c };
		str = str + s;
	}
	return str;
}

void MsgManager::ReadDiscard(int num)
{
	for (int i = 0; i < num; i++)
	{
		if (!input->empty())
		{
			input->pop();
		}
	}
}

void MsgManager::WriteByte(uint8_t value)
{
	output->push(value);
}

void MsgManager::WriteOpcode(uint8_t op)
{
	WriteByte((op) & 0xFF);
}

void MsgManager::WriteNull(int len)
{
	for (int i = 0; i < len; i++)
	{
		output->push(0);
	}
}

void MsgManager::WriteShort(uint16_t num)
{
	WriteByte((uint8_t)((num >> 8) & 0xFF));
	WriteByte(num & 0xFF);
}

void MsgManager::WriteInt(uint32_t num)
{
	WriteByte((uint8_t)((num >> 24) & 0xFF));
	WriteByte((uint8_t)((num >> 16) & 0xFF));
	WriteByte((uint8_t)((num >> 8) & 0xFF));
	WriteByte(num & 0xFF);
}

void MsgManager::WriteLong(uint64_t num)
{
	for (int i = 0; i < 8; i++)
	{
		int current = 7 - i;
		uint8_t curr_byte = (uint8_t)((num >> (current * 8)) & 0xff);
		//printf("curr_byte: %d\n", curr_byte);
		output->push(curr_byte);
	}
}

void MsgManager::WriteString(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		WriteByte(str[i]);
	}
}

void MsgManager::StartBitstream()
{
	while (!bit_queue.empty())
		bit_queue.pop();
}

void MsgManager::WriteBits(int size, int val)
{
	for (int i = size-1; i >= 0; i--)
	{
		char bit = (val >> i) & 1;
		bit_queue.push(bit);
	}
}

void MsgManager::WriteBitstream(uint16_t offset)
{
	assert(bit_queue.size() <= UINT16_MAX);
	uint16_t num_bytes = (uint16_t)bit_queue.size() / 8;
	if (bit_queue.size() % 8 > 0)
		num_bytes++;
	WriteShort(num_bytes + offset);

	int test = 0;

	while (!bit_queue.empty())
	{
		uint8_t next_byte = BitstreamGetNextByte();
		WriteByte(next_byte);
		test++;
	}

	assert(test == num_bytes);
}

bool MsgManager::PendingInput()
{
	return !input->empty();
}

uint8_t MsgManager::BitstreamGetNextByte()
{
	int num_bits = std::min(8, (int)bit_queue.size());
	uint8_t value = 0;
	for (int i = 0; i < num_bits; i++)
	{
		auto next_bit = bit_queue.front();
		bit_queue.pop();

		assert(next_bit == 0 || next_bit == 1);

		value |= (next_bit << (7-i));
	}
	return value;
}

bool MsgManager::GetError()
{
	return error;
}
