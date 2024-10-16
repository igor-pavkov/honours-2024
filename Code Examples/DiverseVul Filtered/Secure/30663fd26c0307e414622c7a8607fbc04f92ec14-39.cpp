static void gen_lea_modrm(CPUX86State *env, DisasContext *s, int modrm)
{
    AddressParts a = gen_lea_modrm_0(env, s, modrm);
    TCGv ea = gen_lea_modrm_1(a);
    gen_lea_v_seg(s, s->aflag, ea, a.def_seg, s->override);
}