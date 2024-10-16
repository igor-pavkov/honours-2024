static TCGv gen_lea_modrm_1(AddressParts a)
{
    TCGv ea;

    TCGV_UNUSED(ea);
    if (a.index >= 0) {
        if (a.scale == 0) {
            ea = cpu_regs[a.index];
        } else {
            tcg_gen_shli_tl(cpu_A0, cpu_regs[a.index], a.scale);
            ea = cpu_A0;
        }
        if (a.base >= 0) {
            tcg_gen_add_tl(cpu_A0, ea, cpu_regs[a.base]);
            ea = cpu_A0;
        }
    } else if (a.base >= 0) {
        ea = cpu_regs[a.base];
    }
    if (TCGV_IS_UNUSED(ea)) {
        tcg_gen_movi_tl(cpu_A0, a.disp);
        ea = cpu_A0;
    } else if (a.disp != 0) {
        tcg_gen_addi_tl(cpu_A0, ea, a.disp);
        ea = cpu_A0;
    }

    return ea;
}