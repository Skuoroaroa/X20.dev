#pragma once

// used: QAngle_t
#include "qangle.h"
// used: MEM_PAD
#include "../../utilities/memory.h"
// used: memalloc
#include "../../core/interfaces.h"
#include <algorithm>
#include <cmath>
#include <cfloat>

#include "../interfaces/imemalloc.h"


// @source: server.dll
enum ECommandButtons : std::uint64_t
{
	IN_ATTACK = 1 << 0,
	IN_JUMP = 1 << 1,
	IN_DUCK = 1 << 2,
	IN_FORWARD = 1 << 3,
	IN_BACK = 1 << 4,
	IN_USE = 1 << 5,
	IN_LEFT = 1 << 7,
	IN_RIGHT = 1 << 8,
	IN_MOVELEFT = 1 << 9,
	IN_MOVERIGHT = 1 << 10,
	IN_SECOND_ATTACK = 1 << 11,
	IN_RELOAD = 1 << 13,
	IN_SPRINT = 1 << 16,
	IN_JOYAUTOSPRINT = 1 << 17,
	IN_SHOWSCORES = 1ULL << 33,
	IN_ZOOM = 1ULL << 34,
	IN_LOOKATWEAPON = 1ULL << 35
};

// compiled protobuf messages and looked at what bits are used in them
enum ESubtickMoveStepBits : std::uint32_t
{
	MOVESTEP_BITS_BUTTON = 0x1U,
	MOVESTEP_BITS_PRESSED = 0x2U,
	MOVESTEP_BITS_WHEN = 0x4U,
	MOVESTEP_BITS_ANALOG_FORWARD_DELTA = 0x8U,
	MOVESTEP_BITS_ANALOG_LEFT_DELTA = 0x10U
};

enum EInputHistoryBits : std::uint32_t
{
	INPUT_HISTORY_BITS_VIEWANGLES = 0x1U,
	INPUT_HISTORY_BITS_SHOOTPOSITION = 0x2U,
	INPUT_HISTORY_BITS_TARGETHEADPOSITIONCHECK = 0x4U,
	INPUT_HISTORY_BITS_TARGETABSPOSITIONCHECK = 0x8U,
	INPUT_HISTORY_BITS_TARGETANGCHECK = 0x10U,
	INPUT_HISTORY_BITS_CL_INTERP = 0x20U,
	INPUT_HISTORY_BITS_SV_INTERP0 = 0x40U,
	INPUT_HISTORY_BITS_SV_INTERP1 = 0x80U,
	INPUT_HISTORY_BITS_PLAYER_INTERP = 0x100U,
	INPUT_HISTORY_BITS_RENDERTICKCOUNT = 0x200U,
	INPUT_HISTORY_BITS_RENDERTICKFRACTION = 0x400U,
	INPUT_HISTORY_BITS_PLAYERTICKCOUNT = 0x800U,
	INPUT_HISTORY_BITS_PLAYERTICKFRACTION = 0x1000U,
	INPUT_HISTORY_BITS_FRAMENUMBER = 0x2000U,
	INPUT_HISTORY_BITS_TARGETENTINDEX = 0x4000U
};

enum EButtonStatePBBits : uint32_t
{
	BUTTON_STATE_PB_BITS_BUTTONSTATE1 = 0x1U,
	BUTTON_STATE_PB_BITS_BUTTONSTATE2 = 0x2U,
	BUTTON_STATE_PB_BITS_BUTTONSTATE3 = 0x4U
};

enum EBaseCmdBits : std::uint32_t
{
	BASE_BITS_MOVE_CRC = 0x1U,
	BASE_BITS_BUTTONPB = 0x2U,
	BASE_BITS_VIEWANGLES = 0x4U,
	BASE_BITS_COMMAND_NUMBER = 0x8U,
	BASE_BITS_CLIENT_TICK = 0x10U,
	BASE_BITS_FORWARDMOVE = 0x20U,
	BASE_BITS_LEFTMOVE = 0x40U,
	BASE_BITS_UPMOVE = 0x80U,
	BASE_BITS_IMPULSE = 0x100U,
	BASE_BITS_WEAPON_SELECT = 0x200U,
	BASE_BITS_RANDOM_SEED = 0x400U,
	BASE_BITS_MOUSEDX = 0x800U,
	BASE_BITS_MOUSEDY = 0x1000U,
	BASE_BITS_CONSUMED_SERVER_ANGLE = 0x2000U,
	BASE_BITS_CMD_FLAGS = 0x4000U,
	BASE_BITS_ENTITY_HANDLE = 0x8000U
};

enum ECSGOUserCmdBits : std::uint32_t
{
	CSGOUSERCMD_BITS_BASECMD = 0x1U,
	CSGOUSERCMD_BITS_LEFTHAND = 0x2U,
	CSGOUSERCMD_BITS_ATTACK3START = 0x4U,
	CSGOUSERCMD_BITS_ATTACK1START = 0x8U,
	CSGOUSERCMD_BITS_ATTACK2START = 0x10U
};

template <typename T>
struct RepeatedPtrField_t
{
	struct Rep_t
	{
		int nAllocatedSize;
		T* tElements[(std::numeric_limits<int>::max() - 2 * sizeof(int)) / sizeof(void*)];
	};

	void* pArena;
	int nCurrentSize;
	int nTotalSize;
	Rep_t* pRep;

	template <typename T>
	T* add(T* element)
	{
		static auto add_to_rep_addr = reinterpret_cast<T * (__fastcall*)(RepeatedPtrField_t*, T*)>(MEM::GetAbsoluteAddress(MEM::FindPattern(CLIENT_DLL, "E8 ? ? ? ? 4C 8B E0 48 8B 44 24 ? 4C 8B CF"), 0x1));
		return add_to_rep_addr(this, element);
	}
};

class CBasePB
{
public:
	MEM_PAD(0x8) // 0x0 VTABLE
		std::uint32_t nHasBits; // 0x8
	std::uint64_t nCachedBits; // 0xC

	void SetBits(std::uint64_t nBits)
	{
		nCachedBits |= nBits;
	}
};

static_assert(sizeof(CBasePB) == 0x18);

class CMsgQAngle : public CBasePB
{
public:
	QAngle_t angValue; // 0x18
};

static_assert(sizeof(CMsgQAngle) == 0x28);

class CMsgVector : public CBasePB
{
public:
	Vector4D_t vecValue; // 0x18
};

static_assert(sizeof(CMsgVector) == 0x28);

class CCSGOInterpolationInfoPB : public CBasePB
{
public:
	float flFraction; // 0x18
	int nSrcTick; // 0x1C
	int nDstTick; // 0x20
};

static_assert(sizeof(CCSGOInterpolationInfoPB) == 0x28);

class CCSGOInputHistoryEntryPB : public CBasePB
{
public:
	CMsgQAngle* pViewAngles; // 0x18
	CMsgVector* pShootPosition; // 0x20
	CMsgVector* pTargetHeadPositionCheck; // 0x28
	CMsgVector* pTargetAbsPositionCheck; // 0x30
	CMsgQAngle* pTargetAngPositionCheck; // 0x38
	CCSGOInterpolationInfoPB* cl_interp; // 0x40
	CCSGOInterpolationInfoPB* sv_interp0; // 0x48
	CCSGOInterpolationInfoPB* sv_interp1; // 0x50
	CCSGOInterpolationInfoPB* player_interp; // 0x58
	int nRenderTickCount; // 0x60
	float flRenderTickFraction; // 0x64
	int nPlayerTickCount; // 0x68
	float flPlayerTickFraction; // 0x6C
	int nFrameNumber; // 0x70
	int nTargetEntIndex; // 0x74
};

static_assert(sizeof(CCSGOInputHistoryEntryPB) == 0x78);

struct CInButtonStatePB : CBasePB
{
	std::uint64_t nValue;
	std::uint64_t nValueChanged;
	std::uint64_t nValueScroll;
};

static_assert(sizeof(CInButtonStatePB) == 0x30);

struct CSubtickMoveStep : CBasePB
{
public:
	std::uint64_t nButton;
	bool bPressed;
	float flWhen;
	float flAnalogForwardDelta;
	float flAnalogLeftDelta;

	// Helper to create a subtick step with proper fraction
	void Setup(std::uint64_t nButtonMask, bool bPressedState, float flFraction)
	{
		nButton = nButtonMask;
		bPressed = bPressedState;
		flWhen = flFraction;
		flAnalogForwardDelta = 0.0f;
		flAnalogLeftDelta = 0.0f;

		// Set the appropriate bits
		SetBits(MOVESTEP_BITS_BUTTON | MOVESTEP_BITS_PRESSED | MOVESTEP_BITS_WHEN);
	}
};

static_assert(sizeof(CSubtickMoveStep) == 0x30);

class CBaseUserCmdPB : public CBasePB
{
public:
	RepeatedPtrField_t<CSubtickMoveStep> subtickMovesField;
	std::string* strMoveCrc;
	CInButtonStatePB* pInButtonState;
	CMsgQAngle* pViewAngles;
	std::int32_t nLegacyCommandNumber;
	std::int32_t nClientTick;
	float flForwardMove;
	float flSideMove;
	float flUpMove;
	std::int32_t nImpulse;
	std::int32_t nWeaponSelect;
	std::int32_t nRandomSeed;
	std::int32_t nMousedX;
	std::int32_t nMousedY;
	std::uint32_t nConsumedServerAngleChanges;
	std::int32_t nCmdFlags;
	std::uint32_t nPawnEntityHandle;

	int CalculateCmdCRCSize()
	{
		return MEM::CallVFunc<int, 7U>(this);
	}

	// Add a subtick move step with proper sorting
	void AddSubtickStep(std::uint64_t nButton, bool bPressed, float flFraction)
	{
		CSubtickMoveStep* pStep = new CSubtickMoveStep();
		pStep->Setup(nButton, bPressed, flFraction);
		subtickMovesField.add(pStep);

		// Mark that we have subtick moves
		SetBits(BASE_BITS_BUTTONPB);
	}
};
static_assert(sizeof(CBaseUserCmdPB) == 0x80);

class CCSGOUserCmdPB
{
public:
	std::uint32_t nHasBits;
	std::uint64_t nCachedSize;
	RepeatedPtrField_t<CCSGOInputHistoryEntryPB> inputHistoryField;

	CBaseUserCmdPB* pBaseCmd;
	bool bLeftHandDesired;
	std::int32_t nAttack3StartHistoryIndex;
	std::int32_t nAttack1StartHistoryIndex;
	std::int32_t nAttack2StartHistoryIndex;

	void CheckAndSetBits(std::uint32_t nBits)
	{
		if (!(nHasBits & nBits))
			nHasBits |= nBits;
	}

	// Add input history entry for subtick tracking
	CCSGOInputHistoryEntryPB* AddInputHistoryEntry()
	{
		CCSGOInputHistoryEntryPB* pEntry = new CCSGOInputHistoryEntryPB();
		inputHistoryField.add(pEntry);
		CheckAndSetBits(CSGOUSERCMD_BITS_BASECMD);
		return pEntry;
	}
};
static_assert(sizeof(CCSGOUserCmdPB) == 0x40);

struct CInButtonState
{
public:
	MEM_PAD(0x8) // 0x0 VTABLE
		std::uint64_t nValue; // 0x8
	std::uint64_t nValueChanged; // 0x10
	std::uint64_t nValueScroll; // 0x18
};
static_assert(sizeof(CInButtonState) == 0x20);

// Subtick command builder for proper command synthesis
class CSubtickCommandBuilder
{

public:
	// Calculate fraction based on frame timing for precise timestamping
	static float CalculateFraction(float flCurrentTime, float flTickStartTime)
	{
		float flFraction = (flCurrentTime - flTickStartTime) / TICK_INTERVAL;
		return std::clamp(flFraction, 0.0f, 0.999f); // Keep just under 1.0
	}

	// Build attack step - use current prediction fraction for accurate origin
	static void BuildAttackStep(CBaseUserCmdPB* pBaseCmd, float flPredictionFraction, uint64_t nMovementButtons = 0)
	{
		if (!pBaseCmd) return;

		uint64_t nButtonMask = IN_ATTACK | nMovementButtons;
		pBaseCmd->AddSubtickStep(nButtonMask, true, flPredictionFraction);
	}

	// Build jump step - use fraction 0.0 for maximum airborne time
	static void BuildJumpStep(CBaseUserCmdPB* pBaseCmd, bool bShouldJump, uint64_t nMovementButtons = 0)
	{
		if (!pBaseCmd || !bShouldJump) return;

		// fraction 0.0 = jump impulse at tick start for maximum physics integration
		uint64_t nButtonMask = IN_JUMP | nMovementButtons;
		pBaseCmd->AddSubtickStep(nButtonMask, true, 0.0f);

		// Add release step if needed
		pBaseCmd->AddSubtickStep(IN_JUMP, false, 0.001f);
	}

	// Build split command for shot during movement
	static void BuildSplitCommand(CBaseUserCmdPB* pBaseCmd, float flShotFraction,
		uint64_t nMovementButtons, bool bAttack)
	{
		if (!pBaseCmd) return;

		// Step 1: Movement without attack (just before shot)
		float flPreFraction = std::max(0.0f, flShotFraction - 0.001f);
		pBaseCmd->AddSubtickStep(nMovementButtons, true, flPreFraction);

		// Step 2: Attack begins at shot_fraction
		if (bAttack)
		{
			uint64_t nAttackMask = nMovementButtons | IN_ATTACK;
			pBaseCmd->AddSubtickStep(nAttackMask, true, flShotFraction);
		}
	}

	// Build secondary attack (scope)
	static void BuildSecondaryAttackStep(CBaseUserCmdPB* pBaseCmd, float flFraction, bool bScope)
	{
		if (!pBaseCmd) return;

		if (bScope)
		{
			pBaseCmd->AddSubtickStep(IN_SECOND_ATTACK, true, flFraction);
		}
		else
		{
			pBaseCmd->AddSubtickStep(IN_SECOND_ATTACK, false, flFraction);
		}
	}

	// Build reload step
	static void BuildReloadStep(CBaseUserCmdPB* pBaseCmd, float flFraction, bool bReload)
	{
		if (!pBaseCmd) return;
		pBaseCmd->AddSubtickStep(IN_RELOAD, bReload, flFraction);
	}

	// Build use step
	static void BuildUseStep(CBaseUserCmdPB* pBaseCmd, float flFraction, bool bUse)
	{
		if (!pBaseCmd) return;
		pBaseCmd->AddSubtickStep(IN_USE, bUse, flFraction);
	}
};

class CUserCmd
{
public:
	MEM_PAD(0x8); // 0x0 VTABLE
	MEM_PAD(0x10); // TODO: find out what this is, added 14.08.2024
	CCSGOUserCmdPB csgoUserCmd; // 0x18
	CInButtonState nButtons; // 0x58
	MEM_PAD(0x20); // 0x78

	CCSGOInputHistoryEntryPB* GetInputHistoryEntry(int nIndex)
	{
		if (nIndex >= csgoUserCmd.inputHistoryField.pRep->nAllocatedSize ||
			nIndex >= csgoUserCmd.inputHistoryField.nCurrentSize)
			return nullptr;

		return csgoUserCmd.inputHistoryField.pRep->tElements[nIndex];
	}

	void SetSubTickAngle(const QAngle_t& angView)
	{
		for (int i = 0; i < csgoUserCmd.inputHistoryField.pRep->nAllocatedSize; i++)
		{
			CCSGOInputHistoryEntryPB* pInputEntry = GetInputHistoryEntry(i);
			if (!pInputEntry || !pInputEntry->pViewAngles)
				continue;

			pInputEntry->pViewAngles->angValue = angView;
		}
	}

	// Helper to initialize command for subtick use
	void InitializeSubtickCommand(int nClientTick, const QAngle_t& angViewAngles)
	{
		if (!csgoUserCmd.pBaseCmd)
			return;

		// Setup base command
		csgoUserCmd.pBaseCmd->nClientTick = nClientTick;
		csgoUserCmd.pBaseCmd->nLegacyCommandNumber = nClientTick;

		// Setup view angles if not already present
		if (!csgoUserCmd.pBaseCmd->pViewAngles)
		{
			csgoUserCmd.pBaseCmd->pViewAngles = new CMsgQAngle();
		}
		csgoUserCmd.pBaseCmd->pViewAngles->angValue = angViewAngles;
		csgoUserCmd.pBaseCmd->SetBits(BASE_BITS_VIEWANGLES);

		// Setup button state
		if (!csgoUserCmd.pBaseCmd->pInButtonState)
		{
			csgoUserCmd.pBaseCmd->pInButtonState = new CInButtonStatePB();
		}

		// Ensure base command has proper bits set
		csgoUserCmd.pBaseCmd->SetBits(BASE_BITS_BUTTONPB | BASE_BITS_CLIENT_TICK);

		// Mark that we have base command in user cmd
		csgoUserCmd.CheckAndSetBits(CSGOUSERCMD_BITS_BASECMD);
	}

	// Add input history entry for this tick's context
	CCSGOInputHistoryEntryPB* AddInputHistoryEntry()
	{
		return csgoUserCmd.AddInputHistoryEntry();
	}

	// Get base command for adding subtick steps
	CBaseUserCmdPB* GetBaseCommand()
	{
		return csgoUserCmd.pBaseCmd;
	}

	// Clear subtick moves (useful when recycling commands)
	void ClearSubtickMoves()
	{
		if (!csgoUserCmd.pBaseCmd)
			return;

		// Reset the repeated field
		csgoUserCmd.pBaseCmd->subtickMovesField.nCurrentSize = 0;
	}
};
static_assert(sizeof(CUserCmd) == 0x98);

// Lerp time helper for proper lag compensation with subtick precision
class CSubtickLerpHelper
{


public:
	// Get lerp time with proper sub-tick precision
	static float GetLerpTime()
	{
		// These would be from convar interfaces in practice
		static float flInterpRatio = 1.0f; // cl_interp_ratio
		static float flMinInterpRatio = 1.0f; // sv_client_min_interp_ratio
		static float flMaxInterpRatio = 2.0f; // sv_client_max_interp_ratio

		float flClamped = std::clamp(flInterpRatio, flMinInterpRatio, flMaxInterpRatio);

		// Return continuous value - NOT rounded to tick boundaries
		// This maintains sub-tick precision for validity checks
		return flClamped * TICK_INTERVAL;
	}

	// Check if a lag record is valid with sub-tick precision
	static bool IsRecordValid(float flSimulationTime, float flCurrentTickTime, float flNetLatency)
	{
		float flLerpTime = std::max(GetLerpTime(), TICK_INTERVAL);
		float flExpectedDiff = std::clamp(flNetLatency * 2.0f + flLerpTime, 0.0f, 1.0f); // sv_maxunlag approx 1.0
		float flActualDiff = flCurrentTickTime - flSimulationTime;
		float flDelta = std::fabs(flExpectedDiff - flActualDiff);

		// Use continuous comparison without rounding
		return flDelta <= 1.0f; // sv_maxunlag threshold
	}
};