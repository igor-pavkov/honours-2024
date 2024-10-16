static inline hwaddr pcnet_rdra_addr(PCNetState *s, int idx)
{
    while (idx < 1) {
        idx += CSR_RCVRL(s);
    }
    return s->rdra + ((CSR_RCVRL(s) - idx) * (BCR_SWSTYLE(s) ? 16 : 8));
}