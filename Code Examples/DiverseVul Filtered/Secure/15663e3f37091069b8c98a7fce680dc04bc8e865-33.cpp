static pj_status_t encode_sockaddr_attr(const void *a, pj_uint8_t *buf, 
				        unsigned len, 
					const pj_stun_msg_hdr *msghdr,
					unsigned *printed)
{
    pj_uint8_t *start_buf = buf;
    const pj_stun_sockaddr_attr *ca = 
	(const pj_stun_sockaddr_attr *)a;

    PJ_CHECK_STACK();
    
    /* Common: attribute type */
    PUTVAL16H(buf, 0, ca->hdr.type);

    if (ca->sockaddr.addr.sa_family == pj_AF_INET()) {
	enum {
	    ATTR_LEN = ATTR_HDR_LEN + STUN_GENERIC_IPV4_ADDR_LEN
	};

	if (len < ATTR_LEN) 
	    return PJ_ETOOSMALL;

	/* attribute len */
	PUTVAL16H(buf, 2, STUN_GENERIC_IPV4_ADDR_LEN);
	buf += ATTR_HDR_LEN;
    
	/* Ignored */
	*buf++ = '\0';

	/* Address family, 1 for IPv4 */
	*buf++ = 1;

	/* IPv4 address */
	if (ca->xor_ed) {
	    pj_uint32_t addr;
	    pj_uint16_t port;

	    addr = ca->sockaddr.ipv4.sin_addr.s_addr;
	    port = ca->sockaddr.ipv4.sin_port;

	    port ^= pj_htons(PJ_STUN_MAGIC >> 16);
	    addr ^= pj_htonl(PJ_STUN_MAGIC);

	    /* Port */
	    pj_memcpy(buf, &port, 2);
	    buf += 2;

	    /* Address */
	    pj_memcpy(buf, &addr, 4);
	    buf += 4;

	} else {
	    /* Port */
	    pj_memcpy(buf, &ca->sockaddr.ipv4.sin_port, 2);
	    buf += 2;

	    /* Address */
	    pj_memcpy(buf, &ca->sockaddr.ipv4.sin_addr, 4);
	    buf += 4;
	}

	pj_assert(buf - start_buf == ATTR_LEN);

    } else if (ca->sockaddr.addr.sa_family == pj_AF_INET6()) {
	/* IPv6 address */
	enum {
	    ATTR_LEN = ATTR_HDR_LEN + STUN_GENERIC_IPV6_ADDR_LEN
	};

	if (len < ATTR_LEN) 
	    return PJ_ETOOSMALL;

	/* attribute len */
	PUTVAL16H(buf, 2, STUN_GENERIC_IPV6_ADDR_LEN);
	buf += ATTR_HDR_LEN;
    
	/* Ignored */
	*buf++ = '\0';

	/* Address family, 2 for IPv6 */
	*buf++ = 2;

	/* IPv6 address */
	if (ca->xor_ed) {
	    unsigned i;
	    pj_uint8_t *dst;
	    const pj_uint8_t *src;
	    pj_uint32_t magic = pj_htonl(PJ_STUN_MAGIC);
	    pj_uint16_t port = ca->sockaddr.ipv6.sin6_port;

	    /* Port */
	    port ^= pj_htons(PJ_STUN_MAGIC >> 16);
	    pj_memcpy(buf, &port, 2);
	    buf += 2;

	    /* Address */
	    dst = buf;
	    src = (const pj_uint8_t*) &ca->sockaddr.ipv6.sin6_addr;
	    for (i=0; i<4; ++i) {
		dst[i] = (pj_uint8_t)(src[i] ^ ((const pj_uint8_t*)&magic)[i]);
	    }
	    pj_assert(sizeof(msghdr->tsx_id[0]) == 1);
	    for (i=0; i<12; ++i) {
		dst[i+4] = (pj_uint8_t)(src[i+4] ^ msghdr->tsx_id[i]);
	    }

	    buf += 16;

	} else {
	    /* Port */
	    pj_memcpy(buf, &ca->sockaddr.ipv6.sin6_port, 2);
	    buf += 2;

	    /* Address */
	    pj_memcpy(buf, &ca->sockaddr.ipv6.sin6_addr, 16);
	    buf += 16;
	}

	pj_assert(buf - start_buf == ATTR_LEN);

    } else {
	return PJNATH_EINVAF;
    }

    /* Done */
    *printed = (unsigned)(buf - start_buf);

    return PJ_SUCCESS;
}