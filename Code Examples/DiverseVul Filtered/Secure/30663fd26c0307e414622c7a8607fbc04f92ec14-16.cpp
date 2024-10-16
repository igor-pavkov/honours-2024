static inline bool byte_reg_is_xH(int reg)
{
    if (reg < 4) {
        return false;
    }
#ifdef TARGET_X86_64
    if (reg >= 8 || x86_64_hregs) {
        return false;
    }
#endif
    return true;
}