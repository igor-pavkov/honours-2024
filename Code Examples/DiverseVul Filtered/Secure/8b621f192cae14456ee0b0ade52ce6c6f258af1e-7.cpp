PJ_DEF(pj_status_t) pjmedia_rtcp_build_rtcp_sdes(
					    pjmedia_rtcp_session *session, 
					    void *buf,
					    pj_size_t *length,
					    const pjmedia_rtcp_sdes *sdes)
{
    pjmedia_rtcp_common *hdr;
    pj_uint8_t *p;
    pj_size_t len;

    PJ_ASSERT_RETURN(session && buf && length && sdes, PJ_EINVAL);

    /* Verify SDES item length */
    if (sdes->cname.slen > 255 || sdes->name.slen  > 255 ||
	sdes->email.slen > 255 || sdes->phone.slen > 255 ||
	sdes->loc.slen   > 255 || sdes->tool.slen  > 255 ||
	sdes->note.slen  > 255)
    {
	return PJ_EINVAL;
    }

    /* Verify buffer length */
    len = sizeof(*hdr);
    if (sdes->cname.slen) len += sdes->cname.slen + 2;
    if (sdes->name.slen)  len += sdes->name.slen  + 2;
    if (sdes->email.slen) len += sdes->email.slen + 2;
    if (sdes->phone.slen) len += sdes->phone.slen + 2;
    if (sdes->loc.slen)   len += sdes->loc.slen   + 2;
    if (sdes->tool.slen)  len += sdes->tool.slen  + 2;
    if (sdes->note.slen)  len += sdes->note.slen  + 2;
    len++; /* null termination */
    len = ((len+3)/4) * 4;
    if (len > *length)
	return PJ_ETOOSMALL;

    /* Build RTCP SDES header */
    hdr = (pjmedia_rtcp_common*)buf;
    pj_memcpy(hdr, &session->rtcp_sr_pkt.common,  sizeof(*hdr));
    hdr->pt = RTCP_SDES;
    hdr->length = pj_htons((pj_uint16_t)(len/4 - 1));

    /* Build RTCP SDES items */
    p = (pj_uint8_t*)hdr + sizeof(*hdr);
#define BUILD_SDES_ITEM(SDES_NAME, SDES_TYPE) \
    if (sdes->SDES_NAME.slen) { \
	*p++ = SDES_TYPE; \
	*p++ = (pj_uint8_t)sdes->SDES_NAME.slen; \
	pj_memcpy(p, sdes->SDES_NAME.ptr, sdes->SDES_NAME.slen); \
	p += sdes->SDES_NAME.slen; \
    }
    BUILD_SDES_ITEM(cname, RTCP_SDES_CNAME);
    BUILD_SDES_ITEM(name,  RTCP_SDES_NAME);
    BUILD_SDES_ITEM(email, RTCP_SDES_EMAIL);
    BUILD_SDES_ITEM(phone, RTCP_SDES_PHONE);
    BUILD_SDES_ITEM(loc,   RTCP_SDES_LOC);
    BUILD_SDES_ITEM(tool,  RTCP_SDES_TOOL);
    BUILD_SDES_ITEM(note,  RTCP_SDES_NOTE);
#undef BUILD_SDES_ITEM

    /* Null termination */
    *p++ = 0;

    /* Pad to 32bit */
    while ((p-(pj_uint8_t*)buf) % 4)
	*p++ = 0;

    /* Finally */
    pj_assert((int)len == p-(pj_uint8_t*)buf);
    *length = len;

    return PJ_SUCCESS;
}