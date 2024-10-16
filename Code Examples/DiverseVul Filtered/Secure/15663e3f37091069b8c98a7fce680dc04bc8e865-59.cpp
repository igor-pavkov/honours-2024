static void PUTVAL32H(pj_uint8_t *buf, unsigned pos, pj_uint32_t hval)
{
    buf[pos+0] = (pj_uint8_t) ((hval & 0xFF000000UL) >> 24);
    buf[pos+1] = (pj_uint8_t) ((hval & 0x00FF0000UL) >> 16);
    buf[pos+2] = (pj_uint8_t) ((hval & 0x0000FF00UL) >>  8);
    buf[pos+3] = (pj_uint8_t) ((hval & 0x000000FFUL) >>  0);
}