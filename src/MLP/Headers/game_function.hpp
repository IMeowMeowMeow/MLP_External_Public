#pragma once
#ifndef GAME_FUNCTIONS_HPP
#define GAME_FUNCTIONS_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include "game_structures.hpp"
#include "util.hpp"
#include "../Memory/memory.hpp"
#include "globals.h"

using namespace protocol::engine::sdk;
using namespace protocol::game::sdk;
using namespace protocol::engine;

std::vector<uintptr_t> find_objects(const std::string& name_find);

u_data_item* AssignToItemData(const std::string& target_name);

inline u_function* find_function_by_name(const std::string& function_name) {
	constexpr auto elements_per_chunk = 64 * 1024;
	auto gobjects = memory::read<fuobjectarray>(memory::module_base + protocol::engine::GOBJECTS);

	for (int i = 0; i < gobjects.num_chunk; ++i) {
		auto chunk_start = memory::read<uintptr_t>(gobjects.chunk_table + (i * 0x8));
		for (int j = 0; j < elements_per_chunk; ++j) {
			auto item = memory::read<fuobjectitem>(chunk_start + (j * sizeof(fuobjectitem)));
			if (item.object == 0) continue;

			auto obj = reinterpret_cast<u_object*>(item.object);
			std::string obj_name = util::get_name_from_fname(obj->fname_index());

			// Check if the name matches
			if (obj_name == function_name) {
				// Ensure it's a function (use class name or other checks)
				auto obj_class_name = util::get_name_from_fname(obj->class_private()->fname_index());
				if (obj_class_name == "Function") {
					std::cout << "Found function '" << obj_name << "' at Address: " << std::hex << item.object << std::endl;
					return static_cast<u_function*>(obj);
				}
			}
		}
	}
	std::cout << "Function '" << function_name << "' not found." << std::endl;
	return nullptr;
}

// Find a field by name in a UStruct
u_field* find_field_by_name(u_struct* obj_struct, const std::string& field_name);

// Trigger a UFunction on a target object
inline void trigger_function(const std::string& function_name, u_object* target, void* params) {
	if (!target) return;

	// Find the UFunction by name
	u_function* func = find_function_by_name(function_name);
	if (!func) return;

	// Call ProcessEvent
	globals::process_event(target, func, params);
}

// Check if an object matches a class name
/*
inline bool is_a(uintptr_t object_address, const std::string& class_name, bool exact_match = false) {
	if (!object_address) return false;

	auto object = reinterpret_cast<u_object*>(object_address);
	auto object_class = object->class_private();

	while (object_class) {
		int fname_idx = object_class->fname_index();
		std::string current_class_name = util::get_name_from_fname(fname_idx);

		if (exact_match) {
			if (current_class_name == class_name) return true;
		}
		else {
			if (current_class_name.find(class_name) != std::string::npos) return true;
		}

		object_class = object_class->super();
	}

	return false;
}
*/

// Function to capitalize strings
inline std::string ToUpperCase(const std::string& str) {
	std::string upper_case = str;
	std::transform(upper_case.begin(), upper_case.end(), upper_case.begin(), ::toupper);
	return upper_case;
}

// Find objects by name
inline std::vector<uintptr_t> find_objects(const std::string& name_find) {
	std::vector<uintptr_t> objs;
	constexpr auto elements_per_chunk = 64 * 1024;
	auto gobjects = memory::read<fuobjectarray>(memory::module_base + GOBJECTS);

	// Preallocate memory to reduce dynamic resizing
	objs.reserve(1024);

	for (int i = 0; i < gobjects.num_chunk; i++) {
		auto chunk_start = memory::read<uintptr_t>(gobjects.chunk_table + (i * sizeof(uintptr_t)));

		for (int j = 0; j < elements_per_chunk; j++) {
			uintptr_t item_address = chunk_start + (j * sizeof(fuobjectitem));
			auto item = memory::read<fuobjectitem>(item_address);
			if (!item.object) continue;

			auto obj = reinterpret_cast<u_object*>(item.object);

			// Read names only if necessary
			std::string_view name = util::get_name_from_fname(obj->fname_index());
			if (name.find(name_find) != std::string_view::npos) {
				objs.emplace_back(item.object);
			}
		}
	}
	return objs;
}


inline uintptr_t* find_object(const std::string& target_name) {
	std::vector<uintptr_t> objects = find_objects(target_name);

	uintptr_t selected_object = 0;

	for (uintptr_t obj_addr : objects) {
		auto obj = reinterpret_cast<u_object*>(obj_addr);
		std::string obj_name = util::get_name_from_fname(obj->fname_index());
		std::string class_name = util::get_name_from_fname(obj->class_private()->fname_index());

		if (obj_name == target_name || class_name == target_name) {
			std::cout << "[LOG] Found Object: " << obj_name << " | Class: " << class_name
				<< " | Address: " << std::hex << obj_addr << std::endl;

			selected_object = obj_addr; 
		}
	}

	if (selected_object != 0) {
		std::cout << "Selected Object Address: " << std::hex << selected_object << std::endl;
		return reinterpret_cast<uintptr_t*>(selected_object);
	}

	std::cout << "Object not found!" << std::endl;
	return nullptr;
}

inline u_data_item* AssignToItemData(const std::string& target_name) {
	std::string upper_name = ToUpperCase(target_name);

	std::vector<uintptr_t> found_objects = find_objects(target_name);

	if (found_objects.empty()) {
		//std::cout << "No objects found for: " << target_name << std::endl;
		return nullptr;
	}

	uintptr_t selected_object = 0;
	for (uintptr_t obj_addr : found_objects) {
		auto obj = reinterpret_cast<u_object*>(obj_addr);
		std::string obj_name = util::get_name_from_fname(obj->fname_index());
		std::string class_name = util::get_name_from_fname(obj->class_private()->fname_index());

		//std::cout << "Checking Object: " << obj_name << " | Class: " << class_name
			//<< " | Address: " << std::hex << obj_addr << std::endl;

		if (obj_name.find(target_name) != std::string::npos) {
			if (obj->is_a("Data_Gun_C") || obj->is_a("Data_Melee_C") || obj->is_a("Data_Consumable_C") || obj->is_a("Data_Consumable_C")) {
				selected_object = obj_addr;
				break;
			}
		}
	}

	if (selected_object != 0) {
		//std::cout << "Selected Object Address: " << std::hex << selected_object << std::endl;
		return reinterpret_cast<u_data_item*>(selected_object);
	}

	//std::cout << "No valid object found for: " << target_name << std::endl;
	return nullptr;
}


// Request_TP function definition
struct FRequestTPParams {
	FVector Location;
	float Orientation_0;
};


inline void request_tp(mec_pawn* pawn, FVector target_location, float orientation) {
	if (!pawn) {
		std::cerr << "Error: Pawn is null!" << std::endl;
		return;
	}

	u_function* request_tp_function = find_function_by_name("Request TP");
	if (!request_tp_function) {
		std::cerr << "Error: 'Request TP' function not found!" << std::endl;
		return;
	}

	struct FRequestTPParams {
		FVector Location;
		float Orientation_0;
	};

	FRequestTPParams params = { target_location, orientation };

	// Debugging outputs
	std::cout << "Request TP Function Class: "
		<< util::get_name_from_fname(request_tp_function->class_private()->fname_index())
		<< std::endl;
	std::cout << "Request TP Function Address: " << std::hex << request_tp_function << std::endl;
	std::cout << "Pawn Address: " << std::hex << pawn << std::endl;
	std::cout << "Params Location: ("
		<< params.Location.X << ", "
		<< params.Location.Y << ", "
		<< params.Location.Z << "), Orientation: "
		<< params.Orientation_0 << std::endl;

	try {
		globals::process_event(pawn, request_tp_function, &params);
		std::cout << "'ProcessEvent' call completed successfully." << std::endl;
	}
	catch (...) {
		std::cerr << "Error: Exception occurred during 'ProcessEvent' call!" << std::endl;
	}
}

inline void list_mec_functions(const std::string& filename = "mec_functions_output.txt") {
	constexpr auto elements_per_chunk = 64 * 1024; // Assuming chunk size
	auto gobjects = memory::read<fuobjectarray>(memory::module_base + protocol::engine::GOBJECTS);

	// Open a file for writing
	std::ofstream output_file(filename);
	if (!output_file.is_open()) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return;
	}

	// Iterate through GObjects
	for (int i = 0; i < gobjects.num_chunk; ++i) {
		auto chunk_start = memory::read<uintptr_t>(gobjects.chunk_table + (i * 0x8));
		for (int j = 0; j < elements_per_chunk; ++j) {
			auto item = memory::read<fuobjectitem>(chunk_start + (j * sizeof(fuobjectitem)));
			if (item.object == 0) continue;

			auto obj = reinterpret_cast<u_object*>(item.object);
			auto fname_idx = obj->fname_index();

			// Check for valid names
			if (fname_idx <= 0) continue;

			std::string obj_name = util::get_name_from_fname(fname_idx);
			std::string obj_class_name = util::get_name_from_fname(obj->class_private()->fname_index());

			// Write to the file
			output_file << "Object Name: " << obj_name << ", Class: " << obj_class_name << std::endl;

			if (obj_class_name == "Mec_C" && obj->class_private()->is_a("Function")) {
				output_file << "[Function] Name: " << obj_name << ", Class: " << obj_class_name << std::endl;
			}
		}
	}

	// Close the file
	output_file.close();
	std::cout << "Mec functions written to " << filename << std::endl;
}


#endif