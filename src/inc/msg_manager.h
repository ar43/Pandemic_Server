#pragma once

#include <queue>
#include <memory>
#include <string>

class MsgManager
{
public:
	MsgManager(std::queue<char> *input,std::queue<char> *output);
	~MsgManager();

	void InitEncryption(uint64_t client_session_key, uint64_t server_session_key);

	uint8_t ReadByte();
	uint8_t ReadOpcode();
	uint16_t ReadShort();
	uint32_t ReadInt();
	uint64_t ReadLong();
	std::string ReadString(uint16_t length);
	void ReadDiscard(int num);

	void WriteByte(uint8_t value);
	void WriteOpcode(uint8_t op);
	void WriteNull(int len);
	void WriteShort(uint16_t num);
	void WriteInt(uint32_t num);
	void WriteLong(uint64_t num);
	void WriteString(std::string str);

	void StartBitstream();
	void WriteBits(int size, int val);
	void WriteBitstream(uint16_t offset);

	bool PendingInput();
	uint8_t BitstreamGetNextByte();

	bool GetError();
private:
	//std::unique_ptr<Encryption> packetEncryption;
	//std::unique_ptr<Encryption> packetDecryption;
	std::queue<char> bit_queue;
	std::queue<char> *input;
	std::queue<char> *output;
	
	bool error = false;
};