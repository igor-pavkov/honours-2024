static pj_uint16_t GETVAL16H(const pj_uint8_t *buf, unsigned pos)
{
    return (pj_uint16_t) ((buf[pos + 0] << 8) | \
			  (buf[pos + 1] << 0));
}