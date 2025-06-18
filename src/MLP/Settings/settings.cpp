#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "imgui/imgui.h"
#include "settings.h"
#include "fstream"
#include "thread"
#include <codecvt>
#include <locale>
#include "../Headers/util.hpp"
#include "../Headers/globals.h"
#include "../Headers/structures.hpp"

using namespace globals;

void SaveColor(rapidjson::Value& parentTree, std::string name, ImColor& color, rapidjson::Document::AllocatorType& allocator) {
	rapidjson::Value colorValue(rapidjson::kObjectType);
	colorValue.AddMember("r", color.Value.x, allocator);
	colorValue.AddMember("g", color.Value.y, allocator);
	colorValue.AddMember("b", color.Value.z, allocator);
	colorValue.AddMember("a", color.Value.w, allocator);
	parentTree.AddMember(rapidjson::Value(name.c_str(), allocator).Move(), colorValue, allocator);
}

void LoadColor(rapidjson::Value& parentTree, std::string name, ImColor& color) {
	if (!parentTree.HasMember(name.c_str()) || !parentTree[name.c_str()].IsObject())
		return;

	rapidjson::Value& colorValue = parentTree[name.c_str()];

	if (colorValue.HasMember("r") && colorValue["r"].IsFloat())
		color.Value.x = colorValue["r"].GetFloat();
	if (colorValue.HasMember("g") && colorValue["g"].IsFloat())
		color.Value.y = colorValue["g"].GetFloat();
	if (colorValue.HasMember("b") && colorValue["b"].IsFloat())
		color.Value.z = colorValue["b"].GetFloat();
	if (colorValue.HasMember("a") && colorValue["a"].IsFloat())
		color.Value.w = colorValue["a"].GetFloat();
}

std::string Settings::StringConfigName(const std::string & configName) {
	std::string name = configName;
	if (name.rfind(".\\", 0) == 0) name = name.substr(2);
	if (name.size() >= 5 && name.compare(name.size() - 5, 5, ".json") == 0)
		name = name.substr(0, name.size() - 5);
	return name;
}

std::vector<std::string> Settings::GetSaveFiles(const std::filesystem::path directory) {
	std::vector<std::string> files;

	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		if (entry.is_regular_file() && entry.path().extension() == L".json") {
			std::wstring wideFilename = entry.path().filename().wstring();

			files.push_back(util::ConvertToUTF8String(wideFilename));
		}
	}

	return files;
}

template <typename T>
bool GetValueIfExist(const rapidjson::Value& obj, const char* key, T& out) {
	if (!obj.HasMember(key)) {
		return false;
	}

	const rapidjson::Value& val = obj[key];

	if constexpr (std::is_same<T, int>::value) {
		if (val.IsInt()) {
			out = val.GetInt();
			return true;
		}
	}
	else if constexpr (std::is_same<T, std::string>::value) {
		if (val.IsString()) {
			out = val.GetString();
			return true;
		}
	}
	else if constexpr (std::is_same<T, float>::value) {
		if (val.IsFloat()) {
			out = val.GetFloat();
			return true;
		}
	}
	else if constexpr (std::is_same<T, double>::value) {
		if (val.IsDouble()) {
			out = val.GetDouble();
			return true;
		}
	}
	else if constexpr (std::is_same<T, bool>::value) {
		if (val.IsBool()) {
			out = val.GetBool();
			return true;
		}
	}

	return false;
}

void Settings::SaveSettings(const std::string& filename) {

	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// Menu
	rapidjson::Value Menu(rapidjson::kObjectType);
	Menu.AddMember("animSpeed", Settings::Menu::animSpeed, allocator);
	Menu.AddMember("rainbowColor", Settings::Menu::rainbowColor, allocator);
	Menu.AddMember("rainbowSpeed", Settings::Menu::rainbowSpeed, allocator);
	Menu.AddMember("animSpeed", Settings::Menu::animSpeed, allocator);
	SaveColor(Menu, "color", Settings::Menu::color, allocator);
	document.AddMember("Menu", Menu, allocator);

	// Player
	rapidjson::Value Player(rapidjson::kObjectType);
	Player.AddMember("stepHandling", Settings::Player::stepHandling, allocator);
	Player.AddMember("acceleration", Settings::Player::acceleration, allocator);
	Player.AddMember("runGun", Settings::Player::runGun, allocator);
	Player.AddMember("alive", Settings::Player::alive, allocator);
	Player.AddMember("fov", Settings::Player::fov, allocator);
	document.AddMember("Player", Player, allocator);

	// Misc
	rapidjson::Value Misc(rapidjson::kObjectType);
	Misc.AddMember("enabled", Settings::Misc::enabled, allocator);
	Misc.AddMember("infinityAmmo", Settings::Misc::infinityAmmo, allocator);
	Misc.AddMember("infinityStamina", Settings::Misc::infinityStamina, allocator);
	Misc.AddMember("godMode", Settings::Misc::godMode, allocator);
	Misc.AddMember("speedHack", Settings::Misc::speedHack, allocator);
	Misc.AddMember("infinityFlyStamina", Settings::Misc::infinityFlyStamina, allocator);
	Misc.AddMember("rainbowSkinDelay", Settings::Misc::rainbowSkinDelay, allocator);
	Misc.AddMember("ammoFreezeValue", Settings::Misc::ammoFreezeValue, allocator);
	Misc.AddMember("noSpread", Settings::Misc::noSpread, allocator);
	document.AddMember("Misc", Misc, allocator);

	// FlyHack
	rapidjson::Value FlyHack(rapidjson::kObjectType);
	FlyHack.AddMember("enabled", Settings::FlyHack::enabled, allocator);
	FlyHack.AddMember("hSpeed", Settings::FlyHack::hSpeed, allocator);
	FlyHack.AddMember("vSpeed", Settings::FlyHack::vSpeed, allocator);
	FlyHack.AddMember("directional", Settings::FlyHack::directional, allocator);
	FlyHack.AddMember("noFall", Settings::FlyHack::noFall, allocator);
	FlyHack.AddMember("noPush", Settings::FlyHack::noPush, allocator);
	FlyHack.AddMember("noClip", Settings::FlyHack::noClip, allocator);
	document.AddMember("FlyHack", FlyHack, allocator);

	// FreeCam
	rapidjson::Value FreeCam(rapidjson::kObjectType);
	FreeCam.AddMember("enabled", Settings::FreeCam::enabled, allocator);
	document.AddMember("FreeCam", FreeCam, allocator);

	// Aimbot
	rapidjson::Value Aimbot(rapidjson::kObjectType);
	Aimbot.AddMember("enabled", Settings::Aimbot::enabled, allocator);
	Aimbot.AddMember("team", Settings::Aimbot::team, allocator);
	Aimbot.AddMember("enemy", Settings::Aimbot::enemy, allocator);
	Aimbot.AddMember("silent", Settings::Aimbot::silent, allocator);
	Aimbot.AddMember("triggerShoot", Settings::Aimbot::triggerShoot, allocator);
	Aimbot.AddMember("checkFOV", Settings::Aimbot::checkFOV, allocator);
	Aimbot.AddMember("drawFOV", Settings::Aimbot::drawFOV, allocator);
	Aimbot.AddMember("fov", Settings::Aimbot::fov, allocator);
	Aimbot.AddMember("maxDistance", Settings::Aimbot::maxDistance, allocator);
	Aimbot.AddMember("smooth", Settings::Aimbot::smooth, allocator);
	Aimbot.AddMember("triggerShootDelay", Settings::Aimbot::triggerShootDelay, allocator);
	document.AddMember("Aimbot", Aimbot, allocator);

	// ESP
	rapidjson::Value ESP(rapidjson::kObjectType);

	// Players settings
	rapidjson::Value Players(rapidjson::kObjectType);
	Players.AddMember("enabled", Settings::ESP::Players::enabled, allocator);
	Players.AddMember("health", Settings::ESP::Players::health, allocator);
	Players.AddMember("stamina", Settings::ESP::Players::stamina, allocator);
	Players.AddMember("inventory", Settings::ESP::Players::inventory, allocator);
	Players.AddMember("enemy", Settings::ESP::Players::enemy, allocator);
	Players.AddMember("dissidents", Settings::ESP::Players::dissidents, allocator);
	Players.AddMember("deadBody", Settings::ESP::Players::deadBody, allocator);
	Players.AddMember("player", Settings::ESP::Players::player, allocator);
	Players.AddMember("ghost", Settings::ESP::Players::ghost, allocator);
	Players.AddMember("box", Settings::ESP::Players::box, allocator);
	Players.AddMember("name", Settings::ESP::Players::name, allocator);
	Players.AddMember("distance", Settings::ESP::Players::distance, allocator);
	Players.AddMember("maxDistance", Settings::ESP::Players::maxDistance, allocator);
	Players.AddMember("state", Settings::ESP::Players::state, allocator);
	Players.AddMember("snapline", Settings::ESP::Players::snapline, allocator);
	Players.AddMember("snaplinePos", util::ImVec2ToJson(Settings::ESP::Players::snaplinePos, document.GetAllocator()), document.GetAllocator());
	ESP.AddMember("Players", Players, allocator);

	// Items settings
	rapidjson::Value Items(rapidjson::kObjectType);
	Items.AddMember("enabled", Settings::ESP::Items::enabled, allocator);

	Items.AddMember("weapons", Settings::ESP::Items::weapons, allocator);
	Items.AddMember("knife", Settings::ESP::Items::knife, allocator);
	Items.AddMember("revolver", Settings::ESP::Items::revolver, allocator);
	Items.AddMember("pistol", Settings::ESP::Items::pistol, allocator);
	Items.AddMember("shorty", Settings::ESP::Items::shorty, allocator);
	Items.AddMember("detonator", Settings::ESP::Items::detonator, allocator);
	Items.AddMember("c4", Settings::ESP::Items::c4, allocator);
	Items.AddMember("shotgun", Settings::ESP::Items::shotgun, allocator);
	Items.AddMember("rifle", Settings::ESP::Items::rifle, allocator);
	Items.AddMember("smg", Settings::ESP::Items::smg, allocator);
	Items.AddMember("defaultGun", Settings::ESP::Items::defaultGun, allocator);

	Items.AddMember("primary", Settings::ESP::Items::primary, allocator);
	Items.AddMember("gazBottle", Settings::ESP::Items::gazBottle, allocator);
	Items.AddMember("ventFilter", Settings::ESP::Items::ventFilter, allocator);
	Items.AddMember("package", Settings::ESP::Items::package, allocator);
	Items.AddMember("rice", Settings::ESP::Items::rice, allocator);
	Items.AddMember("pizzushi", Settings::ESP::Items::pizzushi, allocator);
	Items.AddMember("defibrillator", Settings::ESP::Items::defibrillator, allocator);

	Items.AddMember("secondary", Settings::ESP::Items::secondary, allocator);
	Items.AddMember("battery", Settings::ESP::Items::battery, allocator);
	Items.AddMember("cassette", Settings::ESP::Items::cassette, allocator);
	Items.AddMember("screwDriver", Settings::ESP::Items::screwDriver, allocator);
	Items.AddMember("sample", Settings::ESP::Items::sample, allocator);
	Items.AddMember("container", Settings::ESP::Items::container, allocator);
	Items.AddMember("fuse", Settings::ESP::Items::fuse, allocator);
	Items.AddMember("fish", Settings::ESP::Items::fish, allocator);
	Items.AddMember("defaultMelee", Settings::ESP::Items::defaultMelee, allocator);
	Items.AddMember("hardDrive", Settings::ESP::Items::hardDrive, allocator);
	Items.AddMember("accessCard", Settings::ESP::Items::accessCard, allocator);

	Items.AddMember("name", Settings::ESP::Items::name, allocator);
	Items.AddMember("state", Settings::ESP::Items::state, allocator);
	Items.AddMember("distance", Settings::ESP::Items::distance, allocator);
	Items.AddMember("snapline", Settings::ESP::Items::snapline, allocator);
	Items.AddMember("maxDistance", Settings::ESP::Items::maxDistance, allocator);
	ESP.AddMember("Items", Items, allocator);


	rapidjson::Value Colors(rapidjson::kObjectType);
	SaveColor(Colors, "weapons", Settings::ESP::Colors::Weapons, allocator);
	SaveColor(Colors, "primary", Settings::ESP::Colors::Primary, allocator);
	SaveColor(Colors, "secondary", Settings::ESP::Colors::Secondary, allocator);

	SaveColor(Colors, "dissident", Settings::ESP::Colors::Dissident, allocator);
	SaveColor(Colors, "employee", Settings::ESP::Colors::Employee, allocator);
	SaveColor(Colors, "dissident_ghost", Settings::ESP::Colors::DissidentGhost, allocator);
	SaveColor(Colors, "employee_ghost", Settings::ESP::Colors::EmployeeGhost, allocator);
	SaveColor(Colors, "employee_body", Settings::ESP::Colors::EmployeeBody, allocator);
	SaveColor(Colors, "dissident_body", Settings::ESP::Colors::DissidentBody, allocator);

	SaveColor(Colors, "knife", Settings::ESP::Colors::Knife, allocator);
	SaveColor(Colors, "revolver", Settings::ESP::Colors::Revolver, allocator);
	SaveColor(Colors, "pistol", Settings::ESP::Colors::Pistol, allocator);
	SaveColor(Colors, "detonator", Settings::ESP::Colors::Detonator, allocator);
	SaveColor(Colors, "c4", Settings::ESP::Colors::C4, allocator);
	SaveColor(Colors, "shorty", Settings::ESP::Colors::Shorty, allocator);
	SaveColor(Colors, "shotgun", Settings::ESP::Colors::Shotgun, allocator);
	SaveColor(Colors, "rifle", Settings::ESP::Colors::Rifle, allocator);
	SaveColor(Colors, "smg", Settings::ESP::Colors::SMG, allocator);
	SaveColor(Colors, "default_gun", Settings::ESP::Colors::DefaultGun, allocator);

	SaveColor(Colors, "gaz_bottle", Settings::ESP::Colors::GazBottle, allocator);
	SaveColor(Colors, "vent_filter", Settings::ESP::Colors::VentFilter, allocator);
	SaveColor(Colors, "rice", Settings::ESP::Colors::Rice, allocator);
	SaveColor(Colors, "package", Settings::ESP::Colors::Package, allocator);
	SaveColor(Colors, "pizzushi", Settings::ESP::Colors::Pizzushi, allocator);
	SaveColor(Colors, "defibrillator", Settings::ESP::Colors::Defibrillator, allocator);

	SaveColor(Colors, "battery", Settings::ESP::Colors::Battery, allocator);
	SaveColor(Colors, "screw_driver", Settings::ESP::Colors::ScrewDriver, allocator);
	SaveColor(Colors, "container", Settings::ESP::Colors::Container, allocator);
	SaveColor(Colors, "sample", Settings::ESP::Colors::Sample, allocator);
	SaveColor(Colors, "cassette", Settings::ESP::Colors::Cassette, allocator);
	SaveColor(Colors, "fuse", Settings::ESP::Colors::Fuse, allocator);
	SaveColor(Colors, "fish", Settings::ESP::Colors::Fish, allocator);
	SaveColor(Colors, "hard_drive", Settings::ESP::Colors::HardDrive, allocator);
	SaveColor(Colors, "default_melee", Settings::ESP::Colors::DefaultMelee, allocator);
	SaveColor(Colors, "access_card", Settings::ESP::Colors::AccessCard, allocator);
	ESP.AddMember("Colors", Colors, allocator);

	// Add ESP to document
	document.AddMember("ESP", ESP, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	writer.SetIndent(' ', 4);
	document.Accept(writer);

	std::string content = buffer.GetString();   
	std::wstring wideFilename = util::ConvertToWideString(filename);

	std::ofstream file(wideFilename, std::ios::out | std::ios::binary);
	if (file.is_open()) {
		file.write(content.c_str(), content.size());
		file.close();
	}
}

void Settings::LoadSettings(const std::string& filename) {
	std::wstring wideFilename = util::ConvertToWideString(filename);
	std::ifstream ifs(wideFilename, std::ios::binary); 
	if (!ifs.is_open())
		return;

	std::string jsonContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	rapidjson::Document document;
	if (document.Parse(jsonContent.c_str()).HasParseError())
		return;

	if (document.HasMember("Menu") && document["Menu"].IsObject()) {
		auto& menu = document["Menu"];
		GetValueIfExist(menu, "animSpeed", Settings::Menu::animSpeed);
		GetValueIfExist(menu, "rainbowColor", Settings::Menu::rainbowColor);
		GetValueIfExist(menu, "rainbowSpeed", Settings::Menu::rainbowSpeed);
		LoadColor(menu, "color", Settings::Menu::color);
	}

	if (document.HasMember("Player") && document["Player"].IsObject()) {
		const auto& player = document["Player"];
		GetValueIfExist(player, "stepHandling", Settings::Player::stepHandling);
		GetValueIfExist(player, "acceleration", Settings::Player::acceleration);
		GetValueIfExist(player, "runGun", Settings::Player::runGun);
		GetValueIfExist(player, "alive", Settings::Player::alive);
		GetValueIfExist(player, "fov", Settings::Player::fov);
	}

	if (document.HasMember("Misc") && document["Misc"].IsObject()) {
		const auto& misc = document["Misc"];
		GetValueIfExist(misc, "enabled", Settings::Misc::enabled);
		GetValueIfExist(misc, "infinityAmmo", Settings::Misc::infinityAmmo);
		GetValueIfExist(misc, "infinityStamina", Settings::Misc::infinityStamina);
		GetValueIfExist(misc, "godMode", Settings::Misc::godMode);
		GetValueIfExist(misc, "speedHack", Settings::Misc::speedHack);
		GetValueIfExist(misc, "silentGun", Settings::Misc::silentGun);
		GetValueIfExist(misc, "infinityFlyStamina", Settings::Misc::infinityFlyStamina);
		GetValueIfExist(misc, "rainbowSkinDelay", Settings::Misc::rainbowSkinDelay);
		GetValueIfExist(misc, "ammoFreezeValue", Settings::Misc::ammoFreezeValue);
		GetValueIfExist(misc, "noSpread", Settings::Misc::noSpread);
	}

	if (document.HasMember("FreeCam") && document["FreeCam"].IsObject()) {
		const auto& freeCam = document["FlyHack"];
		GetValueIfExist(freeCam, "enabled", Settings::FreeCam::enabled);
	}
	if (document.HasMember("FlyHack") && document["FlyHack"].IsObject()) {
		const auto& flyHack = document["FlyHack"];
		GetValueIfExist(flyHack, "enabled", Settings::FlyHack::enabled);
		GetValueIfExist(flyHack, "hSpeed", Settings::FlyHack::hSpeed);
		GetValueIfExist(flyHack, "vSpeed", Settings::FlyHack::vSpeed);
		GetValueIfExist(flyHack, "directional", Settings::FlyHack::directional);
		GetValueIfExist(flyHack, "noFall", Settings::FlyHack::noFall);
		GetValueIfExist(flyHack, "noPush", Settings::FlyHack::noPush);
		GetValueIfExist(flyHack, "noClip", Settings::FlyHack::noClip);
	}

	if (document.HasMember("Aimbot") && document["Aimbot"].IsObject()) {
		const auto& aimbot = document["Aimbot"];
		GetValueIfExist(aimbot, "enabled", Settings::Aimbot::enabled);
		GetValueIfExist(aimbot, "team", Settings::Aimbot::team);
		GetValueIfExist(aimbot, "enemy", Settings::Aimbot::enemy);
		GetValueIfExist(aimbot, "silent", Settings::Aimbot::silent);
		GetValueIfExist(aimbot, "triggerShoot", Settings::Aimbot::triggerShoot);
		GetValueIfExist(aimbot, "checkFOV", Settings::Aimbot::checkFOV);
		GetValueIfExist(aimbot, "drawFOV", Settings::Aimbot::drawFOV);
		GetValueIfExist(aimbot, "fov", Settings::Aimbot::fov);
		GetValueIfExist(aimbot, "maxDistance", Settings::Aimbot::maxDistance);
		GetValueIfExist(aimbot, "smooth", Settings::Aimbot::smooth);
	}

	if (document.HasMember("ESP") && document["ESP"].IsObject()) {
		auto& esp = document["ESP"];

		if (esp.HasMember("Players") && esp["Players"].IsObject()) {
			const auto& players = esp["Players"];
			GetValueIfExist(players, "enabled", Settings::ESP::Players::enabled);
			GetValueIfExist(players, "health", Settings::ESP::Players::health);
			GetValueIfExist(players, "stamina", Settings::ESP::Players::stamina);
			GetValueIfExist(players, "inventory", Settings::ESP::Players::inventory);
			GetValueIfExist(players, "enemy", Settings::ESP::Players::enemy);
			GetValueIfExist(players, "dissidents", Settings::ESP::Players::dissidents);
			GetValueIfExist(players, "deadBody", Settings::ESP::Players::deadBody);
			GetValueIfExist(players, "player", Settings::ESP::Players::player);
			GetValueIfExist(players, "ghost", Settings::ESP::Players::ghost);
			GetValueIfExist(players, "box", Settings::ESP::Players::box);
			GetValueIfExist(players, "name", Settings::ESP::Players::name);
			GetValueIfExist(players, "distance", Settings::ESP::Players::distance);
			GetValueIfExist(players, "maxDistance", Settings::ESP::Players::maxDistance);
			GetValueIfExist(players, "state", Settings::ESP::Players::state);
			GetValueIfExist(players, "snapline", Settings::ESP::Players::snapline);

			if (players.HasMember("snaplinePos") && players["snaplinePos"].IsObject()) {
				Settings::ESP::Players::snaplinePos = util::JsonToImVec2(players["snaplinePos"]);
			}
		}

		// Items Section
		if (esp.HasMember("Items") && esp["Items"].IsObject()) {
			const auto& items = esp["Items"];
			GetValueIfExist(items, "enabled", Settings::ESP::Items::enabled);

			GetValueIfExist(items, "weapons", Settings::ESP::Items::weapons);
			GetValueIfExist(items, "knife", Settings::ESP::Items::knife);
			GetValueIfExist(items, "revolver", Settings::ESP::Items::revolver);
			GetValueIfExist(items, "pistol", Settings::ESP::Items::pistol);
			GetValueIfExist(items, "shorty", Settings::ESP::Items::shorty);
			GetValueIfExist(items, "detonator", Settings::ESP::Items::detonator);
			GetValueIfExist(items, "c4", Settings::ESP::Items::c4);
			GetValueIfExist(items, "shotgun", Settings::ESP::Items::shotgun);
			GetValueIfExist(items, "rifle", Settings::ESP::Items::rifle);
			GetValueIfExist(items, "smg", Settings::ESP::Items::smg);
			GetValueIfExist(items, "defaultGun", Settings::ESP::Items::defaultGun);

			GetValueIfExist(items, "primary", Settings::ESP::Items::primary);
			GetValueIfExist(items, "gazBottle", Settings::ESP::Items::gazBottle);
			GetValueIfExist(items, "ventFilter", Settings::ESP::Items::ventFilter);
			GetValueIfExist(items, "package", Settings::ESP::Items::package);
			GetValueIfExist(items, "rice", Settings::ESP::Items::rice);
			GetValueIfExist(items, "pizzushi", Settings::ESP::Items::pizzushi);
			GetValueIfExist(items, "defibrillator", Settings::ESP::Items::defibrillator);

			GetValueIfExist(items, "secondary", Settings::ESP::Items::secondary);
			GetValueIfExist(items, "battery", Settings::ESP::Items::battery);
			GetValueIfExist(items, "cassette", Settings::ESP::Items::cassette);
			GetValueIfExist(items, "screwDriver", Settings::ESP::Items::screwDriver);
			GetValueIfExist(items, "container", Settings::ESP::Items::container);
			GetValueIfExist(items, "sample", Settings::ESP::Items::sample);
			GetValueIfExist(items, "fuse", Settings::ESP::Items::fuse);
			GetValueIfExist(items, "fish", Settings::ESP::Items::fish);
			GetValueIfExist(items, "defaultMelee", Settings::ESP::Items::defaultMelee);
			GetValueIfExist(items, "hardDrive", Settings::ESP::Items::hardDrive);
			GetValueIfExist(items, "accessCard", Settings::ESP::Items::accessCard);

			GetValueIfExist(items, "name", Settings::ESP::Items::name);
			GetValueIfExist(items, "state", Settings::ESP::Items::state);
			GetValueIfExist(items, "distance", Settings::ESP::Items::distance);
			GetValueIfExist(items, "snapline", Settings::ESP::Items::snapline);
			GetValueIfExist(items, "maxDistance", Settings::ESP::Items::maxDistance);
		}

		if (esp.HasMember("Colors") && esp["Colors"].IsObject()) {
			auto& colors = esp["Colors"];
			LoadColor(colors, "weapons", Settings::ESP::Colors::Weapons);
			LoadColor(colors, "primary", Settings::ESP::Colors::Primary);
			LoadColor(colors, "secondary", Settings::ESP::Colors::Secondary);

			LoadColor(colors, "dissident", Settings::ESP::Colors::Dissident);
			LoadColor(colors, "employee", Settings::ESP::Colors::Employee);
			LoadColor(colors, "dissident_ghost", Settings::ESP::Colors::DissidentGhost);
			LoadColor(colors, "employee_ghost", Settings::ESP::Colors::EmployeeGhost);
			LoadColor(colors, "employee_body", Settings::ESP::Colors::EmployeeBody);
			LoadColor(colors, "dissident_body", Settings::ESP::Colors::DissidentBody);

			LoadColor(colors, "knife", Settings::ESP::Colors::Knife);
			LoadColor(colors, "revolver", Settings::ESP::Colors::Revolver);
			LoadColor(colors, "pistol", Settings::ESP::Colors::Pistol);
			LoadColor(colors, "detonator", Settings::ESP::Colors::Detonator);
			LoadColor(colors, "c4", Settings::ESP::Colors::C4);
			LoadColor(colors, "shorty", Settings::ESP::Colors::Shorty);
			LoadColor(colors, "shotgun", Settings::ESP::Colors::Shotgun);
			LoadColor(colors, "rifle", Settings::ESP::Colors::Rifle);
			LoadColor(colors, "smg", Settings::ESP::Colors::SMG);
			LoadColor(colors, "default_gun", Settings::ESP::Colors::DefaultGun);

			LoadColor(colors, "gaz_bottle", Settings::ESP::Colors::GazBottle);
			LoadColor(colors, "vent_filter", Settings::ESP::Colors::VentFilter);
			LoadColor(colors, "rice", Settings::ESP::Colors::Rice);
			LoadColor(colors, "package", Settings::ESP::Colors::Package);
			LoadColor(colors, "pizzushi", Settings::ESP::Colors::Pizzushi);
			LoadColor(colors, "defibrillator", Settings::ESP::Colors::Defibrillator);


			LoadColor(colors, "battery", Settings::ESP::Colors::Battery);
			LoadColor(colors, "screw_driver", Settings::ESP::Colors::ScrewDriver);
			LoadColor(colors, "container", Settings::ESP::Colors::Container);
			LoadColor(colors, "sample", Settings::ESP::Colors::Sample);
			LoadColor(colors, "cassette", Settings::ESP::Colors::Cassette);
			LoadColor(colors, "fuse", Settings::ESP::Colors::Fuse);
			LoadColor(colors, "fish", Settings::ESP::Colors::Fish);
			LoadColor(colors, "hard_drive", Settings::ESP::Colors::HardDrive);
			LoadColor(colors, "default_melee", Settings::ESP::Colors::DefaultMelee);
			LoadColor(colors, "access_card", Settings::ESP::Colors::AccessCard);
		}
	}
}

void Settings::SaveHotkeys() {
	const std::string filename = "hotkeys.json";

	char fullPath[MAX_PATH];
	if (_fullpath(fullPath, filename.c_str(), MAX_PATH)) {
		std::cout << "Saving hotkeys to file: " + std::string(fullPath) << std::endl;
	}

	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	rapidjson::Value hotkeysObject(rapidjson::kObjectType);

	for (const auto& [label, hotkey] : Hotkeys::HotkeysMap) {
		if (hotkey.key == 0) continue;

		rapidjson::Value hotkeyObject(rapidjson::kObjectType);
		hotkeyObject.AddMember("key", hotkey.key, allocator);


		std::string typeStr;
		switch (hotkey.type) {
		case WidgetType::Checkbox: typeStr = "Checkbox"; break;
		case WidgetType::Slider: typeStr = "Slider"; break;
		case WidgetType::Button: typeStr = "Button"; break;
		default: typeStr = "None"; break;
		}

		hotkeyObject.AddMember("type", rapidjson::Value(typeStr.c_str(), allocator), allocator);

		std::string modeStr;
		switch (hotkey.mode) {
		case HotkeyMode::Toggle: modeStr = "Toggle"; break;
		case HotkeyMode::Hold: modeStr = "Hold"; break;
		case HotkeyMode::Press: modeStr = "Press"; break;
		default: modeStr = "None"; break;
		}

		hotkeyObject.AddMember("mode", rapidjson::Value(modeStr.c_str(), allocator), allocator);

		hotkeysObject.AddMember(rapidjson::Value(label.c_str(), allocator), hotkeyObject, allocator);
	}

	document.AddMember("Hotkeys", hotkeysObject, allocator);

	rapidjson::Value otherHotkeyObject(rapidjson::kObjectType);
	otherHotkeyObject.AddMember("hostTpHotkey", Settings::Misc::hostTpHotkey, allocator);
	otherHotkeyObject.AddMember("aimbotHotkey", Settings::Aimbot::hotkey, allocator);
	document.AddMember("OtherHotkeys", otherHotkeyObject, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	writer.SetIndent(' ', 4);
	document.Accept(writer);

	std::string content = buffer.GetString();
	std::wstring wideFilename = util::ConvertToWideString(filename);

	std::ofstream file(wideFilename, std::ios::out | std::ios::binary);
	if (file.is_open()) {
		file.write(content.c_str(), content.size());
		file.close();
	}
}

void Settings::LoadHotkeys() {
	const std::string filename = "hotkeys.json";

	char fullPath[MAX_PATH];
	if (_fullpath(fullPath, filename.c_str(), MAX_PATH)) {
		std::cout << "Loading hotkeys from file: " + std::string(fullPath) << std::endl;
	}

	std::ifstream ifs(filename);
	if (!ifs.is_open())
		return;

	std::string jsonContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	rapidjson::Document document;
	if (document.Parse(jsonContent.c_str()).HasParseError())
		return;

	if (!document.HasMember("Hotkeys") || !document["Hotkeys"].IsObject())
		return;

	auto& hotkeys = document["Hotkeys"];
	for (auto it = hotkeys.MemberBegin(); it != hotkeys.MemberEnd(); ++it) {
		const std::string label = it->name.GetString();
		const rapidjson::Value& hotkeyObject = it->value;

		int key;
		std::string modeStr;
		std::string typeStr;

		if (GetValueIfExist(hotkeyObject, "key", key) && GetValueIfExist(hotkeyObject, "mode", modeStr) && GetValueIfExist(hotkeyObject, "type", typeStr)) {
			HotkeyMode mode = (modeStr == "Toggle") ? HotkeyMode::Toggle : (modeStr == "Hold") ? HotkeyMode::Hold : (modeStr == "Press") ? HotkeyMode::Press : HotkeyMode::None;
			WidgetType type = (typeStr == "Checkbox") ? WidgetType::Checkbox : (typeStr == "Slider") ? WidgetType::Slider : (typeStr == "Button") ? WidgetType::Button : WidgetType::None;

			Hotkeys::HotkeysMap[label] = Hotkey{ key, mode, type };
		}
	}

	if (document.HasMember("OtherHotkeys") && document["OtherHotkeys"].IsObject()) {
		auto& otherHotkeys = document["OtherHotkeys"];

		if (otherHotkeys.HasMember("hostTpHotkey") && otherHotkeys["hostTpHotkey"].IsInt()) {
			Settings::Misc::hostTpHotkey = otherHotkeys["hostTpHotkey"].GetInt();
		}

		if (otherHotkeys.HasMember("aimbotHotkey") && otherHotkeys["aimbotHotkey"].IsInt()) {
			Settings::Aimbot::hotkey = otherHotkeys["aimbotHotkey"].GetInt();
		}
	}
}

void Settings::SaveTeleports() {
	const std::string filename = "teleports.json";

	rapidjson::Document document;
	std::ifstream file(filename, std::ios::in | std::ios::binary);

	if (file.is_open()) {
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		document.Parse(content.c_str());
		file.close();
	}

	if (document.IsNull()) {
		document.SetObject();
	}

	if (!document.HasMember("Teleports") || !document["Teleports"].IsArray()) {
		document.RemoveAllMembers();
		document.AddMember("Teleports", rapidjson::kArrayType, document.GetAllocator());
	}

	document["Teleports"].Clear();

	for (const auto& teleport : Settings::Teleports::teleports_cache) {
		rapidjson::Value teleportObject(rapidjson::kObjectType);
		teleportObject.AddMember("name", rapidjson::Value(teleport.name.c_str(), document.GetAllocator()), document.GetAllocator());
		teleportObject.AddMember("location", util::Vector3ToJson(teleport.location, document.GetAllocator()), document.GetAllocator());
		teleportObject.AddMember("angle", util::Vector2ToJson(teleport.angle, document.GetAllocator()), document.GetAllocator());
		teleportObject.AddMember("hotkey", teleport.hotkey, document.GetAllocator());

		document["Teleports"].PushBack(teleportObject, document.GetAllocator());
	}

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	writer.SetIndent(' ', 4);
	document.Accept(writer);

	std::ofstream outFile(filename, std::ios::out | std::ios::binary);
	if (outFile.is_open()) {
		outFile.write(buffer.GetString(), buffer.GetSize());
		outFile.close();
	}
}

void Settings::LoadTeleports() {
	const std::string filename = "teleports.json";

	std::ifstream ifs(filename);
	if (!ifs.is_open())
		return;

	std::string jsonContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	ifs.close();

	rapidjson::Document document;

	Settings::Teleports::teleports_cache.clear();

	if (document.Parse(jsonContent.c_str()).HasParseError())
		return;
	if (document.HasMember("Teleports") && document["Teleports"].IsArray()) {
		for (const auto& item : document["Teleports"].GetArray()) {
			Teleport teleport{};
			teleport.name = item["name"].GetString();
			teleport.location = util::JsonToVector3(item["location"]);
			teleport.angle = util::JsonToVector2(item["angle"]);
			teleport.hotkey = item["hotkey"].GetInt();

			Settings::Teleports::teleports_cache.push_back(teleport);
		}
	}
}

void Settings::SaveWeaponConfig(const std::string& filename, const ItemProperties& itemProperties) {
	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	rapidjson::Value Stats(rapidjson::kObjectType);
	document.AddMember("Stats", util::ItemPropertiesToJson(itemProperties, document.GetAllocator()), document.GetAllocator());

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	writer.SetIndent(' ', 4);
	document.Accept(writer);

	std::string content = buffer.GetString();
	std::wstring wideFilename = util::ConvertToWideString(filename);

	std::ofstream file(wideFilename, std::ios::out | std::ios::binary);
	if (file.is_open()) {
		file.write(content.c_str(), content.size());
		file.close();
	}
}

void Settings::LoadWeaponConfig(const std::string& filename) {
	std::wstring wideFilename = util::ConvertToWideString(filename);
	std::ifstream ifs(wideFilename, std::ios::binary);
	if (!ifs.is_open())
		return;

	std::string jsonContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	rapidjson::Document document;
	if (document.Parse(jsonContent.c_str()).HasParseError())
		return;

	auto item_stats = util::JsonToItemProperties(document["Stats"]);

	auto item = unique_item_data[item_stats.name];

	if (item) {
		auto gun_data = (u_data_gun*)item;
		auto melee_data = (u_data_melee*)item;

		if (item_stats.is_melee) {
			auto melee_type = melee_data->get_melee_type();

			melee_type->set_heal(item_stats.frontal_damage_health);
			melee_type->set_stamina(item_stats.frontal_damage_stamina);
			melee_type->set_stun(item_stats.frontal_damage_slow);

			melee_type->set_crit_heal(item_stats.back_damage_health);
			melee_type->set_crit_stamina(item_stats.back_damage_stamina);
			melee_type->set_crit_stun(item_stats.back_damage_slow);

			melee_type->set_range(item_stats.attack_range);
			melee_type->set_recover_time(item_stats.attack_speed);
			melee_type->set_cost(item_stats.stamina_cost);
			melee_type->set_cast_time(item_stats.cast_time);

			gun_data->set_can_inventory(item_stats.can_inventory);
		}
		else {
			gun_data->set_damage(item_stats.body_damage_health);
			gun_data->set_stamina_damage(item_stats.body_damage_stamina);
			gun_data->set_stun(item_stats.body_damage_slow);

			gun_data->set_crit(item_stats.head_damage_health);
			gun_data->set_crit_stamina(item_stats.head_damage_stamina);

			gun_data->set_capacity(item_stats.max_ammo);
			gun_data->set_fire_rate(item_stats.fire_rate);
			gun_data->set_stamina_usage(item_stats.stamina_cost);

			gun_data->set_impact_type(item_stats.impact_type);
			gun_data->set_pattern_type(item_stats.pattern_type);

			gun_data->set_auto_fire(item_stats.auto_fire);
			gun_data->set_can_inventory(item_stats.can_inventory);
		}
	}
}