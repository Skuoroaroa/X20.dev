#pragma once


#include "../../core/sdk.h"
#include "../../sdk/entity.h"

#define MULTIPLAYER_BACKUP 150

class CTinyMoveStepData
{
public:
	float flWhen; //0x0000
	MEM_PAD(0x4); //0x0004
	std::uint64_t nButton; //0x0008
	bool bPressed; //0x0010
	MEM_PAD(0x7); //0x0011
}; //Size: 0x0018
class CCSGOInterpolationInfoPB_CL : public CBasePB {
public:
	float m_flFraction; // 0x18
};
static_assert(sizeof(CCSGOInterpolationInfoPB_CL) == 0x20);
class CMoveStepButtons
{
public:
	std::uint64_t nKeyboardPressed; //0x0000
	std::uint64_t nMouseWheelheelPressed; //0x0008
	std::uint64_t nUnPressed; //0x0010
	std::uint64_t nKeyboardCopy; //0x0018
}; //Size: 0x0020

// @credits: www.unknowncheats.me/forum/members/2943409.html
class CExtendedMoveData : public CMoveStepButtons
{
public:
	float flForwardMove; //0x0020
	float flSideMove; //0x0024
	float flUpMove; //0x0028
	std::int32_t nMouseDeltaX; //0x002C
	std::int32_t nMouseDeltaY; //0x0030
	std::int32_t nAdditionalStepMovesCount; //0x0034
	CTinyMoveStepData tinyMoveStepData[12]; //0x0038
	Vector_t vecViewAngle; //0x0158
	std::int32_t nTargetHandle; //0x0164
}; //Size:0x0168

class CCSGOInput
{
public:
	MEM_PAD(0x250);
	CUserCmd arrCommands[MULTIPLAYER_BACKUP];
	MEM_PAD(0x99)
	bool bInThirdPerson;
	MEM_PAD(0x6);
	QAngle_t angThirdPersonAngles;
	MEM_PAD(0xE);
	std::int32_t nSequenceNumber;
	double dbSomeTimer;
	CExtendedMoveData currentMoveData;
	std::int32_t nWeaponSwitchTick;
	MEM_PAD(0x1C4);
	CExtendedMoveData* pExtendedMoveData;
	MEM_PAD(0x48);
	
	int32_t nAttackStartHistoryIndex1;
	int32_t nAttackStartHistoryIndex2;
	int32_t nAttackStartHistoryIndex3;

	CUserCmd* GetUserCmd()
	{
		return &arrCommands[nSequenceNumber % MULTIPLAYER_BACKUP];
	}


	void SetViewAngle(const Vector_t& angView)
	{
		using SetViewAngles_t =
			void(__fastcall*)(void*, int32_t, const Vector_t&);

		static auto fnSetViewAngles =
			reinterpret_cast<SetViewAngles_t>(
				MEM::FindPattern(
					CLIENT_DLL,
					CS_XOR("85 D2 75 ? 48 63 81")
				)
				);

		if (!fnSetViewAngles)
			return;

		fnSetViewAngles(this, 0, angView);
	}
QAngle_t GetViewAngles()
	{
		using fnGetViewAngles = std::int64_t(CS_FASTCALL*)(CCSGOInput*, std::int32_t);
		static auto oGetViewAngles = reinterpret_cast<fnGetViewAngles>(MEM::FindPattern(CLIENT_DLL, CS_XOR("4D 85 C0 74 ? 85 D2 74")));

		#ifdef CS_PARANOID
		CS_ASSERT(oGetViewAngles != nullptr);
		#endif

		return *reinterpret_cast<QAngle_t*>(oGetViewAngles(this, 0));
	}
};
