// https://www.unknowncheats.me/forum/counter-strike-2-a/747109-enginetrace-traceshape-release.html good
// used: game trace manager
#include "cgametracemanager.h"
// used: c_csplayerpawn
#include "../../sdk/entity.h"
#include "../../sdk/datatypes/vector.h"

#include "../../utilities/memory.h"
typedef void(__fastcall* InitTraceData_t)(TraceData_t*);
typedef void(__fastcall* InitTraceInfo_t)(CGameTrace*);
typedef void* (__fastcall* InitTraceFilter_t)(CTraceFilter* pFilter, uintptr_t pPawn, uint64_t mask, int traceType, int filterType);
typedef bool(__fastcall* CreateTrace_t)(TraceData_t* trace, Vector_t start, Vector_t delta, CTraceFilter* filter, int penetration_count, bool unk);
typedef void(__fastcall* GetTraceInfo_t)(TraceData_t* trace, CGameTrace* hit, float unknown_float, void* unknown);
typedef bool(__fastcall* HandleBulletPen_t)(TraceData_t* trace, handle_bullet_data_t* bullet, bullet_modulate_entry_t* entry, int team_num, void* impact_debug);

static InitTraceData_t fnInitTraceData = nullptr;
static InitTraceInfo_t fnInitTraceInfo = nullptr;
static InitTraceFilter_t fnInitFilter = nullptr;
static CreateTrace_t fnCreateTrace = nullptr;
static GetTraceInfo_t fnGetTraceInfo = nullptr;
static HandleBulletPen_t fnHandleBulletPenetration = nullptr;

void EngineTrace::Init() {

    fnInitTraceData = (InitTraceData_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8D 79 ? 33 F6 C7 47"));
    fnInitTraceInfo = (InitTraceInfo_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("40 55 41 55 41 57 48 83 EC 30"));
    fnInitFilter = (InitTraceFilter_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 0F B6 41 ? 33 FF 24"));
    fnCreateTrace = (CreateTrace_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 50 F2 0F 10 02"));
    fnGetTraceInfo = (GetTraceInfo_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 60 48 8B E9 0F 29 74 24"));
    fnHandleBulletPenetration = (HandleBulletPen_t)MEM::FindPattern(CLIENT_DLL, CS_XOR("48 8B C4 44 89 48 ? 48 89 50 ? 48 89 48 ? 55 57"));
}


bool EngineTrace::TraceLine(const Vector_t& start, const Vector_t& end, CGameTrace* outTrace, uintptr_t skipPawnPtr, uint64_t mask) {
    if (!fnInitTraceData || !fnInitTraceInfo || !fnInitFilter || !fnCreateTrace || !fnGetTraceInfo) {
        if (outTrace) outTrace->fraction = 1.0f;
        return false;
    }

    TraceData_t td{};
    fnInitTraceData(&td);

    CTraceFilter filter{};
    fnInitFilter(&filter, skipPawnPtr, mask, 4, 7);

    Vector_t delta = { end.x - start.x, end.y - start.y, end.z - start.z };
    fnCreateTrace(&td, start, delta, &filter, 4, true);

    fnInitTraceInfo(outTrace);

    auto& arr = td.mod_array;
    if (arr.size > 0 && arr.data != nullptr) {
        bullet_modulate_entry_t* entry = &arr.data[0];

        uint16_t surf_idx = entry->surfEnd & 0x7FFF;

        if (surf_idx < 0x80) {
            trace_array_element_t* surf = &td.m_Arr[surf_idx];

            fnGetTraceInfo(&td, outTrace, entry->startFrac, surf);
            return true;
        }
    }

    outTrace->fraction = 1.0f;
    return false;
}


// Добавьте это в конец файла cgametracemanager.cpp

bool EngineTrace::FireBullet(FireBulletData_t& mData, uintptr_t pLocalPawn, C_CSWeaponBase* pWeapon, CCSWeaponBaseVData* pWeaponData)
{
    if (!mData.pTargetPawn || !pLocalPawn || !pWeapon || !pWeaponData)
        return false;

    // Get weapon stats
    float flWeaponDamage = static_cast<float>(pWeaponData->GetDamage());
    float flWeaponPenetration = pWeaponData->GetPenetration();
    float flWeaponRangeModifier = pWeaponData->GetRangeModifier();
    float flWeaponRange = pWeaponData->GetRange();
    int iMaxPenetration = 4;

    // Calculate direction
    Vector_t vecDirection = mData.vecEndPos - mData.vecStartPos;
    float flDistanceToTarget = vecDirection.Length();
    if (flDistanceToTarget < 1.0f)
        return false;

    vecDirection = vecDirection / flDistanceToTarget;

    // Trace to max range
    Vector_t vecEndPosition = mData.vecStartPos + (vecDirection * flWeaponRange);

    // Setup trace data
    TraceData_t td{};
    if (!fnInitTraceData)
        return false;
    fnInitTraceData(&td);

    // Setup filter
    alignas(16) CTraceFilter filter;
    if (!fnInitFilter)
        return false;
    fnInitFilter(&filter, pLocalPawn, 0x1C300B, 4, 7);

    // Create trace
    if (!fnCreateTrace)
        return false;
    Vector_t delta = { vecEndPosition.x - mData.vecStartPos.x,
                       vecEndPosition.y - mData.vecStartPos.y,
                       vecEndPosition.z - mData.vecStartPos.z };
    fnCreateTrace(&td, mData.vecStartPos, delta, &filter, iMaxPenetration, true);

    // Setup bullet penetration data
    handle_bullet_data_t bullet{};
    bullet.damage = flWeaponDamage;
    bullet.penetration = flWeaponPenetration;
    bullet.rangeModifier = flWeaponRangeModifier;
    bullet.penetrationCount = iMaxPenetration;
    bullet.penetrationStopped = false;

    mData.flDamage = flWeaponDamage;
    mData.flCurrentDistance = 0.f;

    auto& arr = td.mod_array;
    for (int i = 0; i < arr.size; ++i)
    {
        if (!arr.data)
            break;

        bullet_modulate_entry_t* entry = &arr.data[i];

        uint16_t surf_idx = entry->surfEnd & 0x7FFF;
        if (surf_idx >= 0x80)
            break;

        trace_array_element_t* surf = &td.m_Arr[surf_idx];

        alignas(16) CGameTrace tr {};
        if (fnInitTraceInfo)
            fnInitTraceInfo(&tr);
        if (fnGetTraceInfo)
            fnGetTraceInfo(&td, &tr, entry->startFrac, surf);

        float flTraceDistance = flWeaponRange - mData.flCurrentDistance;
        float flFraction = tr.fraction;

        if (flFraction >= 1.0f)
            break;

        float flTravelDist = flTraceDistance * flFraction;
        mData.flCurrentDistance += flTravelDist;

        // Apply distance damage modifier
        mData.flDamage *= std::powf(flWeaponRangeModifier, mData.flCurrentDistance / 500.f);

        // Check if we hit the target
        if (tr.m_pHitEntity && reinterpret_cast<uintptr_t>(tr.m_pHitEntity) == reinterpret_cast<uintptr_t>(mData.pTargetPawn))
        {
            // Apply hitgroup damage scaling (simplified)
            // You can add more accurate hitgroup scaling here
            mData.bHitValid = mData.flDamage >= 1.0f;
            return mData.bHitValid;
        }

        // Handle penetration
        if (fnHandleBulletPenetration)
        {
            if (fnHandleBulletPenetration(&td, &bullet, entry, 3, nullptr))
            {
                // Penetration failed
                return false;
            }
        }

        mData.flDamage = bullet.damage;

        if (bullet.penetrationCount <= 0 || bullet.penetrationStopped)
            return false;
    }

    return false;
}
static float CalculateDistanceDamageModifier(float distance, float rangeModifier) {
    return std::pow(rangeModifier, distance / 500.0f);
}
