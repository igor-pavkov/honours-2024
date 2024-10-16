static pj_status_t encode_string_attr(const void *a, pj_uint8_t *buf, 
				      unsigned len, 
				      const pj_stun_msg_hdr *msghdr,
				      unsigned *printed)
{
    const pj_stun_string_attr *ca = 
	(const pj_stun_string_attr*)a;

    PJ_CHECK_STACK();
    
    PJ_UNUSED_ARG(msghdr);

    /* Calculated total attr_len (add padding if necessary) */
    *printed = ((unsigned)ca->value.slen + ATTR_HDR_LEN + 3) & (~3);
    if (len < *printed) {
	*printed = 0;
	return PJ_ETOOSMALL;
    }

    PUTVAL16H(buf, 0, ca->hdr.type);

    /* Special treatment for SOFTWARE attribute:
     * This attribute had caused interop problem when talking to 
     * legacy RFC 3489 STUN servers, due to different "length"
     * rules with RFC 5389.
     */
    if (msghdr->magic != PJ_STUN_MAGIC ||
	ca->hdr.type == PJ_STUN_ATTR_SOFTWARE)
    {
	/* Set the length to be 4-bytes aligned so that we can
	 * communicate with RFC 3489 endpoints
	 */
	PUTVAL16H(buf, 2, (pj_uint16_t)((ca->value.slen + 3) & (~3)));
    } else {
	/* Use RFC 5389 rule */
	PUTVAL16H(buf, 2, (pj_uint16_t)ca->value.slen);
    }

    /* Copy the string */
    pj_memcpy(buf+ATTR_HDR_LEN, ca->value.ptr, ca->value.slen);

    /* Add padding character, if string is not 4-bytes aligned. */
    if (ca->value.slen & 0x03) {
	pj_uint8_t pad[3];
	pj_memset(pad, padding_char, sizeof(pad));
	pj_memcpy(buf+ATTR_HDR_LEN+ca->value.slen, pad,
		  4-(ca->value.slen & 0x03));
    }

    /* Done */
    return PJ_SUCCESS;
}