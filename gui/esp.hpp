#pragma once

#include <vector>
#include <mutex>

#include "state.hpp"
#include "game.h"

#include <imgui/imgui.h>

static bool IsWithinScreenBounds(Vector2& pos)
{
	return pos.x < (float)Screen_get_width(nullptr) && pos.y < (float)Screen_get_height(nullptr);
}

static ImVec2 WorldToScreen(Vector2 pos)
{
	const ImVec2 winsize = { (float)Screen_get_width(nullptr), (float)Screen_get_height(nullptr) };

	// These values are not random, i found them by random testing
	float scale = winsize.y / 1080.0f;
	float view = (180 * scale) / (State.CameraHeight / 3); // The value 180 is specific for 1080p so we need to scale it for other resolutions
	// NOTE : We always need to divide the camera height by 3
	// since 3 is the default zoom in the menu for some reason

	Vector2 localPos = PlayerControl_GetTruePosition(*Game::pLocalPlayer, nullptr);

	/*Camera->GetMain() function*/
	auto mainCamera = Camera_get_main(nullptr);
	Transform* cameraTransform = Component_get_transform(reinterpret_cast<Component*>(mainCamera), nullptr);
	Vector3 cameraPosition = Transform_get_position(cameraTransform, nullptr);

	/*Calculation to compensate for Camera movement*/
	cameraPosition.x = localPos.x - (localPos.x - cameraPosition.x);
	cameraPosition.y = localPos.y - (localPos.y - cameraPosition.y);

	// Here we transform the world position to the screen position
	ImVec2 value;
	value.x = (((pos.x - cameraPosition.x) * view + winsize.x * 0.5f));
	value.y = ((cameraPosition.y - pos.y) * view + winsize.y * 0.5f);

	return value;
}

struct PlayerData
{
	ImVec2 Position = { 0.0f, 0.0f };
	ImVec4 Color{ 0.0f, 0.0f, 0.0f, 0.0f };

	std::string Name = std::string();

	float Distance = 0.0f;
	bool OnScreen = false;
};

typedef struct Drawing
{
	std::mutex m_DrawingMutex;

	static const int MaxPlayersInLevel = 10;
	std::array<PlayerData, MaxPlayersInLevel> m_Players;

	ImVec2 LocalPosition{ 0.0f, 0.0f };
} drawing_t;

class Esp
{
public:
	static void Render();

	static drawing_t& GetDrawing() { return *s_Instance; }
private:
	static drawing_t* s_Instance;
};