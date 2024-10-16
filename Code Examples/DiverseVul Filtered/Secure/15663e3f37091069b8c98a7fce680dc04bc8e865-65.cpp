static pj_status_t decode_uint64_attr(pj_pool_t *pool, 
				      const pj_uint8_t *buf, 
				      const pj_stun_msg_hdr *msghdr, 
				      void **p_attr)
{
    pj_stun_uint64_attr *attr;

    PJ_UNUSED_ARG(msghdr);

    /* Create the attribute */
    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_uint64_attr);
    GETATTRHDR(buf, &attr->hdr);

    if (attr->hdr.length != 8)
	return PJNATH_ESTUNINATTRLEN;

    GETVAL64H(buf, 4, &attr->value);	

    /* Done */
    *p_attr = attr;

    return PJ_SUCCESS;
}