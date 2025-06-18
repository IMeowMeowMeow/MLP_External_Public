#pragma once
#include "../Memory/memory.hpp"
#include "../Settings/settings.h"
#include "ItemProperties.h"
#include "../Settings/Hotkeys/hotkey.h"
#include "game_structures.hpp"
#include "game_math.hpp"
#include "enums.h"
#include "globals.h"
#include <Windows.h>
#include <cmath>
#include <sstream>
#include <iomanip> 
#include <iostream> 
#include <algorithm> 
#include "common.hpp"
#include <string>

#define M_PI 3.14159265358979323846

using namespace globals;
namespace util {
	using namespace protocol::engine::sdk;
	using namespace protocol::game::sdk;

	static matrix4x4_t matrix(vector3 rot, vector3 origin = vector3(0, 0, 0))
	{
		double rad_pitch = (rot.x * (float)M_PI / 180.);
		double rad_yaw = (rot.y * (float)M_PI / 180.);
		double rad_roll = (rot.z * (float)M_PI / 180.);

		double SP = sin(rad_pitch);
		double CP = cos(rad_pitch);
		double SY = sin(rad_yaw);
		double CY = cos(rad_yaw);
		double SR = sin(rad_roll);
		double CR = cos(rad_roll);

		matrix4x4_t matrix;
		matrix.m[0][0] = CP * CY;
		matrix.m[0][1] = CP * SY;
		matrix.m[0][2] = SP;
		matrix.m[0][3] = 0.f;

		matrix.m[1][0] = SR * SP * CY - CR * SY;
		matrix.m[1][1] = SR * SP * SY + CR * CY;
		matrix.m[1][2] = -SR * CP;
		matrix.m[1][3] = 0.f;

		matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		matrix.m[2][1] = CY * SR - CR * SP * SY;
		matrix.m[2][2] = CR * CP;
		matrix.m[2][3] = 0.f;

		matrix.m[3][0] = origin.x;
		matrix.m[3][1] = origin.y;
		matrix.m[3][2] = origin.z;
		matrix.m[3][3] = 1.f;

		return matrix;
	}

	inline static vector2 screen_size() {
		static auto screen_size = vector2{};
		if (screen_size != vector2{ 0, 0 }) return screen_size;

		RECT rect;
		GetWindowRect(GetDesktopWindow(), &rect);
		screen_size = vector2{ (float)(rect.right - rect.left), (float)(rect.bottom - rect.top) };

		return screen_size;
	}

	inline static bool w2s(vector3 location, f_minimal_view_info camera_cache, ImVec2& screen_loc) {
		auto pov = camera_cache;
		vector3 rotation = pov.rotation;
		matrix4x4_t temp_matrix = matrix(rotation);

		float fov = pov.fov;

		vector3 v_axis_x = vector3(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
		vector3 v_axis_y = vector3(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
		vector3 v_axis_z = vector3(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);

		vector3 v_delta = location - pov.location;
		vector3 v_transformed = vector3(v_delta.dot(v_axis_y), v_delta.dot(v_axis_z), v_delta.dot(v_axis_x));

		if (v_transformed.z < 0.1f)
			return false;


		screen_loc.x = (screen_size().x / 2) + v_transformed.x * ((screen_size().x / 2) / tanf(fov * (float)3.14159f / 360.f)) / v_transformed.z;
		screen_loc.y = (screen_size().y / 2) - v_transformed.y * ((screen_size().x / 2) / tanf(fov * (float)3.14159f / 360.f)) / v_transformed.z;

		if (screen_loc.x > screen_size().x ||
			screen_loc.y > screen_size().y ||
			screen_loc.y < 0 ||
			screen_loc.x < 0) return false;

		return true;
	}

	inline static bool w2s(vector3 location, f_minimal_view_info camera_cache, vector3& screen_loc) {
		auto pov = camera_cache;
		vector3 rotation = pov.rotation;
		matrix4x4_t temp_matrix = matrix(rotation);

		float fov = pov.fov;

		vector3 v_axis_x = vector3(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
		vector3 v_axis_y = vector3(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
		vector3 v_axis_z = vector3(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);

		vector3 v_delta = location - pov.location;
		vector3 v_transformed = vector3(v_delta.dot(v_axis_y), v_delta.dot(v_axis_z), v_delta.dot(v_axis_x));

		if (v_transformed.z < 0.1f)
			return false;

		screen_loc.x = (screen_size().x / 2) + v_transformed.x * ((screen_size().x / 2) / tanf(fov * (float)M_PI / 360.f)) / v_transformed.z;
		screen_loc.y = (screen_size().y / 2) - v_transformed.y * ((screen_size().x / 2) / tanf(fov * (float)M_PI / 360.f)) / v_transformed.z;

		if (screen_loc.x > screen_size().x ||
			screen_loc.y > screen_size().y ||
			screen_loc.y < 0 ||
			screen_loc.x < 0) return false;

		return true;
	}

	inline ItemGroup GetItemGroup(const std::string& itemName) {
		static const std::vector<std::string> weapons = {
			"KNIFE", "REVOLVER", "PISTOL", "DETONATOR", "C4",
			"SHORTY", "SHOTGUN", "RIFLE", "SMG", "DEFAULT GUN"
		};

		static const std::vector<std::string> primary = {
			"GAZ BOTTLE", "VENT FILTER", "RICE", "PACKAGE", "PIZZUSHI", "DEFIBRILLATOR"
		};

		static const std::vector<std::string> secondary = {
			"BATTERY", "SCREW DRIVER", "CONTAINER", "SAMPLE", "CASSETTE",
			"FUSE", "FISH", "HARD DRIVE", "ACCESS CARD", "DEFAULT MELEE"
		};

		if (std::find(weapons.begin(), weapons.end(), itemName) != weapons.end()) {
			return ItemGroup::Weapons;
		}

		if (std::find(primary.begin(), primary.end(), itemName) != primary.end()) {
			return ItemGroup::Primary;
		}

		if (std::find(secondary.begin(), secondary.end(), itemName) != secondary.end()) {
			return ItemGroup::Secondary;
		}

		return ItemGroup::Unknown;
	}

	template <typename T>
	inline T Clamp(const T& value, const T min, const T max) {
		return (value < min) ? min : (value > max) ? max : value;
	}

	template <typename T>
	inline T Min(const T& a, const T& b) {
		return a < b ? a : b;
	}

	template <typename T>
	inline T Max(const T& a, const T& b) {
		return a > b ? a : b;
	}

	inline void SendKey(WORD vKey, bool extended = false) {
		INPUT input[1] = { 0 };
		input[0].type = INPUT_KEYBOARD;
		input[0].ki.wVk = vKey;
		input[0].ki.dwFlags = extended ? KEYEVENTF_EXTENDEDKEY : 0;
		SendInput(1, input, sizeof(INPUT));

		// Small delay (important for some applications)
		Sleep(50);

		// Release the key
		input[0].ki.dwFlags = KEYEVENTF_KEYUP | (extended ? KEYEVENTF_EXTENDEDKEY : 0);
		SendInput(1, input, sizeof(INPUT));
	}



	inline static vector2 CalcAngle(vector3& src, vector3& dst) {

		vector2 angle{};

		angle.x = -atan2f(dst.x - src.x, dst.y - src.y) / M_PI * 180.0f + 90.0f;
		angle.y = asinf(Clamp(static_cast<float>((dst.z - src.z) / src.distance(dst)), -1.0f, 1.0f)) * 180.0f / M_PI;

		return angle;
	}

	static vector2 cursor_position() {
		POINT cursor_point{};
		if (!GetCursorPos(&cursor_point))
			return vector2{ 0.f,0.f };
		return vector2{ static_cast<float>(cursor_point.x), static_cast<float>(cursor_point.y) };
	}

	static std::string get_name_from_fname(int key)
	{
		static std::map<int, std::string> cached_fnames{};

		auto cached_name = cached_fnames.find(key);
		if (cached_name != cached_fnames.end())
			return cached_name->second;

		if (key <= 0) return ""; 

		auto chunkOffset = (UINT)((int)(key) >> 16);
		auto name_offset = (USHORT)key;

		auto pool_chunk = memory::read<UINT64>(memory::module_base + protocol::engine::GNAMES + ((chunkOffset + 2) * 8));
		if (!pool_chunk) return "";

		auto entry_offset = pool_chunk + (ULONG)(2 * name_offset);
		auto name_entry = memory::read<INT16>(entry_offset);

		if (!name_entry) return ""; 

		auto len = name_entry >> 6;
		if (len <= 0 || len >= 1028) return ""; 

		char buff[1028] = { 0 }; 
		memory::read_raw(entry_offset + 2, buff, len);
		buff[len] = '\0';

		std::string ret(buff);
		cached_fnames.emplace(key, ret);
		return ret;
	}


	inline static double CalculateDistanceMeters(const vector3& location1, const vector3& location2) {
		double dx = location1.x - location2.x;
		double dy = location1.y - location2.y;
		double dz = location1.z - location2.z;

		return std::sqrt(dx * dx + dy * dy + dz * dz) / 100.0;
	}

	inline static std::string CalculateDistance(const FVector& location1, const vector3& location2) {
		double dx = location1.X - location2.x;
		double dy = location1.Y - location2.y;
		double dz = location1.Z - location2.z;

		double distance = std::sqrt(dx * dx + dy * dy + dz * dz) / 100.0;

		std::ostringstream stream;
		stream << std::fixed << std::setprecision(0) << distance;
		return stream.str();
	}

	inline static vector3 ConvertFVectorToVector3(const FVector& fvec) {
		return vector3{ static_cast<float>(fvec.X), static_cast<float>(fvec.Y), static_cast<float>(fvec.Z) };
	}

	inline static float CalculateDistanceFloat(const vector3& player_pos, const vector3& enemy_pos) {
		return sqrtf(
			powf(enemy_pos.x - player_pos.x, 2.0f) +
			powf(enemy_pos.y - player_pos.y, 2.0f) +
			powf(enemy_pos.z - player_pos.z, 2.0f)
		);
	}

	inline static std::string GetValidName(u_data_item* item_data) {
		std::string item_name = "EMPTY";


		if (item_data) {
			item_name = item_data->get_name().read_string();
			if (item_name == "NAME") {
				static std::unordered_map<std::string, std::string> default_items = {
					{"Default__Data_Gun_C", "DEFAULT GUN"},
					{"Default__Data_Melee_C", "DEFAULT MELEE"},
					{"Default__Data_Consumable_C", "DEFAULT CONSUMABLE"},
				};

				auto object_name = util::get_name_from_fname(item_data->fname_index());
				for (auto& item : default_items) {
					if (object_name.find(item.first) != std::string::npos) {
						item_name = item.second;
						return item_name;
					}
				}
			}
			else if (item_name == "MACHINE PART") {
				static std::unordered_map<std::string, std::string> machine_parts = {
					{"DA_MachinePart_A", "MACHINE PART A"},
					{"DA_MachinePart_B", "MACHINE PART B"},
					{"DA_MachinePart_C", "MACHINE PART C"},
					{"DA_MachinePart_D", "MACHINE PART D"},
				};

				auto object_name = util::get_name_from_fname(item_data->fname_index());
				for (auto& part : machine_parts) {
					if (object_name.find(part.first) != std::string::npos) {
						item_name = part.second;
						return item_name;
					}
				}
			}
		}

		return item_name;
	}

	inline static std::string GetAppData() {
		char* appDataPath;
		size_t len;
		_dupenv_s(&appDataPath, &len, "APPDATA");

		std::string appDataFolder(appDataPath);
		free(appDataPath);

		return appDataFolder;
	}

	inline static std::string GetLocalAppData() {
		char* localAppDataPath;
		size_t len;
		_dupenv_s(&localAppDataPath, &len, "LOCALAPPDATA");

		std::string localAppDataFolder(localAppDataPath);
		free(localAppDataPath);

		return localAppDataFolder;
	}

	inline void OpenConsole() {
		if (AllocConsole()) {
			FILE* f;
			freopen_s(&f, "CONOUT$", "w", stdout);
			freopen_s(&f, "CONIN$", "r", stdin);
		}
	}

	inline void CloseConsole() {
		FILE* f;
		fclose(stdout);
		fclose(stdin);
		FreeConsole();
		std::cout.clear();
		std::cin.clear();
	}

	inline uint64_t ConvertSteamID32To64(uint32_t steamID32) {
		return 76561197960265728ULL + steamID32;
	}

	inline std::wstring ConvertToWideString(const std::string& utf8Str) {
		if (utf8Str.empty()) {
			return L"";
		}

		int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), (int)utf8Str.size(), NULL, 0);
		std::wstring wide_string(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), (int)utf8Str.size(), &wide_string[0], size_needed);
		return wide_string;
	}

	inline std::string ConvertToUTF8String(const std::wstring& wideStr) {
		if (wideStr.empty()) {
			return "";
		}

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), (int)wideStr.size(), NULL, 0, NULL, NULL);
		std::string utf8_string(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), (int)wideStr.size(), &utf8_string[0], size_needed, NULL, NULL);
		return utf8_string;
	}

	inline rapidjson::Value Vector3ToJson(const vector3& vec, rapidjson::Document::AllocatorType& allocator) {
		rapidjson::Value obj(rapidjson::kObjectType);
		obj.AddMember("x", vec.x, allocator);
		obj.AddMember("y", vec.y, allocator);
		obj.AddMember("z", vec.z, allocator);
		return obj;
	}

	inline vector3 JsonToVector3(const rapidjson::Value& obj) {
		return vector3{ obj["x"].GetFloat(), obj["y"].GetFloat(), obj["z"].GetFloat() };
	}

	inline rapidjson::Value Vector2ToJson(const vector2& vec, rapidjson::Document::AllocatorType& allocator) {
		rapidjson::Value obj(rapidjson::kObjectType);
		obj.AddMember("x", vec.x, allocator);
		obj.AddMember("y", vec.y, allocator);
		return obj;
	}

	inline vector2 JsonToVector2(const rapidjson::Value& obj) {
		return vector2{ obj["x"].GetFloat(), obj["y"].GetFloat() };
	}

	inline rapidjson::Value ImVec2ToJson(const ImVec2& vec, rapidjson::Document::AllocatorType& allocator) {
		rapidjson::Value obj(rapidjson::kObjectType);
		obj.AddMember("x", vec.x, allocator);
		obj.AddMember("y", vec.y, allocator);
		return obj;
	}

	inline ImVec2 JsonToImVec2(const rapidjson::Value& obj) {
		return ImVec2{ obj["x"].GetFloat(), obj["y"].GetFloat() };
	}

	inline rapidjson::Value ItemPropertiesToJson(const ItemProperties& itemProperties, rapidjson::Document::AllocatorType& allocator) {
		rapidjson::Value obj(rapidjson::kObjectType);

		rapidjson::Value nameValue;
		nameValue.SetString(itemProperties.name.c_str(), allocator);
		obj.AddMember("name", nameValue, allocator);
		obj.AddMember("is_melee", itemProperties.is_melee, allocator);
		obj.AddMember("can_inventory", itemProperties.can_inventory, allocator);
		obj.AddMember("stamina_cost", itemProperties.stamina_cost, allocator);

		if (itemProperties.is_melee) {
			obj.AddMember("frontal_damage_health", itemProperties.frontal_damage_health, allocator);
			obj.AddMember("frontal_damage_stamina", itemProperties.frontal_damage_stamina, allocator);
			obj.AddMember("frontal_damage_slow", itemProperties.frontal_damage_slow, allocator);

			obj.AddMember("back_damage_health", itemProperties.back_damage_health, allocator);
			obj.AddMember("back_damage_stamina", itemProperties.back_damage_stamina, allocator);
			obj.AddMember("back_damage_slow", itemProperties.back_damage_slow, allocator);

			obj.AddMember("attack_range", itemProperties.attack_range, allocator);
			obj.AddMember("attack_speed", itemProperties.attack_speed, allocator);
			obj.AddMember("cast_time", itemProperties.cast_time, allocator);
		}
		else {
			obj.AddMember("body_damage_health", itemProperties.body_damage_health, allocator);
			obj.AddMember("body_damage_stamina", itemProperties.body_damage_stamina, allocator);
			obj.AddMember("body_damage_slow", itemProperties.body_damage_slow, allocator);

			obj.AddMember("head_damage_health", itemProperties.head_damage_health, allocator);
			obj.AddMember("head_damage_stamina", itemProperties.head_damage_stamina, allocator);

			obj.AddMember("fire_rate", itemProperties.fire_rate, allocator);
			obj.AddMember("max_ammo", itemProperties.max_ammo, allocator);
			obj.AddMember("impact_type", itemProperties.impact_type, allocator);
			obj.AddMember("pattern_type", itemProperties.pattern_type, allocator);
			obj.AddMember("auto_fire", itemProperties.auto_fire, allocator);
		}

		return obj;
	}

	inline ItemProperties JsonToItemProperties(const rapidjson::Value& obj) {
		ItemProperties itemProperties{};

		if (!obj.IsObject()) return itemProperties;

		if (obj.HasMember("name") && obj["name"].IsString()) {
			itemProperties.name = obj["name"].GetString();
		}

		if (obj.HasMember("is_melee") && obj["is_melee"].IsBool()) {
			itemProperties.is_melee = obj["is_melee"].GetBool();
		}

		if (itemProperties.is_melee) {
			if (obj.HasMember("frontal_damage_health")) itemProperties.frontal_damage_health = obj["frontal_damage_health"].GetInt();
			if (obj.HasMember("frontal_damage_stamina")) itemProperties.frontal_damage_stamina = obj["frontal_damage_stamina"].GetInt();
			if (obj.HasMember("frontal_damage_slow")) itemProperties.frontal_damage_slow = obj["frontal_damage_slow"].GetDouble();

			if (obj.HasMember("back_damage_health")) itemProperties.back_damage_health = obj["back_damage_health"].GetInt();
			if (obj.HasMember("back_damage_stamina")) itemProperties.back_damage_stamina = obj["back_damage_stamina"].GetInt();
			if (obj.HasMember("back_damage_slow")) itemProperties.back_damage_slow = obj["back_damage_slow"].GetDouble();

			if (obj.HasMember("attack_range")) itemProperties.attack_range = obj["attack_range"].GetInt();
			if (obj.HasMember("attack_speed")) itemProperties.attack_speed = obj["attack_speed"].GetDouble();
			if (obj.HasMember("cast_time")) itemProperties.cast_time = obj["cast_time"].GetDouble();
		}
		else {
			if (obj.HasMember("body_damage_health")) itemProperties.body_damage_health = obj["body_damage_health"].GetInt();
			if (obj.HasMember("body_damage_stamina")) itemProperties.body_damage_stamina = obj["body_damage_stamina"].GetInt();
			if (obj.HasMember("body_damage_slow")) itemProperties.body_damage_slow = obj["body_damage_slow"].GetDouble();

			if (obj.HasMember("head_damage_health")) itemProperties.head_damage_health = obj["head_damage_health"].GetInt();
			if (obj.HasMember("head_damage_stamina")) itemProperties.head_damage_stamina = obj["head_damage_stamina"].GetInt();

			if (obj.HasMember("fire_rate")) itemProperties.fire_rate = obj["fire_rate"].GetDouble();
			if (obj.HasMember("max_ammo")) itemProperties.max_ammo = obj["max_ammo"].GetInt();
			if (obj.HasMember("impact_type")) itemProperties.impact_type = obj["impact_type"].GetInt();
			if (obj.HasMember("pattern_type")) itemProperties.pattern_type = obj["pattern_type"].GetInt();
			if (obj.HasMember("auto_fire")) itemProperties.auto_fire = obj["auto_fire"].GetBool();
		}

		if (obj.HasMember("can_inventory") && obj["can_inventory"].IsBool()) {
			itemProperties.can_inventory = obj["can_inventory"].GetBool();
		}

		if (obj.HasMember("stamina_cost") && obj["stamina_cost"].IsInt()) {
			itemProperties.stamina_cost = obj["stamina_cost"].GetInt();
		}

		return itemProperties;
	}

	inline void SetHandItem(const std::string& item_name) {
		if (!local_mec) return;
		auto it = unique_item_data.find(item_name);
		if (it != unique_item_data.end()) {
			local_mec->set_hand_item(it->second);
			auto item_stats = GetItemProperties(item_name);
			FStr_ItemState item_state{};
			item_state.Value_8 = item_stats.max_ammo;
			if (item_state.Value_8 || item_state.Time_15) {
				local_mec->set_hand_state(item_state);
			}
		}
	}

	inline void SetBagItem(const std::string& item_name) {
		if (!local_mec) return;
		auto it = unique_item_data.find(item_name);
		if (it != unique_item_data.end()) {
			local_mec->set_bag_item(it->second);
			auto item_stats = GetItemProperties(item_name);
			FStr_ItemState item_state{};
			item_state.Value_8 = item_stats.max_ammo;
			if (item_state.Value_8 || item_state.Time_15) {
				local_mec->set_bag_state(item_state);
			}
		}
	}
}