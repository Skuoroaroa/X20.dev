// https://www.unknowncheats.me/forum/counter-strike-2-a/747109-enginetrace-traceshape-release.html good
#pragma once
// used: pad and findpattern
#include "../../utilities/memory.h"
// used: vector
#include "../../sdk/datatypes/vector.h"
// used: array
#include <array>

// used: game trace manager
#include <cstdint>
// used: c_csplayerpawn
#include "../../sdk/entity.h"
struct alignas(16) CGameTrace {
    void* m_Surface;
    void* m_pHitEntity;
    void* m_HitboxData;
    char _pad1[0x10];
    uint32_t m_Contents;
    char _pad2[0x4A];
    Vector_t m_StartPos;
    Vector_t m_EndPos;
    Vector_t m_Normal;
    Vector_t m_Pos;
    char _pad3[0x4];
    float fraction;
    char _pad[0xC];
    char _pad4[0x6];
    bool m_all_solid;
    char _pad5[0x4D];
};

struct trace_array_element_t { char data[0x30]; };
struct bullet_modulate_entry_t {
    float startFrac; float endFrac; float damage; int maxSecondaryTraces;
    uint16_t surfStart; uint16_t surfEnd; uint8_t flags; uint8_t pad[3];
};
struct bullet_mod_array_t {
    int size; char pad4[4]; bullet_modulate_entry_t* data; char pad16[8];
};

struct alignas(16) TraceData_t {
    char pad0[24];
    trace_array_element_t m_Arr[0x80];
    char pad6168[8];
    bullet_mod_array_t mod_array;
    bullet_modulate_entry_t mod_inline[8];
    Vector_t tail_start;
    Vector_t tail_end;
    char _pad6200[12];
};

struct alignas(16) CTraceFilter {
    char pad[164];
};

struct handle_bullet_data_t {
    float damage;           // +0
    float penetration;      // +4
    float rangeModifier;    // +8
    float tailLength;       // +12
    int penetrationCount;   // +16 
    bool penetrationStopped;// +20 
    char pad[3];
};
struct FireBulletData_t
{
    C_CSPlayerPawn* pTargetPawn = nullptr;
    Vector_t vecStartPos = {};
    Vector_t vecEndPos = {};
    float flDamage = 0.f;
    float flCurrentDistance = 0.f;
    int iPenetrationCount = 4;
    bool bHitValid = false;
};
namespace EngineTrace {
    void Init();
    bool TraceLine(
        const Vector_t& start,
        const Vector_t& end,
        CGameTrace* outTrace,
        uintptr_t skipPawnPtr = 0,
        uint64_t mask = 0x1C3003
    );

    bool FireBullet(FireBulletData_t& mData, uintptr_t pLocalPawn, C_CSWeaponBase* pWeapon, CCSWeaponBaseVData* pWeaponData);
   
}