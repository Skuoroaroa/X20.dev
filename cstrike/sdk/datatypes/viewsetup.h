#pragma once

// used: mem_pad
#include "../../utilities/memory.h"

// used: vector_t
#include "vector.h"
// used: qangle_t
#include "qangle.h"

class CViewSetup
{
public:
	MEM_PAD(0x450);
	float flOrthoLeft; // 0x0450
	float flOrthoTop; // 0x0454
	float flOrthoRight; // 0x0458
	float flOrthoBottom; // 0x045C
	MEM_PAD(0x38);
	float flFov; // 0x0498
	float flFovViewmodel; // 0x049C
	Vector_t vecOrigin; // 0x04A0
	MEM_PAD(0xC); // 0x04AC
	QAngle_t angView; // 0x04B8
	MEM_PAD(0x14); // 0x04C4
	float flAspectRatio; // 0x04D8
};