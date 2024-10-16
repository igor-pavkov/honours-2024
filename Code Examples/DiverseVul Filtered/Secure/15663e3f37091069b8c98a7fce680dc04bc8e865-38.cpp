static pj_status_t decode_sockaddr_attr(pj_pool_t *pool, 
				        const pj_uint8_t *buf, 
					const pj_stun_msg_hdr *msghdr, 
				        void **p_attr)
{
    pj_stun_sockaddr_attr *attr;
    int af;
    unsigned addr_len;
    pj_uint32_t val;

    PJ_CHECK_STACK();
    
    PJ_UNUSED_ARG(msghdr);

    /* Create the attribute */
    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_sockaddr_attr);
    GETATTRHDR(buf, &attr->hdr);

    /* Check that the attribute length is valid */
    if (attr->hdr.length != STUN_GENERIC_IPV4_ADDR_LEN &&
	attr->hdr.length != STUN_GENERIC_IPV6_ADDR_LEN)
    {
	return PJNATH_ESTUNINATTRLEN;
    }

    /* Check address family */
    val = *(pj_uint8_t*)(buf + ATTR_HDR_LEN + 1);

    /* Check address family is valid */
    if (val == 1) {
	if (attr->hdr.length != STUN_GENERIC_IPV4_ADDR_LEN)
	    return PJNATH_ESTUNINATTRLEN;
	af = pj_AF_INET();
	addr_len = 4;
    } else if (val == 2) {
	if (attr->hdr.length != STUN_GENERIC_IPV6_ADDR_LEN)
	    return PJNATH_ESTUNINATTRLEN;
	af = pj_AF_INET6();
	addr_len = 16;
    } else {
	/* Invalid address family */
	return PJNATH_EINVAF;
    }

    /* Get port and address */
    pj_sockaddr_init(af, &attr->sockaddr, NULL, 0);
    pj_sockaddr_set_port(&attr->sockaddr, 
			 GETVAL16H(buf, ATTR_HDR_LEN+2));
    pj_memcpy(pj_sockaddr_get_addr(&attr->sockaddr),
	      buf+ATTR_HDR_LEN+4,
	      addr_len);

    /* Done */
    *p_attr = (void*)attr;

    return PJ_SUCCESS;
}