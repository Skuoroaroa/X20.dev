#pragma once

/// used: utlthash
#include "../datatypes/utlthash.h"
// used: utlvector
#include "../datatypes/utlvector.h"
#include "../../utilities/memory.h"

struct CSchemaClassField {
    const char* Name;
    void* Type;
    uint32_t  Offset;
    uint32_t MetadataSize;
    void* Metadata;
};

class CSchemaClassInfo {
public:
    void* ptr;
    const char* Name;
    const char* ModuleName;
    char pad0[0x8];
    uint32_t Size;
    uint16_t NumFields;
    char pad1[0x2];
    uint16_t StaticSize;
    uint16_t MetadataSize;
    char pad2[0x4];
    CSchemaClassField* Fields;

    const char* GetName() {
        return *reinterpret_cast<const char**>((unsigned __int64)(this) + 0x8);
    };
};

struct CSchemaDeclaredClass {
    void* ptr;
    const char* Name;
    const char* ModuleName;
    const char* UnkStr;
    CSchemaClassInfo* mClass;
};

struct CSchemaDeclaredClassEntry {
    uint64_t Hash[2];
    CSchemaDeclaredClass* mDeclaredClass;
};

struct CSchemaTypeScope {
    void* ptr;
    char Name[256];
    char pad1[0x368];
    int16_t NumDeclaredClasses;
    int16_t UnkCount;
    int32_t UnkFlags;
    CSchemaDeclaredClassEntry* mDeclaredClasses;
};

class ISchemaSystem {
public:
    CSchemaTypeScope* FindTypeScopeForModule(const char* name) {
        return MEM::CallVirtual<CSchemaTypeScope*>(this, 13, name, nullptr);
    };
};