static void PUTVAL64H(pj_uint8_t *buf, unsigned pos, const pj_timestamp *ts)
{
    PUTVAL32H(buf, pos, ts->u32.hi);
    PUTVAL32H(buf, pos+4, ts->u32.lo);
}