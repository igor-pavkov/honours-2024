static inline TCGMemOp mo_b_d(int b, TCGMemOp ot)
{
    return b & 1 ? ot : MO_8;
}