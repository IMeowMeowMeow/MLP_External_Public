#pragma once
#include "../Memory/memory.hpp"	
#include "enums.h"

#define pthis (uintptr_t)this
// Does not add get_##name to the front and uses the direct name
#define GET_OFFSET_NAMED(offset, name, type) type name() { return memory::read<type>((uintptr_t)this + offset); }
// Does not add set_##name to the front and uses the direct name
#define SET_OFFSET_NAMED(offset, name, type) void name(type value) { memory::write<type>((uintptr_t)this + offset, value); }
// Adds get_##name to the front
#define GET_OFFSET(offset, name, type) GET_OFFSET_NAMED(offset, get_##name, type)
// Adds set_##name to the front
#define SET_OFFSET(offset, name, type) SET_OFFSET_NAMED(offset, set_##name, type)
#define OFFSET(offset, name, type) GET_OFFSET(offset, name, type) SET_OFFSET(offset, name, type)


#define GET_ENUM_OFFSET_NAMED(offset, name, type) type name() { return static_cast<type>(memory::read<int>((uintptr_t)this + offset)); }
#define SET_ENUM_OFFSET_NAMED(offset, name, type) void name(type value) { memory::write<int>((uintptr_t)this + offset, static_cast<int>(value)); }

#define GET_ENUM_OFFSET(offset, name, type) GET_ENUM_OFFSET_NAMED(offset, get_##name, type)
#define SET_ENUM_OFFSET(offset, name, type) SET_ENUM_OFFSET_NAMED(offset, set_##name, type)

#define ENUM_OFFSET(offset, name, type) GET_ENUM_OFFSET(offset, name, type) SET_ENUM_OFFSET(offset, name, type)