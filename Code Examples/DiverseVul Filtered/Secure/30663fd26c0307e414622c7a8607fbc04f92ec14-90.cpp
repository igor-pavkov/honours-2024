static void gen_exts(TCGMemOp ot, TCGv reg)
{
    gen_ext_tl(reg, reg, ot, true);
}