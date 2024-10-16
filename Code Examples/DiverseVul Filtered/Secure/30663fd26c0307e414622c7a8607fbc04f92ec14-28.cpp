static void gen_extu(TCGMemOp ot, TCGv reg)
{
    gen_ext_tl(reg, reg, ot, false);
}