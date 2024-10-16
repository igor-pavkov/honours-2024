static pj_status_t encode_uint_attr(const void *a, pj_uint8_t *buf, 
				    unsigned len, 
				    const pj_stun_msg_hdr *msghdr,
				    unsigned *printed)
{
    const pj_stun_uint_attr *ca = (const pj_stun_uint_attr*)a;

    PJ_CHECK_STACK();

    PJ_UNUSED_ARG(msghdr);
    
    if (len < 8) 
	return PJ_ETOOSMALL;

    PUTVAL16H(buf, 0, ca->hdr.type);
    PUTVAL16H(buf, 2, (pj_uint16_t)4);
    PUTVAL32H(buf, 4, ca->value);
    
    /* Done */
    *printed = 8;

    return PJ_SUCCESS;
}