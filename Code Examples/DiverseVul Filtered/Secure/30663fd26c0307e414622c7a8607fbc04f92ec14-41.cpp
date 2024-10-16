static void gen_bndck(CPUX86State *env, DisasContext *s, int modrm,
                      TCGCond cond, TCGv_i64 bndv)
{
    TCGv ea = gen_lea_modrm_1(gen_lea_modrm_0(env, s, modrm));

    tcg_gen_extu_tl_i64(cpu_tmp1_i64, ea);
    if (!CODE64(s)) {
        tcg_gen_ext32u_i64(cpu_tmp1_i64, cpu_tmp1_i64);
    }
    tcg_gen_setcond_i64(cond, cpu_tmp1_i64, cpu_tmp1_i64, bndv);
    tcg_gen_extrl_i64_i32(cpu_tmp2_i32, cpu_tmp1_i64);
    gen_helper_bndck(cpu_env, cpu_tmp2_i32);
}