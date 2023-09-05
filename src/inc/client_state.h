#pragma once

enum class ClientState
{
	CSTATE_UNCONNECTED, //unused server-side
	CSTATE_AWAITING,
	CSTATE_LOBBY,
	CSTATE_GAME
};