#pragma once
enum class JoinLobbyResponse
{
	LOBBY_OK,
	LOBBY_FULL,
	LOBBY_NOEXIST,

	LOBBY_PENDING = 255
};