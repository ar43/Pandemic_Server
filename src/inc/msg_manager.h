#pragma once

#include <queue>
#include <memory>
#include <string>

enum class DataSubtype
{
	Normal,
	SpecialA,
	SpecialC,
	SpecialS
};

enum class ByteOrder
{
	BigEndian,
	LittleEndian,
	MiddleEndianBigInt,
	MiddleEndianSmallInt
};

class MsgManager
{
public:
	MsgManager(std::queue<char> *input,std::queue<char> *output);
	~MsgManager();

	void InitEncryption(uint64_t client_session_key, uint64_t server_session_key);

	uint8_t ReadByte(DataSubtype type = DataSubtype::Normal);
	uint8_t ReadOpcode();
	uint16_t ReadShort(ByteOrder order = ByteOrder::BigEndian, DataSubtype type = DataSubtype::Normal);
	uint32_t ReadInt();
	uint64_t ReadLong();
	std::string ReadString();
	void ReadDiscard(int num);

	void WriteByte(uint8_t value, DataSubtype type = DataSubtype::Normal);
	void WriteOpcode(uint8_t op);
	void WriteNull(int len);
	void WriteShort(uint16_t num, ByteOrder order = ByteOrder::BigEndian, DataSubtype type = DataSubtype::Normal);
	void WriteInt(uint32_t num, ByteOrder order = ByteOrder::BigEndian, DataSubtype type = DataSubtype::Normal);
	void WriteLong(uint64_t num);
	void WriteString(std::string str);

	void StartBitstream();
	void WriteBits(int size, int val);
	void WriteBitstream(uint16_t offset);

	bool PendingInput();
	uint8_t BitstreamGetNextByte();
private:
	//std::unique_ptr<Encryption> packetEncryption;
	//std::unique_ptr<Encryption> packetDecryption;
	std::queue<char> bit_queue;
	std::queue<char> *input;
	std::queue<char> *output;
};