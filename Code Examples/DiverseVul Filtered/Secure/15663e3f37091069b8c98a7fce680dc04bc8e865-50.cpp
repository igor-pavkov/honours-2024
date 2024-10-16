static void GETATTRHDR(const pj_uint8_t *buf, pj_stun_attr_hdr *hdr)
{
    hdr->type = GETVAL16H(buf, 0);
    hdr->length = GETVAL16H(buf, 2);
}