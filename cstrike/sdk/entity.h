#pragma once

// @test: using interfaces in the header | not critical but could blow up someday with thousands of errors or affect to compilation time etc
// used: cgameentitysystem, ischemasystem
#include "../core/interfaces.h"
#include "interfaces/igameresourceservice.h"
#include "interfaces/ischemasystem.h"

// used: schema field
#include "../core/schema.h"

// used: l_print
#include "../utilities/log.h"
// used: vector_t
#include "datatypes/vector.h"
// used: qangle_t
#include "datatypes/qangle.h"
// used: ctransform
#include "datatypes/transform.h"

// used: cbasehandle
#include "entity_handle.h"
// used: game's definitions
#include "const.h"
// used: entity vdata
#include "vdata.h"

#include <vector>

using GameTime_t = std::float_t;
using GameTick_t = std::int32_t;

class C_CSWeaponBase;
class CEntityInstance;
class CCSWeaponBaseVData;
class CEntityIdentity
{
public:
	CS_CLASS_NO_INITIALIZER(CEntityIdentity);

	// @note: handle index is not entity index
	SCHEMA_ADD_OFFSET(std::uint32_t, GetIndex, 0x10);
	SCHEMA_ADD_FIELD(const char*, GetDesignerName, "CEntityIdentity", "m_designerName");
	SCHEMA_ADD_FIELD(std::uint32_t, GetFlags, "CEntityIdentity", "m_flags");

	[[nodiscard]] bool IsValid()
	{
		return GetIndex() != INVALID_EHANDLE_INDEX;
	}

	[[nodiscard]] int GetEntryIndex()
	{
		if (!IsValid())
			return ENT_ENTRY_MASK;

		return GetIndex() & ENT_ENTRY_MASK;
	}

	[[nodiscard]] int GetSerialNumber()
	{
		return GetIndex() >> NUM_SERIAL_NUM_SHIFT_BITS;
	}

	CEntityInstance* pInstance; // 0x00
};

class CEntityInstance
{
public:
	CS_CLASS_NO_INITIALIZER(CEntityInstance);

	void GetSchemaClassInfo(CSchemaClassInfo** pReturn)
	{
		return MEM::CallVFunc<void, 46>(this, pReturn);
	}

	[[nodiscard]] CBaseHandle GetRefEHandle()
	{
		CEntityIdentity* pIdentity = GetIdentity();
		if (pIdentity == nullptr)
			return CBaseHandle();

		return CBaseHandle(pIdentity->GetEntryIndex(), pIdentity->GetSerialNumber() - (pIdentity->GetFlags() & 1));
	}

	SCHEMA_ADD_FIELD(CEntityIdentity*, GetIdentity, "CEntityInstance", "m_pEntity");
};

class CCollisionProperty
{
public:
	std::uint16_t CollisionMask()
	{
		return *reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(this) + 0x38);
	}

	CS_CLASS_NO_INITIALIZER(CCollisionProperty);

	SCHEMA_ADD_FIELD(Vector_t, GetMins, "CCollisionProperty", "m_vecMins");
	SCHEMA_ADD_FIELD(Vector_t, GetMaxs, "CCollisionProperty", "m_vecMaxs");

	SCHEMA_ADD_FIELD(std::uint8_t, GetSolidFlags, "CCollisionProperty", "m_usSolidFlags");
	SCHEMA_ADD_FIELD(std::uint8_t, GetCollisionGroup, "CCollisionProperty", "m_CollisionGroup");
};


class CGameSceneNode
{
public:
	CS_CLASS_NO_INITIALIZER(CGameSceneNode);

	SCHEMA_ADD_FIELD(CTransform, GetNodeToWorld, "CGameSceneNode", "m_nodeToWorld");
	SCHEMA_ADD_FIELD(CEntityInstance*, GetOwner, "CGameSceneNode", "m_pOwner");

	SCHEMA_ADD_FIELD(Vector_t, GetAbsOrigin, "CGameSceneNode", "m_vecAbsOrigin");
	SCHEMA_ADD_FIELD(Vector_t, GetRenderOrigin, "CGameSceneNode", "m_vRenderOrigin");

	SCHEMA_ADD_FIELD(QAngle_t, GetAngleRotation, "CGameSceneNode", "m_angRotation");
	SCHEMA_ADD_FIELD(QAngle_t, GetAbsAngleRotation, "CGameSceneNode", "m_angAbsRotation");

	SCHEMA_ADD_FIELD(bool, IsDormant, "CGameSceneNode", "m_bDormant");


};





class C_BaseEntity : public CEntityInstance
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseEntity);

	[[nodiscard]] bool IsBasePlayerController()
	{
		CSchemaClassInfo* pClassInfo;
		GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			return false;

		return FNV1A::Hash(pClassInfo->Name) == FNV1A::HashConst("C_CSPlayerController");
	}

	static C_BaseEntity* GetLocalPlayer();

	// get entity origin on scene
	[[nodiscard]] const Vector_t& GetSceneOrigin();







	SCHEMA_ADD_FIELD(CGameSceneNode*, GetGameSceneNode, "C_BaseEntity", "m_pGameSceneNode");
	SCHEMA_ADD_FIELD(CCollisionProperty*, GetCollision, "C_BaseEntity", "m_pCollision");
	SCHEMA_ADD_FIELD(std::uint8_t, GetTeam, "C_BaseEntity", "m_iTeamNum");
	SCHEMA_ADD_FIELD(CBaseHandle, GetOwnerHandle, "C_BaseEntity", "m_hOwnerEntity");
	SCHEMA_ADD_FIELD(Vector_t, GetBaseVelocity, "C_BaseEntity", "m_vecBaseVelocity");
	SCHEMA_ADD_FIELD(Vector_t, GetAbsVelocity, "C_BaseEntity", "m_vecAbsVelocity");
	SCHEMA_ADD_FIELD(Vector_t, GetVecVelocity, "C_BaseEntity", "m_vecVelocity");
	SCHEMA_ADD_FIELD(bool, IsTakingDamage, "C_BaseEntity", "m_bTakesDamage");
	SCHEMA_ADD_FIELD(std::uint32_t, GetFlags, "C_BaseEntity", "m_fFlags");
	SCHEMA_ADD_FIELD(std::int32_t, GetEflags, "C_BaseEntity", "m_iEFlags");
	SCHEMA_ADD_FIELD(std::uint8_t, GetMoveType, "C_BaseEntity", "m_nActualMoveType"); // m_nActualMoveType returns CSGO style movetype, m_nMoveType returns bitwise shifted move type
	SCHEMA_ADD_FIELD(std::uint8_t, GetLifeState, "C_BaseEntity", "m_lifeState");
	SCHEMA_ADD_FIELD(std::int32_t, GetHealth, "C_BaseEntity", "m_iHealth");
	SCHEMA_ADD_FIELD(std::int32_t, GetMaxHealth, "C_BaseEntity", "m_iMaxHealth");
	SCHEMA_ADD_FIELD(float, GetWaterLevel, "C_BaseEntity", "m_flWaterLevel");
	SCHEMA_ADD_FIELD_EX(void*, GetVData, "C_BaseEntity", "m_nSubclassID", 0x8);
};

class CGlowProperty;

class C_BaseModelEntity : public C_BaseEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseModelEntity);

	SCHEMA_ADD_FIELD(CCollisionProperty, GetCollisionInstance, "C_BaseModelEntity", "m_Collision");
	SCHEMA_ADD_FIELD(CGlowProperty, GetGlowProperty, "C_BaseModelEntity", "m_Glow");
	SCHEMA_ADD_FIELD(Vector_t, GetViewOffset, "C_BaseModelEntity", "m_vecViewOffset");
	SCHEMA_ADD_FIELD(GameTime_t, GetCreationTime, "C_BaseModelEntity", "m_flCreateTime");
	SCHEMA_ADD_FIELD(GameTick_t, GetCreationTick, "C_BaseModelEntity", "m_nCreationTick");
	SCHEMA_ADD_FIELD(CBaseHandle, GetMoveParent, "C_BaseModelEntity", "m_hOldMoveParent");
	SCHEMA_ADD_FIELD(std::float_t, GetAnimTime, "C_BaseModelEntity", "m_flAnimTime");
	SCHEMA_ADD_FIELD(std::float_t, GetSimulationTime, "C_BaseModelEntity", "m_flSimulationTime");
};

class CPlayer_ItemServices;
class CPlayer_CameraServices;

class CPlayer_WeaponServices
{
public:
	SCHEMA_ADD_FIELD(CBaseHandle, GetActiveWeapon, "CPlayer_WeaponServices", "m_hActiveWeapon");
	[[nodiscard]] Vector_t GetWeaponShootPosition()
	{
		Vector_t ret = {};
		MEM::CallVFunc<void, 26>(this, &ret);
		return ret;
	}

};

class CCSPlayer_WeaponServices : public CPlayer_WeaponServices
{
public:
	CS_CLASS_NO_INITIALIZER(CCSPlayer_WeaponServices);

	SCHEMA_ADD_FIELD(GameTime_t, GetNextAttack,
		"CCSPlayer_WeaponServices", "m_flNextAttack");

	SCHEMA_ADD_FIELD(bool, IsLookingAtWeapon,
		"CCSPlayer_WeaponServices", "m_bIsLookingAtWeapon");

	SCHEMA_ADD_FIELD(bool, IsHoldingLookAtWeapon,
		"CCSPlayer_WeaponServices", "m_bIsHoldingLookAtWeapon");

	SCHEMA_ADD_FIELD(uint32_t, GetOldTotalShootPositionHistoryCount,
		"CCSPlayer_WeaponServices", "m_nOldTotalShootPositionHistoryCount");

	SCHEMA_ADD_FIELD(uint32_t, GetOldTotalInputHistoryCount,
		"CCSPlayer_WeaponServices", "m_nOldTotalInputHistoryCount");

	SCHEMA_ADD_FIELD(void*, GetNetworkAnimTiming,
		"CCSPlayer_WeaponServices", "m_networkAnimTiming");

	SCHEMA_ADD_FIELD(bool, IsBlockInspectUntilNextGraphUpdate,
		"CCSPlayer_WeaponServices", "m_bBlockInspectUntilNextGraphUpdate");
};


class CPlayer_MovementServices
{
public:
	CS_CLASS_NO_INITIALIZER(CPlayer_MovementServices);

	// Getters
	SCHEMA_ADD_FIELD(std::int32_t, GetImpulse, "CPlayer_MovementServices", "m_nImpulse");
	SCHEMA_ADD_FIELD(void*, GetButtons, "CPlayer_MovementServices", "m_nButtons"); // CInButtonState
	SCHEMA_ADD_FIELD(std::uint64_t, GetQueuedButtonDownMask, "CPlayer_MovementServices", "m_nQueuedButtonDownMask");
	SCHEMA_ADD_FIELD(std::uint64_t, GetQueuedButtonChangeMask, "CPlayer_MovementServices", "m_nQueuedButtonChangeMask");
	SCHEMA_ADD_FIELD(std::uint64_t, GetButtonDoublePressed, "CPlayer_MovementServices", "m_nButtonDoublePressed");
	SCHEMA_ADD_FIELD(std::uint32_t*, GetButtonPressedCmdNumber, "CPlayer_MovementServices", "m_pButtonPressedCmdNumber"); // [64]
	SCHEMA_ADD_FIELD(std::uint32_t, GetLastCommandNumberProcessed, "CPlayer_MovementServices", "m_nLastCommandNumberProcessed");
	SCHEMA_ADD_FIELD(std::uint64_t, GetToggleButtonDownMask, "CPlayer_MovementServices", "m_nToggleButtonDownMask");
	SCHEMA_ADD_FIELD(float, GetCmdForwardMove, "CPlayer_MovementServices", "m_flCmdForwardMove");
	SCHEMA_ADD_FIELD(float, GetCmdLeftMove, "CPlayer_MovementServices", "m_flCmdLeftMove");
	SCHEMA_ADD_FIELD(float, GetCmdUpMove, "CPlayer_MovementServices", "m_flCmdUpMove");
	SCHEMA_ADD_FIELD(float, GetMaxspeed, "CPlayer_MovementServices", "m_flMaxspeed");
	SCHEMA_ADD_FIELD(float*, GetForceSubtickMoveWhen, "CPlayer_MovementServices", "m_arrForceSubtickMoveWhen"); // [4]
	SCHEMA_ADD_FIELD(float, GetForwardMove, "CPlayer_MovementServices", "m_flForwardMove");
	SCHEMA_ADD_FIELD(float, GetLeftMove, "CPlayer_MovementServices", "m_flLeftMove");
	SCHEMA_ADD_FIELD(float, GetUpMove, "CPlayer_MovementServices", "m_flUpMove");
	SCHEMA_ADD_FIELD(Vector_t, GetLastMovementImpulses, "CPlayer_MovementServices", "m_vecLastMovementImpulses");
	SCHEMA_ADD_FIELD(QAngle_t, GetOldViewAngles, "CPlayer_MovementServices", "m_vecOldViewAngles");


};
class C_BasePlayerPawn : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_BasePlayerPawn);
	SCHEMA_ADD_FIELD(CPlayer_MovementServices*, GetMovementServices, "C_BasePlayerPawn", "m_pMovementServices");

	SCHEMA_ADD_FIELD(GameTime_t, GetNextAttack, "CCSPlayer_WeaponServices", "m_flNextAttack");
	SCHEMA_ADD_FIELD(CBaseHandle, GetControllerHandle, "C_BasePlayerPawn", "m_hController");
	SCHEMA_ADD_FIELD(CCSPlayer_WeaponServices*, GetWeaponServices, "C_BasePlayerPawn", "m_pWeaponServices");
	SCHEMA_ADD_FIELD(CPlayer_ItemServices*, GetItemServices, "C_BasePlayerPawn", "m_pItemServices");
	SCHEMA_ADD_FIELD(CPlayer_CameraServices*, GetCameraServices, "C_BasePlayerPawn", "m_pCameraServices");
	SCHEMA_ADD_FIELD(Vector_t, GetOldOrigin, "C_BasePlayerPawn", "m_vOldOrigin");
};

class CCSPlayer_ViewModelServices;

class C_CSPlayerPawnBase : public C_BasePlayerPawn

{
public:
	[[nodiscard]] C_CSWeaponBase* GetActiveWeapon();
	CS_CLASS_NO_INITIALIZER(C_CSPlayerPawnBase);


	SCHEMA_ADD_FIELD(CCSPlayer_ViewModelServices*, GetViewModelServices, "C_CSPlayerPawnBase", "m_pViewModelServices");
	SCHEMA_ADD_FIELD(float, GetLowerBodyYawTarget, "C_CSPlayerPawnBase", "m_flLowerBodyYawTarget");
	SCHEMA_ADD_FIELD(float, GetFlashMaxAlpha, "C_CSPlayerPawnBase", "m_flFlashMaxAlpha");
	SCHEMA_ADD_FIELD(float, GetFlashDuration, "C_CSPlayerPawnBase", "m_flFlashDuration");
	SCHEMA_ADD_FIELD(Vector_t, GetLastSmokeOverlayColor, "C_CSPlayerPawnBase", "m_vLastSmokeOverlayColor");
	SCHEMA_ADD_FIELD(int, GetSurvivalTeam, "C_CSPlayerPawnBase", "m_nSurvivalTeam"); // danger zone

};

class CModelState {
public:
	SCHEMA_ADD_FIELD(uintptr_t, GetBoneArray, "CModelState", "m_pBoneArray");
};
class CSkeletonInstance {
public:
	SCHEMA_ADD_FIELD(CModelState, GetModelState, "CSkeletonInstance", "m_modelState");

	SCHEMA_ADD_FIELD(bool, IsAnimationEnabled, "CSkeletonInstance", "m_bIsAnimationEnabled");
	SCHEMA_ADD_FIELD(bool, UseParentRenderBounds, "CSkeletonInstance", "m_bUseParentRenderBounds");
	SCHEMA_ADD_FIELD(bool, DisableSolidCollisionsForHierarchy, "CSkeletonInstance", "m_bDisableSolidCollisionsForHierarchy");

	SCHEMA_ADD_FIELD(bool, IsDirtyMotionType, "CSkeletonInstance", "m_bDirtyMotionType");
	SCHEMA_ADD_FIELD(bool, IsGeneratingLatchedParentSpaceState, "CSkeletonInstance", "m_bIsGeneratingLatchedParentSpaceState");

	SCHEMA_ADD_FIELD(uint8_t, GetHitboxSet, "CSkeletonInstance", "m_nHitboxSet");
};
class C_CSPlayerPawn : public C_CSPlayerPawnBase
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSPlayerPawn);
	[[nodiscard]] C_CSWeaponBase* GetActiveWeapon();
	[[nodiscard]] bool IsOtherEnemy(C_CSPlayerPawn*);
	[[nodiscard]] int GetAssociatedTeam();
	[[nodiscard]] bool CanAttack(const float);
	[[nodiscard]] std::uint32_t GetOwnerHandleIndex();
	[[nodiscard]] std::uint16_t GetCollisionMask();

	[[nodiscard]] Vector_t GetEyePosition();
	SCHEMA_ADD_FIELD(std::ptrdiff_t, GetBulletServices, "C_CSPlayerPawn", "m_pBulletServices");
	SCHEMA_ADD_FIELD(uintptr_t, GetAimPunchServices, "C_CSPlayerPawn", "m_pAimPunchServices");

	[[nodiscard]] QAngle_t GetAimPunchAngle()
	{
		uintptr_t pServices = GetAimPunchServices();
		if (!pServices)
			return {};
		return *reinterpret_cast<QAngle_t*>(pServices + 0x50);
	}
	SCHEMA_ADD_FIELD(bool, IsScoped, "C_CSPlayerPawn", "m_bIsScoped");
	SCHEMA_ADD_FIELD(bool, IsDefusing, "C_CSPlayerPawn", "m_bIsDefusing");
	SCHEMA_ADD_FIELD(bool, IsGrabbingHostage, "C_CSPlayerPawn", "m_bIsGrabbingHostage");
	SCHEMA_ADD_FIELD(bool, IsWaitForNoAttack, "C_CSPlayerPawn", "m_bWaitForNoAttack");
	SCHEMA_ADD_FIELD(int, GetShotsFired, "C_CSPlayerPawn", "m_iShotsFired");
	SCHEMA_ADD_FIELD(std::int32_t, GetArmorValue, "C_CSPlayerPawn", "m_ArmorValue");
	SCHEMA_ADD_FIELD_EX(bool, IsSpottedOnRadar, "C_CSPlayerPawn", "m_entitySpottedState", 0x8);
	[[nodiscard]] bool IsLocalPlayerPawn();

};
class CBaseAnimGraph : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(CBaseAnimGraph);

	SCHEMA_ADD_FIELD(bool, IsClientRagdoll, "CBaseAnimGraph", "m_bClientRagdoll");
};

class C_BaseFlex : public CBaseAnimGraph
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseFlex);
	/* not implemented */
};

// Add this to entity.h after the C_Inferno class definition
class C_SmokeGrenadeProjectile : public C_BaseFlex
{
public:
	CS_CLASS_NO_INITIALIZER(C_SmokeGrenadeProjectile);

	// Smoke effect timing
	SCHEMA_ADD_FIELD(std::int32_t, GetSmokeEffectTickBegin, "C_SmokeGrenadeProjectile", "m_nSmokeEffectTickBegin");
	SCHEMA_ADD_FIELD(bool, DidSmokeEffect, "C_SmokeGrenadeProjectile", "m_bDidSmokeEffect");
	SCHEMA_ADD_FIELD(std::int32_t, GetRandomSeed, "C_SmokeGrenadeProjectile", "m_nRandomSeed");

	// Smoke color and detonation
	SCHEMA_ADD_FIELD(Vector_t, GetSmokeColor, "C_SmokeGrenadeProjectile", "m_vSmokeColor");
	SCHEMA_ADD_FIELD(Vector_t, GetSmokeDetonationPos, "C_SmokeGrenadeProjectile", "m_vSmokeDetonationPos");

	// Voxel data for smoke volume
	SCHEMA_ADD_FIELD(void*, GetVoxelFrameData, "C_SmokeGrenadeProjectile", "m_VoxelFrameData");
	SCHEMA_ADD_FIELD(std::int32_t, GetVoxelFrameDataSize, "C_SmokeGrenadeProjectile", "m_nVoxelFrameDataSize");
	SCHEMA_ADD_FIELD(std::int32_t, GetVoxelUpdate, "C_SmokeGrenadeProjectile", "m_nVoxelUpdate");

	// Smoke state
	SCHEMA_ADD_FIELD(bool, IsSmokeVolumeDataReceived, "C_SmokeGrenadeProjectile", "m_bSmokeVolumeDataReceived");
	SCHEMA_ADD_FIELD(bool, IsSmokeEffectSpawned, "C_SmokeGrenadeProjectile", "m_bSmokeEffectSpawned");

	// Helper methods for ESP
	[[nodiscard]] bool IsActive()
	{
		return DidSmokeEffect() && IsSmokeEffectSpawned();
	}

	[[nodiscard]] float GetRadius()
	{
		// Smoke radius is typically around 150-180 units
		// You can make this configurable via a convar or variable
		return 160.f; // Standard CS2 smoke radius
	}

	[[nodiscard]] Vector_t GetSmokeOrigin()
	{
		Vector_t detonationPos = GetSmokeDetonationPos();
		if (!detonationPos.IsZero())
			return detonationPos;

		// Fallback to entity origin
		CGameSceneNode* pNode = GetGameSceneNode();
		if (pNode)
			return pNode->GetAbsOrigin();

		return Vector_t{};
	}
};
class CBasePlayerController : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(CBasePlayerController);

	SCHEMA_ADD_FIELD(std::uint64_t, GetSteamId, "CBasePlayerController", "m_steamID");
	SCHEMA_ADD_FIELD(std::uint32_t, GetTickBase, "CBasePlayerController", "m_nTickBase");
	SCHEMA_ADD_FIELD(CBaseHandle, GetPawnHandle, "CBasePlayerController", "m_hPawn");
	SCHEMA_ADD_FIELD(bool, IsLocalPlayerController, "CBasePlayerController", "m_bIsLocalPlayerController");
};

// forward decleration
class C_CSWeaponBaseGun;
class C_BasePlayerWeapon;
class CCSPlayerController : public CBasePlayerController
{
public:
	CS_CLASS_NO_INITIALIZER(CCSPlayerController);

	[[nodiscard]] static CCSPlayerController* GetLocalPlayerController();

	// @note: always get origin from pawn not controller
	[[nodiscard]] const Vector_t& GetPawnOrigin();

	SCHEMA_ADD_FIELD(std::uint32_t, GetPing, "CCSPlayerController", "m_iPing");
	SCHEMA_ADD_FIELD(const char*, GetPlayerName, "CCSPlayerController", "m_sSanitizedPlayerName");
	SCHEMA_ADD_FIELD(std::int32_t, GetPawnHealth, "CCSPlayerController", "m_iPawnHealth");
	SCHEMA_ADD_FIELD(std::int32_t, GetPawnArmor, "CCSPlayerController", "m_iPawnArmor");
	SCHEMA_ADD_FIELD(bool, IsPawnHasDefuser, "CCSPlayerController", "m_bPawnHasDefuser");
	SCHEMA_ADD_FIELD(bool, IsPawnHasHelmet, "CCSPlayerController", "m_bPawnHasHelmet");
	SCHEMA_ADD_FIELD(bool, IsPawnAlive, "CCSPlayerController", "m_bPawnIsAlive");
	SCHEMA_ADD_FIELD(CBaseHandle, GetPlayerPawnHandle, "CCSPlayerController", "m_hPlayerPawn");
};



class C_EconItemView
{
public:
	CS_CLASS_NO_INITIALIZER(C_EconItemView);

	SCHEMA_ADD_FIELD(std::uint16_t, GetItemDefinitionIndex, "C_EconItemView", "m_iItemDefinitionIndex");
	SCHEMA_ADD_FIELD(std::uint64_t, GetItemID, "C_EconItemView", "m_iItemID");
	SCHEMA_ADD_FIELD(std::uint32_t, GetItemIDHigh, "C_EconItemView", "m_iItemIDHigh");
	SCHEMA_ADD_FIELD(std::uint32_t, GetItemIDLow, "C_EconItemView", "m_iItemIDLow");
	SCHEMA_ADD_FIELD(std::uint32_t, GetAccountID, "C_EconItemView", "m_iAccountID");
};

class CAttributeManager
{
public:
	CS_CLASS_NO_INITIALIZER(CAttributeManager);
	virtual ~CAttributeManager() = 0;
};
static_assert(sizeof(CAttributeManager) == 0x8);

class C_AttributeContainer : public CAttributeManager
{
public:
	CS_CLASS_NO_INITIALIZER(C_AttributeContainer);

	SCHEMA_ADD_FIELD(C_EconItemView, GetItem, "C_AttributeContainer", "m_Item");
};
class C_EconEntity : public C_BaseFlex
{
public:
	CS_CLASS_NO_INITIALIZER(C_EconEntity);

	SCHEMA_ADD_FIELD(C_AttributeContainer, GetAttributeManager, "C_EconEntity", "m_AttributeManager");
	SCHEMA_ADD_FIELD(std::uint32_t, GetOriginalOwnerXuidLow, "C_EconEntity", "m_OriginalOwnerXuidLow");
	SCHEMA_ADD_FIELD(std::uint32_t, GetOriginalOwnerXuidHigh, "C_EconEntity", "m_OriginalOwnerXuidHigh");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackPaintKit, "C_EconEntity", "m_nFallbackPaintKit");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackSeed, "C_EconEntity", "m_nFallbackSeed");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackWear, "C_EconEntity", "m_flFallbackWear");
	SCHEMA_ADD_FIELD(std::int32_t, GetFallbackStatTrak, "C_EconEntity", "m_nFallbackStatTrak");
	SCHEMA_ADD_FIELD(CBaseHandle, GetViewModelAttachmentHandle, "C_EconEntity", "m_hViewmodelAttachment");
};
class C_BasePlayerWeapon : public C_EconEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_BasePlayerWeapon);

	SCHEMA_ADD_FIELD(GameTick_t, GetNextPrimaryAttackTick, "C_BasePlayerWeapon", "m_nNextPrimaryAttackTick");
	SCHEMA_ADD_FIELD(float, GetNextPrimaryAttackTickRatio, "C_BasePlayerWeapon", "m_flNextPrimaryAttackTickRatio");
	SCHEMA_ADD_FIELD(GameTick_t, GetNextSecondaryAttackTick, "C_BasePlayerWeapon", "m_nNextSecondaryAttackTick");
	SCHEMA_ADD_FIELD(float, GetNextSecondaryAttackTickRatio, "C_BasePlayerWeapon", "m_flNextSecondaryAttackTickRatio");
	SCHEMA_ADD_FIELD(std::int32_t, GetClip1, "C_BasePlayerWeapon", "m_iClip1");
	SCHEMA_ADD_FIELD(std::int32_t, GetClip2, "C_BasePlayerWeapon", "m_iClip2");
};

class C_Inferno : public C_BaseModelEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_Inferno);

	// Fire effect & particles
	SCHEMA_ADD_FIELD(std::uint32_t, GetFireDamageEffect, "C_Inferno", "m_nfxFireDamageEffect"); // ParticleIndex_t
	SCHEMA_ADD_FIELD(void*, GetInfernoPointsSnapshot, "C_Inferno", "m_hInfernoPointsSnapshot"); // CStrongHandle<InfoForResourceTypeIParticleSnapshot>
	SCHEMA_ADD_FIELD(void*, GetInfernoFillerPointsSnapshot, "C_Inferno", "m_hInfernoFillerPointsSnapshot");
	SCHEMA_ADD_FIELD(void*, GetInfernoOutlinePointsSnapshot, "C_Inferno", "m_hInfernoOutlinePointsSnapshot");
	SCHEMA_ADD_FIELD(void*, GetInfernoClimbingOutlinePointsSnapshot, "C_Inferno", "m_hInfernoClimbingOutlinePointsSnapshot");
	SCHEMA_ADD_FIELD(void*, GetInfernoDecalsSnapshot, "C_Inferno", "m_hInfernoDecalsSnapshot");

	// Fire positions (array of 64 vectors)
	// Note: SCHEMA doesn't directly support arrays, so we'll use offset directly
	Vector_t* GetFirePositions()
	{
		return reinterpret_cast<Vector_t*>(reinterpret_cast<std::uintptr_t>(this) + 0x1018);
	}

	Vector_t* GetFireParentPositions()
	{
		return reinterpret_cast<Vector_t*>(reinterpret_cast<std::uintptr_t>(this) + 0x1318);
	}

	bool* GetFireIsBurning()
	{
		return reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + 0x1618);
	}

	Vector_t* GetBurnNormal()
	{
		return reinterpret_cast<Vector_t*>(reinterpret_cast<std::uintptr_t>(this) + 0x1658);
	}

	SCHEMA_ADD_FIELD(std::int32_t, GetFireCount, "C_Inferno", "m_fireCount");
	SCHEMA_ADD_FIELD(std::int32_t, GetInfernoType, "C_Inferno", "m_nInfernoType");
	SCHEMA_ADD_FIELD(float, GetFireLifetime, "C_Inferno", "m_nFireLifetime");
	SCHEMA_ADD_FIELD(bool, IsInPostEffectTime, "C_Inferno", "m_bInPostEffectTime");
	SCHEMA_ADD_FIELD(std::int32_t, GetLastFireCount, "C_Inferno", "m_lastFireCount");
	SCHEMA_ADD_FIELD(std::int32_t, GetFireEffectTickBegin, "C_Inferno", "m_nFireEffectTickBegin");

	// Drawable & bounds
	SCHEMA_ADD_FIELD(std::int32_t, GetDrawableCount, "C_Inferno", "m_drawableCount");
	SCHEMA_ADD_FIELD(bool, GetBlosCheck, "C_Inferno", "m_blosCheck");
	SCHEMA_ADD_FIELD(std::int32_t, GetLosPeriod, "C_Inferno", "m_nlosperiod");
	SCHEMA_ADD_FIELD(float, GetMaxFireHalfWidth, "C_Inferno", "m_maxFireHalfWidth");
	SCHEMA_ADD_FIELD(float, GetMaxFireHeight, "C_Inferno", "m_maxFireHeight");
	SCHEMA_ADD_FIELD(Vector_t, GetMinBounds, "C_Inferno", "m_minBounds");
	SCHEMA_ADD_FIELD(Vector_t, GetMaxBounds, "C_Inferno", "m_maxBounds");
	SCHEMA_ADD_FIELD(float, GetLastGrassBurnThink, "C_Inferno", "m_flLastGrassBurnThink");

	// Helper methods for ESP
	[[nodiscard]] float GetRadius()
	{
		Vector_t minBounds = GetMinBounds();
		Vector_t maxBounds = GetMaxBounds();

		// Calculate approximate radius from bounds
		float width = maxBounds.x - minBounds.x;
		float height = maxBounds.y - minBounds.y;

		return (width + height) / 4.0f;
	}

	[[nodiscard]] Vector_t GetFireOrigin(int index)
	{
		if (index < 0 || index >= GetFireCount())
			return Vector_t{};

		return GetFirePositions()[index];
	}

	[[nodiscard]] bool IsFireBurning(int index)
	{
		if (index < 0 || index >= GetFireCount())
			return false;

		return GetFireIsBurning()[index];
	}

	[[nodiscard]] int GetActiveFireCount()
	{
		int count = 0;
		int fireCount = GetFireCount();

		for (int i = 0; i < fireCount; i++)
		{
			if (IsFireBurning(i))
				count++;
		}

		return count;
	}

	// Get the main fire origin (first burning fire point)
	[[nodiscard]] Vector_t GetMainFireOrigin()
	{
		int fireCount = GetFireCount();

		for (int i = 0; i < fireCount; i++)
		{
			if (IsFireBurning(i))
				return GetFirePositions()[i];
		}

		// Fallback to entity origin
		CGameSceneNode* pNode = GetGameSceneNode();
		if (pNode)
			return pNode->GetAbsOrigin();

		return Vector_t{};
	}
};
class CCSWeaponBaseVData
{
public:
	CS_CLASS_NO_INITIALIZER(CCSWeaponBaseVData);

	// Weapon Type & Category
	SCHEMA_ADD_FIELD(int, GetWeaponType, "CCSWeaponBaseVData", "m_WeaponType");
	SCHEMA_ADD_FIELD(int, GetWeaponCategory, "CCSWeaponBaseVData", "m_WeaponCategory");

	// Animation
	SCHEMA_ADD_FIELD(const char*, GetAnimSkeleton, "CCSWeaponBaseVData", "m_szAnimSkeleton");

	// Muzzle positions
	SCHEMA_ADD_FIELD(Vector_t, GetMuzzlePos0, "CCSWeaponBaseVData", "m_vecMuzzlePos0");
	SCHEMA_ADD_FIELD(Vector_t, GetMuzzlePos1, "CCSWeaponBaseVData", "m_vecMuzzlePos1");

	// Tracer particle
	SCHEMA_ADD_FIELD(const char*, GetTracerParticle, "CCSWeaponBaseVData", "m_szTracerParticle");

	// Gear & Loadout
	SCHEMA_ADD_FIELD(int, GetGearSlot, "CCSWeaponBaseVData", "m_GearSlot");
	SCHEMA_ADD_FIELD(int, GetGearSlotPosition, "CCSWeaponBaseVData", "m_GearSlotPosition");
	SCHEMA_ADD_FIELD(int, GetDefaultLoadoutSlot, "CCSWeaponBaseVData", "m_DefaultLoadoutSlot");

	// Economy
	SCHEMA_ADD_FIELD(int, GetPrice, "CCSWeaponBaseVData", "m_nPrice");
	SCHEMA_ADD_FIELD(int, GetKillAward, "CCSWeaponBaseVData", "m_nKillAward");

	// Ammo
	SCHEMA_ADD_FIELD(int, GetPrimaryReserveAmmoMax, "CCSWeaponBaseVData", "m_nPrimaryReserveAmmoMax");
	SCHEMA_ADD_FIELD(int, GetSecondaryReserveAmmoMax, "CCSWeaponBaseVData", "m_nSecondaryReserveAmmoMax");

	// Weapon flags
	SCHEMA_ADD_FIELD(bool, IsMeleeWeapon, "CCSWeaponBaseVData", "m_bMeleeWeapon");
	SCHEMA_ADD_FIELD(bool, HasBurstMode, "CCSWeaponBaseVData", "m_bHasBurstMode");
	SCHEMA_ADD_FIELD(bool, IsRevolver, "CCSWeaponBaseVData", "m_bIsRevolver");
	SCHEMA_ADD_FIELD(bool, CannotShootUnderwater, "CCSWeaponBaseVData", "m_bCannotShootUnderwater");

	// Weapon name
	SCHEMA_ADD_FIELD(const char*, GetName, "CCSWeaponBaseVData", "m_szName");

	// Silencer
	SCHEMA_ADD_FIELD(int, GetSilencerType, "CCSWeaponBaseVData", "m_eSilencerType");

	// Crosshair
	SCHEMA_ADD_FIELD(int, GetCrosshairMinDistance, "CCSWeaponBaseVData", "m_nCrosshairMinDistance");
	SCHEMA_ADD_FIELD(int, GetCrosshairDeltaDistance, "CCSWeaponBaseVData", "m_nCrosshairDeltaDistance");

	// Fire mode
	SCHEMA_ADD_FIELD(bool, IsFullAuto, "CCSWeaponBaseVData", "m_bIsFullAuto");
	SCHEMA_ADD_FIELD(int, GetNumBullets, "CCSWeaponBaseVData", "m_nNumBullets");
	SCHEMA_ADD_FIELD(bool, ReloadsSingleShells, "CCSWeaponBaseVData", "m_bReloadsSingleShells");

	// Timing
	SCHEMA_ADD_FIELD(float, GetCycleTime, "CCSWeaponBaseVData", "m_flCycleTime");
	SCHEMA_ADD_FIELD(float, GetMaxSpeed, "CCSWeaponBaseVData", "m_flMaxSpeed");

	// Spread & Inaccuracy
	SCHEMA_ADD_FIELD(float, GetSpread, "CCSWeaponBaseVData", "m_flSpread");
	SCHEMA_ADD_FIELD(float, GetInaccuracyCrouch, "CCSWeaponBaseVData", "m_flInaccuracyCrouch");
	SCHEMA_ADD_FIELD(float, GetInaccuracyStand, "CCSWeaponBaseVData", "m_flInaccuracyStand");
	SCHEMA_ADD_FIELD(float, GetInaccuracyJump, "CCSWeaponBaseVData", "m_flInaccuracyJump");
	SCHEMA_ADD_FIELD(float, GetInaccuracyLand, "CCSWeaponBaseVData", "m_flInaccuracyLand");
	SCHEMA_ADD_FIELD(float, GetInaccuracyLadder, "CCSWeaponBaseVData", "m_flInaccuracyLadder");
	SCHEMA_ADD_FIELD(float, GetInaccuracyFire, "CCSWeaponBaseVData", "m_flInaccuracyFire");
	SCHEMA_ADD_FIELD(float, GetInaccuracyMove, "CCSWeaponBaseVData", "m_flInaccuracyMove");

	// Recoil
	SCHEMA_ADD_FIELD(float, GetRecoilAngle, "CCSWeaponBaseVData", "m_flRecoilAngle");
	SCHEMA_ADD_FIELD(float, GetRecoilAngleVariance, "CCSWeaponBaseVData", "m_flRecoilAngleVariance");
	SCHEMA_ADD_FIELD(float, GetRecoilMagnitude, "CCSWeaponBaseVData", "m_flRecoilMagnitude");
	SCHEMA_ADD_FIELD(float, GetRecoilMagnitudeVariance, "CCSWeaponBaseVData", "m_flRecoilMagnitudeVariance");

	// Tracer
	SCHEMA_ADD_FIELD(int, GetTracerFrequency, "CCSWeaponBaseVData", "m_nTracerFrequency");

	// Inaccuracy special cases
	SCHEMA_ADD_FIELD(float, GetInaccuracyJumpInitial, "CCSWeaponBaseVData", "m_flInaccuracyJumpInitial");
	SCHEMA_ADD_FIELD(float, GetInaccuracyJumpApex, "CCSWeaponBaseVData", "m_flInaccuracyJumpApex");
	SCHEMA_ADD_FIELD(float, GetInaccuracyReload, "CCSWeaponBaseVData", "m_flInaccuracyReload");

	// Deploy	SCHEMA_ADD_FIELD(float, GetDeployDuration, "CCSWeaponBaseVData", "m_flDeployDuration");
	SCHEMA_ADD_FIELD(float, GetDisallowAttackAfterReloadStartDuration, "CCSWeaponBaseVData", "m_flDisallowAttackAfterReloadStartDuration");

	// Burst
	SCHEMA_ADD_FIELD(int, GetBurstShotCount, "CCSWeaponBaseVData", "m_nBurstShotCount");
	SCHEMA_ADD_FIELD(bool, AllowBurstHolster, "CCSWeaponBaseVData", "m_bAllowBurstHolster");

	// Seeds
	SCHEMA_ADD_FIELD(int, GetRecoilSeed, "CCSWeaponBaseVData", "m_nRecoilSeed");
	SCHEMA_ADD_FIELD(int, GetSpreadSeed, "CCSWeaponBaseVData", "m_nSpreadSeed");

	// Movement
	SCHEMA_ADD_FIELD(float, GetAttackMoveSpeedFactor, "CCSWeaponBaseVData", "m_flAttackMovespeedFactor");
	SCHEMA_ADD_FIELD(float, GetInaccuracyPitchShift, "CCSWeaponBaseVData", "m_flInaccuracyPitchShift");
	SCHEMA_ADD_FIELD(float, GetInaccuracyAltSoundThreshold, "CCSWeaponBaseVData", "m_flInaccuracyAltSoundThreshold");

	// Radio
	SCHEMA_ADD_FIELD(const char*, GetUseRadioSubtitle, "CCSWeaponBaseVData", "m_szUseRadioSubtitle");

	// Zoom behavior
	SCHEMA_ADD_FIELD(bool, UnzoomsAfterShot, "CCSWeaponBaseVData", "m_bUnzoomsAfterShot");
	SCHEMA_ADD_FIELD(bool, HideViewModelWhenZoomed, "CCSWeaponBaseVData", "m_bHideViewModelWhenZoomed");
	SCHEMA_ADD_FIELD(int, GetZoomLevels, "CCSWeaponBaseVData", "m_nZoomLevels");
	SCHEMA_ADD_FIELD(int, GetZoomFOV1, "CCSWeaponBaseVData", "m_nZoomFOV1");
	SCHEMA_ADD_FIELD(int, GetZoomFOV2, "CCSWeaponBaseVData", "m_nZoomFOV2");
	SCHEMA_ADD_FIELD(float, GetZoomTime0, "CCSWeaponBaseVData", "m_flZoomTime0");
	SCHEMA_ADD_FIELD(float, GetZoomTime1, "CCSWeaponBaseVData", "m_flZoomTime1");
	SCHEMA_ADD_FIELD(float, GetZoomTime2, "CCSWeaponBaseVData", "m_flZoomTime2");

	// Iron sight
	SCHEMA_ADD_FIELD(float, GetIronSightPullUpSpeed, "CCSWeaponBaseVData", "m_flIronSightPullUpSpeed");
	SCHEMA_ADD_FIELD(float, GetIronSightPutDownSpeed, "CCSWeaponBaseVData", "m_flIronSightPutDownSpeed");
	SCHEMA_ADD_FIELD(float, GetIronSightFOV, "CCSWeaponBaseVData", "m_flIronSightFOV");
	SCHEMA_ADD_FIELD(float, GetIronSightPivotForward, "CCSWeaponBaseVData", "m_flIronSightPivotForward");
	SCHEMA_ADD_FIELD(float, GetIronSightLooseness, "CCSWeaponBaseVData", "m_flIronSightLooseness");

	// Damage
	SCHEMA_ADD_FIELD(int, GetDamage, "CCSWeaponBaseVData", "m_nDamage");
	SCHEMA_ADD_FIELD(float, GetHeadshotMultiplier, "CCSWeaponBaseVData", "m_flHeadshotMultiplier");
	SCHEMA_ADD_FIELD(float, GetArmorRatio, "CCSWeaponBaseVData", "m_flArmorRatio");
	SCHEMA_ADD_FIELD(float, GetPenetration, "CCSWeaponBaseVData", "m_flPenetration");
	SCHEMA_ADD_FIELD(float, GetRange, "CCSWeaponBaseVData", "m_flRange");
	SCHEMA_ADD_FIELD(float, GetRangeModifier, "CCSWeaponBaseVData", "m_flRangeModifier");

	// Flinch
	SCHEMA_ADD_FIELD(float, GetFlinchVelocityModifierLarge, "CCSWeaponBaseVData", "m_flFlinchVelocityModifierLarge");
	SCHEMA_ADD_FIELD(float, GetFlinchVelocityModifierSmall, "CCSWeaponBaseVData", "m_flFlinchVelocityModifierSmall");

	// Recovery time
	SCHEMA_ADD_FIELD(float, GetRecoveryTimeCrouch, "CCSWeaponBaseVData", "m_flRecoveryTimeCrouch");
	SCHEMA_ADD_FIELD(float, GetRecoveryTimeStand, "CCSWeaponBaseVData", "m_flRecoveryTimeStand");
	SCHEMA_ADD_FIELD(float, GetRecoveryTimeCrouchFinal, "CCSWeaponBaseVData", "m_flRecoveryTimeCrouchFinal");
	SCHEMA_ADD_FIELD(float, GetRecoveryTimeStandFinal, "CCSWeaponBaseVData", "m_flRecoveryTimeStandFinal");
	SCHEMA_ADD_FIELD(int, GetRecoveryTransitionStartBullet, "CCSWeaponBaseVData", "m_nRecoveryTransitionStartBullet");
	SCHEMA_ADD_FIELD(int, GetRecoveryTransitionEndBullet, "CCSWeaponBaseVData", "m_nRecoveryTransitionEndBullet");

	// Grenade specific
	SCHEMA_ADD_FIELD(float, GetThrowVelocity, "CCSWeaponBaseVData", "m_flThrowVelocity");
	SCHEMA_ADD_FIELD(Vector_t, GetSmokeColor, "CCSWeaponBaseVData", "m_vSmokeColor");

	// Animation class
	SCHEMA_ADD_FIELD(const char*, GetAnimClass, "CCSWeaponBaseVData", "m_szAnimClass");
};

class C_CSWeaponBase : public C_BasePlayerWeapon
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSWeaponBase);

	// --- STATE ---
	SCHEMA_ADD_FIELD(int, GetWeaponAnimState, "C_CSWeaponBase", "m_iWeaponGameplayAnimState");
	SCHEMA_ADD_FIELD(float, GetWeaponAnimStateTime, "C_CSWeaponBase", "m_flWeaponGameplayAnimStateTimestamp");

	SCHEMA_ADD_FIELD(float, GetInspectCancelTime, "C_CSWeaponBase", "m_flInspectCancelCompleteTime");
	SCHEMA_ADD_FIELD(bool, IsInspectPending, "C_CSWeaponBase", "m_bInspectPending");
	SCHEMA_ADD_FIELD(bool, ShouldInspectLoop, "C_CSWeaponBase", "m_bInspectShouldLoop");

	// --- ACCURACY ---
	SCHEMA_ADD_FIELD(float, GetCrosshairDistance, "C_CSWeaponBase", "m_flCrosshairDistance");
	SCHEMA_ADD_FIELD(float, GetAccuracyPenalty, "C_CSWeaponBase", "m_fAccuracyPenalty");
	SCHEMA_ADD_FIELD(float, GetLastAccuracyUpdateTime, "C_CSWeaponBase", "m_flLastAccuracyUpdateTime");
	SCHEMA_ADD_FIELD(float, GetAccuracyZoom, "C_CSWeaponBase", "m_fAccuracySmoothedForZoom");

	SCHEMA_ADD_FIELD(float, GetTurningInaccuracyDelta, "C_CSWeaponBase", "m_flTurningInaccuracyDelta");
	SCHEMA_ADD_FIELD(Vector_t, GetTurningEyeDir, "C_CSWeaponBase", "m_vecTurningInaccuracyEyeDirLast");
	SCHEMA_ADD_FIELD(float, GetTurningInaccuracy, "C_CSWeaponBase", "m_flTurningInaccuracy");

	// --- RECOIL ---
	SCHEMA_ADD_FIELD(int, GetRecoilIndexInt, "C_CSWeaponBase", "m_iRecoilIndex");
	SCHEMA_ADD_FIELD(float, GetRecoilIndex, "C_CSWeaponBase", "m_flRecoilIndex");

	// --- FIRE ---
	SCHEMA_ADD_FIELD(bool, IsFireOnEmpty, "C_CSWeaponBase", "m_bFireOnEmpty");
	SCHEMA_ADD_FIELD(int, GetAmmoLastCheck, "C_CSWeaponBase", "m_iAmmoLastCheck");

	SCHEMA_ADD_FIELD(int, GetPostponeFireTicks, "C_CSWeaponBase", "m_nPostponeFireReadyTicks");
	SCHEMA_ADD_FIELD(float, GetPostponeFireFrac, "C_CSWeaponBase", "m_flPostponeFireReadyFrac");

	SCHEMA_ADD_FIELD(float, GetLastShotTime, "C_CSWeaponBase", "m_fLastShotTime");

	SCHEMA_ADD_FIELD(float, GetNextClientFireTime, "C_CSWeaponBase", "m_flNextClientFireBulletTime");
	SCHEMA_ADD_FIELD(float, GetNextClientFireTimeRepredict, "C_CSWeaponBase", "m_flNextClientFireBulletTime_Repredict");

	// --- BURST / MODE ---
	SCHEMA_ADD_FIELD(int, GetWeaponMode, "C_CSWeaponBase", "m_weaponMode");
	SCHEMA_ADD_FIELD(bool, IsBurstMode, "C_CSWeaponBase", "m_bBurstMode");
	SCHEMA_ADD_FIELD(float, GetLastBurstChangeTime, "C_CSWeaponBase", "m_flLastBurstModeChangeTime");

	// --- RELOAD ---
	SCHEMA_ADD_FIELD(bool, IsInReload, "C_CSWeaponBase", "m_bInReload");

	// --- SILENCER ---
	SCHEMA_ADD_FIELD(bool, IsSilencerOn, "C_CSWeaponBase", "m_bSilencerOn");
	SCHEMA_ADD_FIELD(float, GetSilencerSwitchTime, "C_CSWeaponBase", "m_flTimeSilencerSwitchComplete");

	// --- DROP INFO ---
	SCHEMA_ADD_FIELD(float, GetDroppedTime, "C_CSWeaponBase", "m_flDroppedAtTime");
	SCHEMA_ADD_FIELD(bool, IsDroppedNearBuyZone, "C_CSWeaponBase", "m_bDroppedNearBuyZone");
	SCHEMA_ADD_FIELD(int, GetDropTick, "C_CSWeaponBase", "m_nDropTick");

	SCHEMA_ADD_FIELD(bool, WasActiveWhenDropped, "C_CSWeaponBase", "m_bWasActiveWeaponWhenDropped");

	// --- TEAM ---
	SCHEMA_ADD_FIELD(int, GetOriginalTeam, "C_CSWeaponBase", "m_iOriginalTeamNumber");
	SCHEMA_ADD_FIELD(int, GetMostRecentTeam, "C_CSWeaponBase", "m_iMostRecentTeamNumber");

	SCHEMA_ADD_FIELD(bool, WasOwnedByCT, "C_CSWeaponBase", "m_bWasOwnedByCT");
	SCHEMA_ADD_FIELD(bool, WasOwnedByT, "C_CSWeaponBase", "m_bWasOwnedByTerrorist");

	// --- MISC ---
	SCHEMA_ADD_FIELD(float, GetPlaybackRate, "C_CSWeaponBase", "m_flWeaponActionPlaybackRate");
	SCHEMA_ADD_FIELD(float, GetNextAttackRenderOffset, "C_CSWeaponBase", "m_flNextAttackRenderTimeOffset");

	SCHEMA_ADD_FIELD(bool, IsUIWeapon, "C_CSWeaponBase", "m_bUIWeapon");
	SCHEMA_ADD_FIELD(bool, HasVisualsData, "C_CSWeaponBase", "m_bVisualsDataSet");

	SCHEMA_ADD_FIELD(float, GetLastLOSFailureTime, "C_CSWeaponBase", "m_flLastLOSTraceFailureTime");
	SCHEMA_ADD_FIELD(float, GetLastShakeTime, "C_CSWeaponBase", "m_flLastShakeTime");

	// --- IRONSIGHT ---
	SCHEMA_ADD_FIELD(int, GetIronSightMode, "C_CSWeaponBase", "m_iIronSightMode");

	// --- OWNER ---
	SCHEMA_ADD_FIELD(CBaseHandle, GetPrevOwner, "C_CSWeaponBase", "m_hPrevOwner");
	__forceinline float get_inaccuracy() {
		float high;
		float low;
		float flTotalInaccuracy = MEM::CallVFunc<float, 413>(this, &high, &low);
		return flTotalInaccuracy;
	}
	__forceinline void UpdateAccuracyPenalty() {
		MEM::CallVFunc<void, 414>(this);
	}

	CCSWeaponBaseVData* GetWeaponVData()
	{
		return reinterpret_cast<CCSWeaponBaseVData*>(this->GetVData());
	}

};



class C_EconWearable : public C_EconEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_EconWearable);

	SCHEMA_ADD_FIELD(std::int32_t, GetForceSkin, "C_EconWearable", "m_nForceSkin");
	SCHEMA_ADD_FIELD(bool, IsAlwaysAllow, "C_EconWearable", "m_bAlwaysAllow");
};



class C_CSWeaponBaseGun : public C_CSWeaponBase
{
public:
	CS_CLASS_NO_INITIALIZER(C_CSWeaponBaseGun);

	SCHEMA_ADD_FIELD(std::int32_t, GetZoomLevel, "C_CSWeaponBaseGun", "m_zoomLevel");
	SCHEMA_ADD_FIELD(std::int32_t, GetBurstShotsRemaining, "C_CSWeaponBaseGun", "m_iBurstShotsRemaining");
	SCHEMA_ADD_FIELD(bool, IsBurstMode, "C_CSWeaponBase", "m_bBurstMode");
	SCHEMA_ADD_FIELD(float, GetPostponeFireReadyFrac, "C_CSWeaponBase", "m_flPostponeFireReadyFrac");

	[[nodiscard]] bool CanPrimaryAttack(const int nWeaponType, const float flServerTime);
	[[nodiscard]] bool CanSecondaryAttack(const int nWeaponType, const float flServerTime);
};

class C_BaseCSGrenade : public C_CSWeaponBase
{
public:
	SCHEMA_ADD_FIELD(bool, IsHeldByPlayer, "C_BaseCSGrenade", "m_bIsHeldByPlayer");
	SCHEMA_ADD_FIELD(bool, IsPinPulled, "C_BaseCSGrenade", "m_bPinPulled");
	SCHEMA_ADD_FIELD(GameTime_t, GetThrowTime, "C_BaseCSGrenade", "m_fThrowTime");
	SCHEMA_ADD_FIELD(float, GetThrowStrength, "C_BaseCSGrenade", "m_flThrowStrength");
};
class C_CSWeaponBase;
class CCSWeaponBaseVData;
class C_CSPlayerPawn;



class C_BaseGrenade : public C_BaseFlex
{
public:
	CS_CLASS_NO_INITIALIZER(C_BaseGrenade);
};

class C_PlantedC4 : public C_BaseEntity
{
public:
	CS_CLASS_NO_INITIALIZER(C_PlantedC4);

	SCHEMA_ADD_FIELD(bool, IsBombTicking, "C_PlantedC4", "m_bBombTicking");
	SCHEMA_ADD_FIELD(float, GetC4Blow, "C_PlantedC4", "m_flC4Blow");
	SCHEMA_ADD_FIELD(float, GetTimerLength, "C_PlantedC4", "m_flTimerLength");
	SCHEMA_ADD_FIELD(float, GetDefuseLength, "C_PlantedC4", "m_flDefuseLength");
	SCHEMA_ADD_FIELD(float, GetDefuseCountDown, "C_PlantedC4", "m_flDefuseCountDown");
	SCHEMA_ADD_FIELD(bool, IsBombDefused, "C_PlantedC4", "m_bBombDefused");
	SCHEMA_ADD_FIELD(CBaseHandle, GetDefuserHandle, "C_PlantedC4", "m_hBombDefuser");
	SCHEMA_ADD_FIELD(bool, HasBeenDefused, "C_PlantedC4", "m_bHasExploded");
	SCHEMA_ADD_FIELD(int, GetBombSite, "C_PlantedC4", "m_nBombSite");

	[[nodiscard]] float GetTimeRemaining(float flServerTime)
	{
		if (!IsBombTicking())
			return 0.f;
		return GetC4Blow() - flServerTime;
	}

	[[nodiscard]] float GetDefuseTimeRemaining(float flServerTime)
	{
		if (GetDefuserHandle().IsValid())
			return GetDefuseCountDown() - flServerTime;
		return 0.f;
	}

	[[nodiscard]] bool IsBeingDefused()
	{
		return GetDefuserHandle().IsValid();
	}
};