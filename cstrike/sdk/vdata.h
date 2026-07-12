#pragma once

// used: schema field
#include "../core/schema.h"
// used: rop
#include "../utilities/memory.h"

using CFiringModeFloat = std::float_t[2];
using CSkillFloat = std::float_t[4];

class CBasePlayerVData
{
public:
	CS_CLASS_NO_INITIALIZER(CBasePlayerVData);
};

class CBasePlayerWeaponVData
{
public:
	CS_CLASS_NO_INITIALIZER(CBasePlayerWeaponVData);

	SCHEMA_ADD_FIELD(bool, IsFullAuto, "CBasePlayerWeaponVData", "m_bIsFullAuto");
	SCHEMA_ADD_FIELD(std::int32_t, GetMaxClip1, "CBasePlayerWeaponVData", "m_iMaxClip1");
};

