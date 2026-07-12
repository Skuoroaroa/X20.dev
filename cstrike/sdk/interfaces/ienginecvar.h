#pragma once

// used: cutllinkedlist
#include "../datatypes/utllinkedlist.h"
// used: fnv1a hashing
#include "../../utilities/fnv1a.h"

// used: sdk datatypes
#include "../datatypes/color.h"
#include "../datatypes/vector.h"
#include "../datatypes/qangle.h"

enum EConVarFlag : int
{
	FCVAR_NONE = 0,
	FCVAR_UNREGISTERED = (1 << 0),
	FCVAR_DEVELOPMENTONLY = (1 << 1),
	FCVAR_GAMEDLL = (1 << 2),
	FCVAR_CLIENTDLL = (1 << 3),
	FCVAR_HIDDEN = (1 << 4),
	FCVAR_PROTECTED = (1 << 5),
	FCVAR_SPONLY = (1 << 6),
	FCVAR_ARCHIVE = (1 << 7),
	FCVAR_NOTIFY = (1 << 8),
	FCVAR_USERINFO = (1 << 9),
	FCVAR_CHEAT = (1 << 14),
	FCVAR_PRINTABLEONLY = (1 << 10),
	FCVAR_UNLOGGED = (1 << 11),
	FCVAR_NEVER_AS_STRING = (1 << 12),
	FCVAR_REPLICATED = (1 << 13),
	FCVAR_DEMO = (1 << 16),
	FCVAR_DONTRECORD = (1 << 17),
	FCVAR_RELOAD_MATERIALS = (1 << 20),
	FCVAR_RELOAD_TEXTURES = (1 << 21),
	FCVAR_NOT_CONNECTED = (1 << 22),
	FCVAR_MATERIAL_SYSTEM_THREAD = (1 << 23),
	FCVAR_ARCHIVE_XBOX = (1 << 24),
	FCVAR_ACCESSIBLE_FROM_THREADS = (1 << 25),
	FCVAR_SERVER_CAN_EXECUTE = (1 << 28),
	FCVAR_SERVER_CANNOT_QUERY = (1 << 29),
	FCVAR_CLIENTCMD_CAN_EXECUTE = (1 << 30),
	FCVAR_MATERIAL_THREAD_MASK = (FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES | FCVAR_MATERIAL_SYSTEM_THREAD)
};

enum EConVarType : short
{
	EConVarType_Invalid = -1,
	EConVarType_Bool,
	EConVarType_Int16,
	EConVarType_UInt16,
	EConVarType_Int32,
	EConVarType_UInt32,
	EConVarType_Int64,
	EConVarType_UInt64,
	EConVarType_Float32,
	EConVarType_Float64,
	EConVarType_String,
	EConVarType_Color,
	EConVarType_Vector2,
	EConVarType_Vector3,
	EConVarType_Vector4,
	EConVarType_Qangle,
	EConVarType_MAX
};

union CVValue_t
{
	bool i1;
	short i16;
	uint16_t u16;
	int i32;
	uint32_t u32;
	int64_t i64;
	uint64_t u64;
	float fl;
	double db;
	const char* sz;
	Color_t clr;
	Vector2D_t vec2;
	Vector_t vec3;
	Vector4D_t vec4;
	QAngle_t ang;
};

class CConVar
{
public:
	const char* szName;
	CConVar* m_pNext;
	MEM_PAD(0x10);
	const char* szDescription;
	uint32_t nType;
	uint32_t nRegistered;
	uint32_t nFlags;
	MEM_PAD(0xC);
	CVValue_t value;
};

class IEngineCVar
{
public:
	MEM_PAD(0x40);
	CUtlLinkedList<CConVar*> listConvars;

	CConVar* Find(FNV1A_t uHashedName)
	{
		for (auto i = I::Cvar->listConvars.m_Head; i != I::Cvar->listConvars.kInvalidIndex;) {

			CConVar* pConVar = I::Cvar->listConvars.memory[i].m_Element;
			i = I::Cvar->listConvars.memory[i].m_Next;

			if (pConVar == nullptr)
				continue;

			if (FNV1A::Hash(pConVar->szName) == uHashedName)
				return pConVar;

		}

		return nullptr;
	}

	void UnlockHiddenCVars()
	{
		for (auto i = I::Cvar->listConvars.m_Head; i != I::Cvar->listConvars.kInvalidIndex;) {

			CConVar* pConVar = I::Cvar->listConvars.memory[i].m_Element;
			i = I::Cvar->listConvars.memory[i].m_Next;

			if (pConVar == nullptr)
				continue;

			if (pConVar->nFlags & FCVAR_HIDDEN)
				pConVar->nFlags &= ~FCVAR_HIDDEN;

			if (pConVar->nFlags & FCVAR_DEVELOPMENTONLY)
				pConVar->nFlags &= ~FCVAR_DEVELOPMENTONLY;
		}
	}
};