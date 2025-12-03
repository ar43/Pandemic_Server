#pragma once
#include <winsock2.h>
#include <queue>
#include <string>
#include <assert.h>
#include <memory>
#include "randutils.hpp"
#include "client_state.h"

//class Encryption;

class MsgManager;
class OpcodeManager;
class PlayerInfo;
struct ClientInput;
class Timer;

class Client
{
public:
	Client(SOCKET socket);
	~Client();

	std::shared_ptr<MsgManager> msg_manager;
	std::unique_ptr<OpcodeManager> opcode_manager;
	std::shared_ptr<ClientInput> client_input;

	std::unique_ptr<PlayerInfo> player_info;

	void ReadInput();
	void SendOutput();
	void AddToLobby(int id);

	const uint8_t GetPid();
	void SetPid(uint8_t pid);

	int UpdateAwaiting();
	void Update();

	SOCKET socket;

	int awaiting_substate = 0;
	ClientState state;
	bool dropped = false;
	bool disconnected = false;

private:
	static const size_t PACKET_SIZE = 1024;
	static const uint8_t MAX_NAME_LEN = 29;
	const double TIMEOUT_TIME = 10000.0;
	uint64_t ticks = 0;
	uint8_t pid;
	randutils::mt19937_rng rng;

	std::queue<char> input;
	std::queue<char> output;

	static void PrintSocketError();

	std::unique_ptr<Timer> timeout_timer;

	void Drop(std::string reason);
	void RejectConnection(uint8_t code);

	
	void CheckTimeout();
};