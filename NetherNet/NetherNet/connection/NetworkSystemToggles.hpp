#pragma once
#include <cstdint>

struct NetworkSystemToggles {
public:
	uint64_t unk1;  //this + 0x0
	uint64_t unk2;	//this + 0x10
	uint32_t unk3;	//this + 0x20

	NetworkSystemToggles(const NetworkSystemToggles& other) noexcept :
		unk1(other.unk1), unk2(other.unk2), unk3(other.unk3) {}

	NetworkSystemToggles(NetworkSystemToggles&& other) noexcept :
		unk1(other.unk1), unk2(other.unk2), unk3(other.unk3) {}
};