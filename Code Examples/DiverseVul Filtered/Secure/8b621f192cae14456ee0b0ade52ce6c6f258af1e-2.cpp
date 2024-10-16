PJ_DEF(pj_status_t) pjmedia_rtcp_build_rtcp_bye(pjmedia_rtcp_session *session,
						void *buf,
						pj_size_t *length,
						const pj_str_t *reason)
{
    pjmedia_rtcp_common *hdr;
    pj_uint8_t *p;
    pj_size_t len;

    PJ_ASSERT_RETURN(session && buf && length, PJ_EINVAL);

    /* Verify BYE reason length */
    if (reason && reason->slen > 255)
	return PJ_EINVAL;

    /* Verify buffer length */
    len = sizeof(*hdr);
    if (reason && reason->slen) len += reason->slen + 1;
    len = ((len+3)/4) * 4;
    if (len > *length)
	return PJ_ETOOSMALL;

    /* Build RTCP BYE header */
    hdr = (pjmedia_rtcp_common*)buf;
    pj_memcpy(hdr, &session->rtcp_sr_pkt.common,  sizeof(*hdr));
    hdr->pt = RTCP_BYE;
    hdr->length = pj_htons((pj_uint16_t)(len/4 - 1));

    /* Write RTCP BYE reason */
    p = (pj_uint8_t*)hdr + sizeof(*hdr);
    if (reason && reason->slen) {
	*p++ = (pj_uint8_t)reason->slen;
	pj_memcpy(p, reason->ptr, reason->slen);
	p += reason->slen;
    }

    /* Pad to 32bit */
    while ((p-(pj_uint8_t*)buf) % 4)
	*p++ = 0;

    pj_assert((int)len == p-(pj_uint8_t*)buf);
    *length = len;

    return PJ_SUCCESS;
}