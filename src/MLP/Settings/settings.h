#pragma once
#include "vector"
#include "string"
#include "map"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "imgui/imgui.h"
#include "filesystem"
#include "fstream"
#include "thread"
#include "../Headers/game_structures.hpp"
#include <unordered_map>
#include "../Headers/ItemProperties.h"
#include "../Headers/structures.hpp"

using namespace protocol::engine::sdk;
using namespace protocol::game::sdk;

namespace Settings {
	std::string StringConfigName(const std::string& configName);
	std::vector<std::string> GetSaveFiles(const std::filesystem::path directory);

	void LoadSettings(const std::string& filename);
	void SaveSettings(const std::string& filename);

	void SaveWeaponConfig(const std::string& filename, const ItemProperties& itemProperties);
	void LoadWeaponConfig(const std::string& filename);

	void LoadHotkeys();
	void SaveHotkeys();

	void LoadTeleports();
	void SaveTeleports();
	namespace Menu {
		inline ImColor color = ImColor(228, 170, 28);
		inline float animSpeed = 1.0f;
		inline bool rainbowColor = false;
		inline float rainbowSpeed = 1.0f;
		inline float redShift = 1.0f;
		inline float greenShift = 1.0f;
		inline float blueShift = 1.0f;
		inline int hotkey = 0x70;
	}

	namespace FlyHack {
		inline bool enabled = false;
		inline bool directional = false;
		inline bool noFall = true;
		inline bool noPush = true;
		inline bool noClip = false;
		inline int forwardKey = 0x57;
		inline int leftKey = 0x41;
		inline int backwardKey = 0x53;
		inline int rightKey = 0x44;
		inline int upKey = 0x20;
		inline int downKey = 0x11;
		inline float hSpeed = 50.0f;
		inline float vSpeed = 50.0f;
		inline int hotkey = 0x00;
	}

	namespace FreeCam {
		inline bool enabled = false;
		inline int hotkey = 0x00;
	}

	namespace Player {
		inline double friction = 0;
		inline double acceleration = 100;
		inline int stepHandling = 100;
		inline bool alive = true;
		inline float fov = 104.0f;
		inline bool runGun = false;
	}

	namespace Misc {
		inline bool enabled = false;
		inline bool fovTp = false;
		inline float fovTpRadius = 10.0f;
		inline bool drawFov = false;
		inline bool infinityAmmo = false;
		inline int ammoFreezeValue = 1;
		inline bool silentGun = false;
		inline bool speedHack = false;
		inline bool infinityStamina = false;
		inline bool infinityFlyStamina = false;
		inline bool godMode = false;
		inline bool noSpread = false;
		inline bool freezeHandItem = false;
		inline bool freezeBagItem = false;
		inline bool rainbowSkin = false;
		inline int rainbowSkinDelay = 100;
		inline int hotkey = 0x00;

		inline int fovTpHotkey = 0x00;
		inline int hostTpHotkey = 0x00;

		inline u_data_item* saved_hand_item = nullptr;
		inline FStr_ItemState saved_hand_state{};

		inline u_data_item* saved_bag_item = nullptr;
		inline FStr_ItemState saved_bag_state{};
	}

	namespace Aimbot {
		inline bool enabled = false;
		inline bool team = false;
		inline bool enemy = false;
		inline bool silent = false;
		inline bool triggerShoot = false;
		inline bool checkFOV = false;
		inline bool drawFOV = false;
		inline float fov = 20.0f;
		inline double maxDistance = 30.0;
		inline float smooth = 0.0;
		inline int triggerShootDelay = 50;
		inline int hotkey = VK_MENU;

		inline ImVec2 p2 = ImVec2(0.6, 0.8);
		inline ImVec2 p3 = ImVec2(0.4, 0.2);
	}

	namespace ESP {
		namespace Players {
			inline bool enabled = false;
			inline bool preview = false;
			inline bool health = false;
			inline bool stamina = false;
			inline bool inventory = false;
			inline bool lookDirection = false;
			inline bool enemy = false;
			inline bool dissidents = false;
			inline bool deadBody = false;
			inline bool player = false;
			inline bool ghost = false;
			inline bool box = false;
			inline bool skeleton = false;
			inline bool name = false;
			inline bool distance = false;
			inline double maxDistance = 100.0;
			inline bool state = false;
			inline bool snapline = false;
			inline ImVec2 snaplinePos = ImVec2(960, 1080);
			inline int hotkey = 0x00;

			inline ImVec2 nameLocalPos = { 0, 0 };
			inline ImVec2 distanceLocalPos = { 0, 0 };
			inline float textOffset = 0.0f;
			inline float baseOffset = 10.0f;
		}

		namespace Items {
			inline bool enabled = false;

			inline bool primary = false;
			inline bool gazBottle = false;
			inline bool ventFilter = false;
			inline bool package = false;
			inline bool rice = false;
			inline bool pizzushi = false;
			inline bool defibrillator = false;

			inline bool secondary = false;
			inline bool screwDriver = false;
			inline bool sample = false;
			inline bool container = false;
			inline bool fuse = false;
			inline bool fish = false;
			inline bool egg = false;
			inline bool battery = false;
			inline bool cassette = false;
			inline bool accessCard = false;
			inline bool defaultMelee = false;
			inline bool hardDrive = false;

			inline bool weapons = false;
			inline bool revolver = false;
			inline bool pistol = false;
			inline bool shorty = false;
			inline bool knife = false;
			inline bool detonator = false;
			inline bool c4 = false;
			inline bool shotgun = false;
			inline bool rifle = false;
			inline bool smg = false;
			inline bool defaultGun = false;

			inline bool name = false;
			inline bool state = false;
			inline bool distance = false;
			inline bool crate = false;
			inline bool snapline = false;
			inline double maxDistance = 70.0;
			inline int hotkey = 0x00;

			inline static std::map<std::string, bool&> weapons_map = {
				{"KNIFE", knife},
				{"REVOLVER", revolver},
				{"PISTOL", pistol},
				{"DETONATOR", detonator},
				{"C4", c4},
				{"SHORTY", shorty},
				{"SHOTGUN", shotgun},
				{"RIFLE", rifle},
				{"SMG", smg},
				{"DEFAULT GUN", defaultGun}
			};

			inline static std::map<std::string, bool&> primary_map = {
				{"GAZ BOTTLE", gazBottle},
				{"VENT FILTER", ventFilter},
				{"RICE", rice},
				{"PACKAGE", package},
				{"PIZZUSHI", pizzushi},
				{"DEFIBRILLATOR", defibrillator}
			};

			inline static std::map<std::string, bool&> secondary_map = {
				{"BATTERY", battery},
				{"SCREW DRIVER", screwDriver},
				{"CONTAINER", container},
				{"SAMPLE", sample},
				{"CASSETTE", cassette},
				{"FUSE", fuse},
				{"FISH", fish},
				{"EGG", egg},
				{"HARD DRIVE", hardDrive},
				{"ACCESS CARD", accessCard},
				{"DEFAULT MELEE", defaultMelee}
			};
		}

		namespace Colors {
			inline ImColor Weapons = ImColor(255, 255, 255);
			inline ImColor Primary = ImColor(255, 255, 255);
			inline ImColor Secondary = ImColor(255, 255, 255); 

			inline ImColor Dissident = ImColor(255, 0, 0);
			inline ImColor Employee = ImColor(0, 255, 0);
			inline ImColor DissidentGhost = ImColor(255, 165, 0);
			inline ImColor EmployeeGhost = ImColor(255, 165, 0);
			inline ImColor EmployeeBody = ImColor(0, 255, 0);
			inline ImColor DissidentBody = ImColor(255, 0, 0);

			inline ImColor Knife = ImColor(255, 255, 255);
			inline ImColor Revolver = ImColor(255, 255, 255);
			inline ImColor Pistol = ImColor(255, 255, 255);
			inline ImColor Detonator = ImColor(255, 255, 0); 
			inline ImColor C4 = ImColor(255, 255, 0);
			inline ImColor Shorty = ImColor(255, 255, 255);
			inline ImColor Shotgun = ImColor(255, 255, 255);
			inline ImColor Rifle = ImColor(255, 255, 255);
			inline ImColor SMG = ImColor(255, 255, 255);
			inline ImColor DefaultGun = ImColor(255, 255, 255);

			inline ImColor GazBottle = ImColor(255, 255, 255);
			inline ImColor VentFilter = ImColor(255, 255, 255);
			inline ImColor Rice = ImColor(255, 255, 255);
			inline ImColor Package = ImColor(255, 255, 255);
			inline ImColor Pizzushi = ImColor(255, 255, 255);
			inline ImColor Defibrillator = ImColor(255, 255, 255);

			inline ImColor Battery = ImColor(255, 255, 255);
			inline ImColor ScrewDriver = ImColor(255, 255, 255);
			inline ImColor Container = ImColor(255, 255, 255);
			inline ImColor Sample = ImColor(255, 255, 255);
			inline ImColor Cassette = ImColor(255, 255, 255);
			inline ImColor Fuse = ImColor(255, 255, 255);
			inline ImColor Fish = ImColor(255, 255, 255);
			inline ImColor HardDrive = ImColor(255, 255, 255);
			inline ImColor AccessCard = ImColor(255, 255, 255);
			inline ImColor DefaultMelee = ImColor(255, 255, 255);

			inline std::unordered_map<std::string, ImColor&> ColorsMap = {
				// Weapons
				{"KNIFE", Knife},
				{"REVOLVER", Revolver},
				{"PISTOL", Pistol},
				{"DETONATOR", Detonator},
				{"C4", C4},
				{"SHORTY", Shorty},
				{"SHOTGUN", Shotgun},
				{"RIFLE", Rifle},
				{"SMG", SMG},
				{"DEFAULT GUN", DefaultGun},

				// Primary Items
				{"GAZ BOTTLE", GazBottle},
				{"VENT FILTER", VentFilter},
				{"RICE", Rice},
				{"PACKAGE", Package},
				{"PIZZUSHI", Pizzushi},
				{"DEFIBRILLATOR", Defibrillator},

				// Secondary Items
				{"BATTERY", Battery},
				{"SCREW DRIVER", ScrewDriver},
				{"CONTAINER", Container},
				{"SAMPLE", Sample},
				{"CASSETTE", Cassette},
				{"FUSE", Fuse},
				{"FISH", Fish},
				{"HARD DRIVE", HardDrive},
				{"ACCESS CARD", AccessCard},
				{"DEFAULT MELEE", DefaultMelee}
			};
		}

		namespace Tasks {
			inline bool enabled = false;
			inline bool state = false;
			inline bool distance = false;
			inline bool name = false;
			inline double maxDistance = 70.0;

			inline bool alimentations = false;
			inline bool computers = false;
			inline bool pressure = false;
			inline bool delivery = false;
			inline bool pizzushi = false;
			inline bool vents = false;
			inline int hotkey = 0x00;
		}
	}

	namespace Test {
		inline bool enabled = false;
		inline int hotkey = 0x00;
	}
	
	namespace Teleports {
		inline std::vector< Teleport > teleports_cache{};
	}
}

