#include "samp.h"
#include "loader.h"

void stMiscInfo::toggleCursor(bool iToggle)
{
    static POINT lastPos = {-1, -1};
    void		*obj = *(void **)(g_handle.dwSAMP + SAMP_MISC_INFO);
    ((void(__thiscall *) (void *, int, bool)) (g_handle.dwSAMP + SAMP_FUNC_TOGGLECURSOR))(obj, iToggle ? 2 : 0, !iToggle);
    if (!iToggle)
    {
        GetCursorPos(&lastPos);
        ((void(__thiscall *) (void *)) (g_handle.dwSAMP + SAMP_FUNC_CURSORUNLOCKACTORCAM))(obj);
        SetCursor(LoadCursor(NULL, NULL));
    } else if (lastPos.x != -1 && lastPos.y != -1) {
        SetCursorPos(lastPos.x, lastPos.y);
    }
}

template<typename T>
T GetSAMPPtrInfo(uint32_t offset)
{
    if (g_handle.samp == nullptr)
        throw "g_handle.samp has null";
    return *(T *)(g_handle.dwSAMP + offset);
}

struct stChatInfo *stGetSampChatInfo(void)
{
    return GetSAMPPtrInfo<stChatInfo *>(SAMP_CHAT_INFO_OFFSET);
}

struct stInputInfo *stGetInputInfo(void)
{
    return GetSAMPPtrInfo<stInputInfo *>(SAMP_CHAT_INPUT_INFO_OFFSET);
}

struct stKillInfo *stGetKillInfo(void)
{
    return GetSAMPPtrInfo<stKillInfo *>(SAMP_KILL_INFO_OFFSET);
}

struct stScoreboardInfo *stGetScoreboardInfo(void)
{
    return GetSAMPPtrInfo<stScoreboardInfo *>(SAMP_SCOREBOARD_INFO);
}

stMiscInfo *stGetMiscInfo()
{
    return GetSAMPPtrInfo<stMiscInfo *>(SAMP_MISC_INFO);
}
