static inline TCGMemOp mo_b_d32(int b, TCGMemOp ot)
{
    return b & 1 ? (ot == MO_16 ? MO_16 : MO_32) : MO_8;
}