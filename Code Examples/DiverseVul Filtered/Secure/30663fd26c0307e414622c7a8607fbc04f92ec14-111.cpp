static void gen_ldst_modrm(CPUX86State *env, DisasContext *s, int modrm,
                           TCGMemOp ot, int reg, int is_store)
{
    int mod, rm;

    mod = (modrm >> 6) & 3;
    rm = (modrm & 7) | REX_B(s);
    if (mod == 3) {
        if (is_store) {
            if (reg != OR_TMP0)
                gen_op_mov_v_reg(ot, cpu_T0, reg);
            gen_op_mov_reg_v(ot, rm, cpu_T0);
        } else {
            gen_op_mov_v_reg(ot, cpu_T0, rm);
            if (reg != OR_TMP0)
                gen_op_mov_reg_v(ot, reg, cpu_T0);
        }
    } else {
        gen_lea_modrm(env, s, modrm);
        if (is_store) {
            if (reg != OR_TMP0)
                gen_op_mov_v_reg(ot, cpu_T0, reg);
            gen_op_st_v(s, ot, cpu_T0, cpu_A0);
        } else {
            gen_op_ld_v(s, ot, cpu_T0, cpu_A0);
            if (reg != OR_TMP0)
                gen_op_mov_reg_v(ot, reg, cpu_T0);
        }
    }
}