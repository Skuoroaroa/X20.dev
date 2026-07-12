#pragma once

// used: schema field
#include "../../utilities/memory.h"

#include "../entity_handle.h"

#define MAX_ENTITIES_IN_LIST 512
#define MAX_ENTITY_LISTS 64 // 0x3F
#define MAX_TOTAL_ENTITIES MAX_ENTITIES_IN_LIST* MAX_ENTITY_LISTS

class C_BaseEntity;


class CGameEntitySystem
{
public:
	
	/// GetClientEntity
	template <typename T = C_BaseEntity>
	T* Get(int nIndex)
	{
		return reinterpret_cast<T*>(this->GetEntityByIndex(nIndex));
	}

	/// GetClientEntityFromHandle
	template <typename T = C_BaseEntity>
	T* Get(const CBaseHandle hHandle)
	{
		if (!hHandle.IsValid())
			return nullptr;

		return reinterpret_cast<T*>(this->GetEntityByIndex(hHandle.GetEntryIndex()));
	}

	int GetHighestEntityIndex()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + 0x2090);
	}

private:
	void* GetEntityByIndex(int nIndex)
	{
		using fnGetBaseEntity = void* (CS_THISCALL*)(void*, int);
		static auto GetBaseEntity = reinterpret_cast<fnGetBaseEntity>(MEM::FindPattern(CLIENT_DLL, CS_XOR("4C 8D 49 10 81 FA ? ? ? ?")));
		return GetBaseEntity(this, nIndex);
	}
};
