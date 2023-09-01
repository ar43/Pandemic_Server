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

uint8_t MsgManager::ReadByte(DataSubtype type)
{
	if (!input->empty())
	{
		auto ret = (uint8_t)input->front();

		switch (type)
		{
			case DataSubtype::Normal:
			{
				break;
			}
			case DataSubtype::SpecialA:
			{
				ret = ret - 128;
				break;
			}
			case DataSubtype::SpecialS:
			{
				ret = 128 - ret;
				break;
			}
			case DataSubtype::SpecialC:
			{
				ret = 0 - ret;
				break;
			}
		}

		input->pop();
		return ret;
	}
	else
	{
		spdlog::error("MsgManager::ReadByte: underflow");
		return 0;
	}
}

uint8_t MsgManager::ReadOpcode()
{
	auto opcode = (uint8_t)((ReadByte()) & 0xFF);
	return opcode;
}

uint16_t MsgManager::ReadShort(ByteOrder order, DataSubtype type)
{
	uint16_t a = 0;
	uint16_t b = 0;

	switch (order)
	{
		case ByteOrder::BigEndian:
		{
			a = (uint16_t)ReadByte();
			b = (uint16_t)ReadByte(type);
			break;
		}
		case ByteOrder::LittleEndian:
		{
			b = (uint16_t)ReadByte(type);
			a = (uint16_t)ReadByte();
			break;
		}
		default:
		{
			spdlog::error("MsgManager::ReadShort: invalid order");
			break;
		}
	}

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

std::string MsgManager::ReadString()
{
	std::string str = "";
	char c = (char)ReadByte();
	while (c != '\n')
	{
		std::string s{c};
		str = str + s;
		c = (char)ReadByte();
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

void MsgManager::WriteByte(uint8_t value, DataSubtype type)
{
	switch (type)
	{
	case DataSubtype::Normal:
	{
		break;
	}
	case DataSubtype::SpecialC:
	{
		value = 0 - value;
		break;
	}
	case DataSubtype::SpecialA:
	{
		value += 128;
		break;
	}
	case DataSubtype::SpecialS:
	{
		value = 128 - value;
		break;
	}
	default:
	{
		spdlog::error("WriteByte: unhandled DataSubtype");
		break;
	}
	}
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

void MsgManager::WriteShort(uint16_t num, ByteOrder order, DataSubtype type)
{
	switch (order)
	{
		case ByteOrder::LittleEndian:
		{
			WriteByte(num & 0xFF, type);
			WriteByte((uint8_t)((num >> 8) & 0xFF));
			break;
		}
		case ByteOrder::BigEndian:
		{
			WriteByte((uint8_t)((num >> 8) & 0xFF));
			WriteByte(num & 0xFF, type);
			break;
		}
		default:
		{
			spdlog::error("WriteInt: unhandled ByteOrder");
			break;
		}
	}
	
}

void MsgManager::WriteInt(uint32_t num, ByteOrder order, DataSubtype type)
{
	switch (order)
	{
		case ByteOrder::LittleEndian:
		{
			WriteByte(num & 0xFF, type);
			WriteByte((uint8_t)((num >> 8) & 0xFF));
			WriteByte((uint8_t)((num >> 16) & 0xFF));
			WriteByte((uint8_t)((num >> 24) & 0xFF));
			break;
		}
		case ByteOrder::BigEndian:
		{
			WriteByte((uint8_t)((num >> 24) & 0xFF));
			WriteByte((uint8_t)((num >> 16) & 0xFF));
			WriteByte((uint8_t)((num >> 8) & 0xFF));
			WriteByte(num & 0xFF, type);
			break;
		}
		case ByteOrder::MiddleEndianBigInt:
		{
			WriteByte((uint8_t)((num >> 8) & 0xFF));
			WriteByte(num & 0xFF, type);
			WriteByte((uint8_t)((num >> 24) & 0xFF));
			WriteByte((uint8_t)((num >> 16) & 0xFF));
			break;
		}
		default:
		{
			spdlog::error("WriteInt: unhandled ByteOrder");
			break;
		}
	}
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
	for (int i = 0; i < str.length(); i++)
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