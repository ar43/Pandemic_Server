#pragma once
#include <winsock2.h>
#include <queue>
#include <string>
#include <assert.h>
#include <memory>
#include "randutils.hpp"

//class Encryption;

class MsgManager;
class OpcodeManager;
struct ClientInput;

enum ClientState
{
	CSTATE_AWAITING,
	CSTATE_LOBBY,
	CSTATE_GAME
};

class Client
{
public:
	Client(SOCKET socket, randutils::mt19937_rng *rng);
	~Client();

	std::shared_ptr<MsgManager> msg_manager;
	std::unique_ptr<OpcodeManager> opcode_manager;
	std::shared_ptr<ClientInput> client_input;

	void ReadInput();
	void SendOutput();

	uint16_t GetPid();
	void SetPid(uint16_t pid);

	void UpdateAwaiting(uint8_t current_players, uint8_t max_players);
	void Update();

	SOCKET socket;

	int logged_state = 0;
	ClientState state;
	bool dropped = false;
	bool disconnected = false;

	uint8_t position = 0;

private:
	uint64_t time = 0;
	int timeout_counter = 0;
	uint16_t pid;
	bool sent_welcome_message = false;
	randutils::mt19937_rng *rng = nullptr;
	uint32_t current_region = 0;

	std::queue<char> input;
	std::queue<char> output;

	void Drop(std::string reason);
	void RejectConnection(uint8_t code);
	void AddToGame();
	void CheckTimeout();
};