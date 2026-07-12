#pragma once
#include "../../dependencies/safetyhook/safetyhook.h"
#include "log.h"

template <typename T>
class CBaseHookObject
{
public:
    bool Create(void* pFunction, void* pDetour)
    {
        if (pFunction == nullptr || pDetour == nullptr)
            return false;

        pBaseFn = pFunction;
        pReplaceFn = pDetour;

        return Replace();
    }

    bool Replace()
    {
        if (pBaseFn == nullptr || bIsHooked)
            return false;

        try
        {
            hook = safetyhook::create_inline(pBaseFn, pReplaceFn);
            bIsHooked = true;
            return true;
        }
        catch (const std::exception& e)
        {
#ifdef _DEBUG
            L_PRINT(LOG_ERROR) << CS_XOR("failed to create safetyhook: ") << e.what() << CS_XOR(" at: ") << L::AddFlags(LOG_MODE_INT_SHOWBASE | LOG_MODE_INT_FORMAT_HEX) << reinterpret_cast<std::uintptr_t>(pBaseFn);
#else
            L_PRINT(LOG_ERROR) << CS_XOR("failed to enable hook");
#endif
            CS_ASSERT(false);
            return false;
        }
    }

    bool Remove()
    {
        return Restore();
    }

    bool Restore()
    {
        if (!bIsHooked)
            return false;

        hook = {};
        bIsHooked = false;
        return true;
    }

    CS_INLINE T GetOriginal()
    {
        return hook.original<T>();
    }

    CS_INLINE bool IsHooked() const
    {
        return bIsHooked;
    }

private:
    bool bIsHooked = false;
    void* pBaseFn = nullptr;
    void* pReplaceFn = nullptr;
    safetyhook::InlineHook hook;
};