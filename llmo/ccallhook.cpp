#include "ccallhook.h"

CCallHook::CCallHook(void *addr, eSafeCall save, uint size, eCodePos pos)
{
    hook_addr = addr;
    _size = size;
    hook_pos = pos;
    m_shortAsm = new CShortAsm();
    _func = nullptr;

    orig_bytes = new byte[size + 1];
    memsafe::copy(orig_bytes, addr, size);

    if (pos == cp_before){
        if (orig_bytes[0] == 0xE9)
            memsafe::set(orig_bytes, 0x90, 5);
        else ModOriginalBytes(reinterpret_cast<uint>(m_shortAsm->getAddr()) + m_shortAsm->getWriteOffset());
        m_shortAsm->insert(orig_bytes, size);
    }
    if (checkFlag(save, sc_registers))
        m_shortAsm->pushad();
    if (checkFlag(save, sc_flags))
        m_shortAsm->pushfd();

    hook_offset = m_shortAsm->getWriteOffset();
    if (pos == cp_skip)
        ModOriginalBytes(reinterpret_cast<uint>(m_shortAsm->getAddr()) + m_shortAsm->getWriteOffset());
    disable();

    if (checkFlag(save, sc_flags))
        m_shortAsm->popfd();
    if (checkFlag(save, sc_registers))
        m_shortAsm->popad();
    if (pos == cp_after){
        ModOriginalBytes(reinterpret_cast<uint>(m_shortAsm->getAddr()) + m_shortAsm->getWriteOffset());
        m_shortAsm->insert(orig_bytes, size);
    }

    m_shortAsm->jmp(reinterpret_cast<int>(addr) + static_cast<int>(size));

    memsafe::set(addr, 0x90, size);
    byteValue<uint> v;
    v.value = reinterpret_cast<uint>(m_shortAsm->getAddr()) - (reinterpret_cast<uint>(addr) + 5);
    memsafe::set(addr, 0xE9, 1);
    memsafe::set(reinterpret_cast<void*>(reinterpret_cast<uint>(addr) + 1), v.bytes[0], 1);
    memsafe::set(reinterpret_cast<void*>(reinterpret_cast<uint>(addr) + 2), v.bytes[1], 1);
    memsafe::set(reinterpret_cast<void*>(reinterpret_cast<uint>(addr) + 3), v.bytes[2], 1);
    memsafe::set(reinterpret_cast<void*>(reinterpret_cast<uint>(addr) + 4), v.bytes[3], 1);
}

CCallHook::~CCallHook()
{
    disable();
    //delete m_shortAsm;
}

void CCallHook::setFunc(void(__stdcall*func)())
{
    _func = func;
}

void CCallHook::enable(void(__stdcall*func)())
{
    setNops();
    if (func == nullptr){
        if (hook_pos == cp_skip){
            m_shortAsm->setWriteOffset(hook_offset);
            m_shortAsm->insert(orig_bytes, _size);
        }
        return;
    }
    m_shortAsm->setWriteOffset(hook_offset);
    m_shortAsm->call(reinterpret_cast<int>(func));
    _func = func;
}

void CCallHook::enable()
{
    return enable(_func);
}

void CCallHook::disable()
{
    m_shortAsm->setWriteOffset(hook_offset);
    if (hook_pos == cp_skip)
        m_shortAsm->insert(orig_bytes, _size);
    else setNops();
}

template<typename T> inline
bool CCallHook::checkFlag(T value, T flag )
{
    return (value & flag);
}

void CCallHook::setNops()
{
    m_shortAsm->setWriteOffset(hook_offset);
    for (int i = 0; i < static_cast<int>(_size); ++i)
        m_shortAsm->nop();
}

void CCallHook::ModOriginalBytes(uint offset)
{
    // call and jmp (long)
    if (orig_bytes[0] == 0xE9 || orig_bytes[0] == 0xE8){
        byteValue<uint> v;
        v.bytes[0] = orig_bytes[1];
        v.bytes[1] = orig_bytes[2];
        v.bytes[2] = orig_bytes[3];
        v.bytes[3] = orig_bytes[4];
        uint o_addr = v.value + (reinterpret_cast<uint>(hook_addr) + 5);
        v.value = o_addr - (offset + 5);
        orig_bytes[1] = v.bytes[0];
        orig_bytes[2] = v.bytes[1];
        orig_bytes[3] = v.bytes[2];
        orig_bytes[4] = v.bytes[3];
    }
    // conditionals jmp's (long)
    else if (orig_bytes[0] == 0x0F) {
        if (orig_bytes[1] >= 0x81 && orig_bytes[1] <= 0x8F){
            byteValue<uint> v;
            v.bytes[0] = orig_bytes[2];
            v.bytes[1] = orig_bytes[3];
            v.bytes[2] = orig_bytes[4];
            v.bytes[3] = orig_bytes[5];
            uint o_addr = v.value + (reinterpret_cast<uint>(hook_addr) + 6);
            v.value = o_addr - (offset + 6);
            orig_bytes[2] = v.bytes[0];
            orig_bytes[3] = v.bytes[1];
            orig_bytes[4] = v.bytes[2];
            orig_bytes[5] = v.bytes[3];
        }
    }
}

