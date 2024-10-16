static pj_status_t encode_errcode_attr(const void *a, pj_uint8_t *buf, 
				       unsigned len, 
				       const pj_stun_msg_hdr *msghdr,
				       unsigned *printed)
{
    const pj_stun_errcode_attr *ca = 
	(const pj_stun_errcode_attr*)a;

    PJ_CHECK_STACK();
    
    PJ_UNUSED_ARG(msghdr);

    if (len < ATTR_HDR_LEN + 4 + (unsigned)ca->reason.slen) 
	return PJ_ETOOSMALL;

    /* Copy and convert attribute to network byte order */
    PUTVAL16H(buf, 0, ca->hdr.type);
    PUTVAL16H(buf, 2, (pj_uint16_t)(4 + ca->reason.slen));
    PUTVAL16H(buf, 4, 0);
    buf[6] = (pj_uint8_t)(ca->err_code / 100);
    buf[7] = (pj_uint8_t)(ca->err_code % 100);

    /* Copy error string */
    pj_memcpy(buf + ATTR_HDR_LEN + 4, ca->reason.ptr, ca->reason.slen);

    /* Done */
    *printed = (ATTR_HDR_LEN + 4 + (unsigned)ca->reason.slen + 3) & (~3);

    return PJ_SUCCESS;
}