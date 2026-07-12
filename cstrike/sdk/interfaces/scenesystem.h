#pragma once
#include "../datatypes/qangle.h"
#include "../datatypes/matrix.h"

struct CSceneLightDesc
{
    char pad0[4];
    float colorR;
    float colorG;
    float colorB;
    float backup_colorR;
    float backup_colorG;
    float backup_colorB;
    char pad18[33];
    bool shadows;
    char pad3E[54];
    std::uint8_t flags;
    bool enabled;
    char pad76[44];
    Vector4D_t vDirection;
};

struct CSceneLightObject
{
    uint64_t vtable;
    char pad8[216];
    CSceneLightDesc desc;
    char pad460[944];
};