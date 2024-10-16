static pj_status_t decode_xored_sockaddr_attr(pj_pool_t *pool, 
					      const pj_uint8_t *buf, 
					      const pj_stun_msg_hdr *msghdr, 
					      void **p_attr)
{
    pj_stun_sockaddr_attr *attr;
    pj_status_t status;

    status = decode_sockaddr_attr(pool, buf, msghdr, p_attr);
    if (status != PJ_SUCCESS)
	return status;

    attr = *(pj_stun_sockaddr_attr**)p_attr;

    attr->xor_ed = PJ_TRUE;

    if (attr->sockaddr.addr.sa_family == pj_AF_INET()) {
	attr->sockaddr.ipv4.sin_port ^= pj_htons(PJ_STUN_MAGIC >> 16);
	attr->sockaddr.ipv4.sin_addr.s_addr ^= pj_htonl(PJ_STUN_MAGIC);
    } else if (attr->sockaddr.addr.sa_family == pj_AF_INET6()) {
	unsigned i;
	pj_uint8_t *dst = (pj_uint8_t*) &attr->sockaddr.ipv6.sin6_addr;
	pj_uint32_t magic = pj_htonl(PJ_STUN_MAGIC);

	attr->sockaddr.ipv6.sin6_port ^= pj_htons(PJ_STUN_MAGIC >> 16);

	/* If the IP address family is IPv6, X-Address is computed by
	 * taking the mapped IP address in host byte order, XOR'ing it
	 * with the concatenation of the magic cookie and the 96-bit 
	 * transaction ID, and converting the result to network byte 
	 * order.
	 */
	for (i=0; i<4; ++i) {
	    dst[i] ^= ((const pj_uint8_t*)&magic)[i];
	}
	pj_assert(sizeof(msghdr->tsx_id[0]) == 1);
	for (i=0; i<12; ++i) {
	    dst[i+4] ^= msghdr->tsx_id[i];
	}

    } else {
	return PJNATH_EINVAF;
    }

    /* Done */
    *p_attr = attr;

    return PJ_SUCCESS;
}