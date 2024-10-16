static pj_status_t encode_empty_attr(const void *a, pj_uint8_t *buf, 
				     unsigned len, 
				     const pj_stun_msg_hdr *msghdr,
				     unsigned *printed)
{
    const pj_stun_empty_attr *ca = (pj_stun_empty_attr*)a;

    PJ_UNUSED_ARG(msghdr);

    if (len < ATTR_HDR_LEN) 
	return PJ_ETOOSMALL;

    PUTVAL16H(buf, 0, ca->hdr.type);
    PUTVAL16H(buf, 2, 0);

    /* Done */
    *printed = ATTR_HDR_LEN;

    return PJ_SUCCESS;
}