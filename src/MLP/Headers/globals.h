#pragma once
#include <vector>
#include <unordered_map>
#include "game_structures.hpp"
#include <d3d11.h>

namespace globals {
	using namespace protocol::engine::sdk;
	using namespace protocol::game::sdk;

	inline RECT game_rect;
	inline HWND game_window;
	inline bool game_window_active;

	inline DWORD64 nopush_addr;
	inline DWORD64 nofall_addr;
	inline DWORD64 noclip_addr;
	inline DWORD64 client_addr;
	inline DWORD64 camera1_addr;
	inline DWORD64 camera2_addr;

	using tProcessEvent = void(__fastcall*)(u_object*, u_function*, void*);

	inline tProcessEvent process_event = nullptr;

	inline u_world* gworld = 0;
	inline a_game_state_base* game_state = 0;
	inline u_game_instance* owning_instance = 0;
	inline u_localplayer* local_player = 0;
	inline a_player_controller* local_controller = 0;
	inline a_player_camera_manager* local_camera_manager = 0;
	inline mec_pawn* local_mec = 0;
	inline u_main_gi_c* main_gi = 0;

	inline std::vector < mec_pawn* > ghost_cache{};
	inline std::vector < mec_pawn* > player_cache{};
	inline std::vector < world_item* > world_item_cache{};
	inline std::vector < a_item* > item_cache{};
	inline std::vector < a_interaction_c* > interaction_cache{};
	inline std::vector < a_weapon_case_code_c* > weapon_case_cache{};

	inline std::unordered_map<std::string, u_data_item*> unique_item_data{};

	inline const char* NoPushMask = "xxxxxxxxxxxx";
	inline const char* NoFallMask = "xxxxxxxx";
	inline const char* NoClipMask = "xxxxxxx";
	inline const char* CameraMask = "xxxxxxx";

	inline BYTE NoPushlShellcode[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	inline BYTE NoFallShellcode[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	inline BYTE NoCliplShellcode[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

	inline BYTE FreeCamShellcode[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	//inline BYTE FreeCamShellcode[] = { 0xE9, 0xD6, 0x78, 0x6C, 0x03, 0x0Fm 0x1F };

	inline BYTE Camera1Pattern[] = { 0x44, 0x0F, 0x11, 0xBF, 0x60, 0x02, 0x00, 0x00 };
	inline BYTE Camera2Pattern[] = { 0x44, 0x0F, 0x11, 0xB7, 0x70, 0x02, 0x00, 0x00 };

	inline BYTE NoPushPattern[] = { 0x28, 0x0F, 0x11, 0x0C, 0xD0, 0xF2, 0x0F, 0x11, 0x44, 0xD0, 0x10, 0x48 };
	inline BYTE NoFallPattern[] = { 0xC8, 0xF2, 0x0F, 0x11, 0x44, 0xC8, 0x10, 0x49 };
	inline BYTE NoClipPattern[] = { 0xF2, 0x43, 0x0F, 0x58, 0x5C, 0xD0, 0x08 };
}