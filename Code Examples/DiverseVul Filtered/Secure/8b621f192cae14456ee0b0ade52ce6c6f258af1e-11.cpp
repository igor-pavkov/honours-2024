static void parse_rtcp_sdes(pjmedia_rtcp_session *sess,
			    const void *pkt,
			    pj_size_t size)
{
    pjmedia_rtcp_sdes *sdes = &sess->stat.peer_sdes;
    char *p, *p_end;
    char *b, *b_end;

    p = (char*)pkt + 8;
    p_end = (char*)pkt + size;

    pj_bzero(sdes, sizeof(*sdes));
    b = sess->stat.peer_sdes_buf_;
    b_end = b + sizeof(sess->stat.peer_sdes_buf_);

    while (p < p_end) {
	pj_uint8_t sdes_type, sdes_len;
	pj_str_t sdes_value = {NULL, 0};

	sdes_type = *p++;

	/* Check for end of SDES item list */
	if (sdes_type == RTCP_SDES_NULL || p == p_end)
	    break;

	sdes_len = *p++;

	/* Check for corrupted SDES packet */
	if (p + sdes_len > p_end)
	    break;

	/* Get SDES item */
	if (b + sdes_len < b_end) {
	    pj_memcpy(b, p, sdes_len);
	    sdes_value.ptr = b;
	    sdes_value.slen = sdes_len;
	    b += sdes_len;
	} else {
	    /* Insufficient SDES buffer */
	    PJ_LOG(5, (sess->name,
		    "Unsufficient buffer to save RTCP SDES type %d:%.*s",
		    sdes_type, sdes_len, p));
	    p += sdes_len;
	    continue;
	}

	switch (sdes_type) {
	case RTCP_SDES_CNAME:
	    sdes->cname = sdes_value;
	    break;
	case RTCP_SDES_NAME:
	    sdes->name = sdes_value;
	    break;
	case RTCP_SDES_EMAIL:
	    sdes->email = sdes_value;
	    break;
	case RTCP_SDES_PHONE:
	    sdes->phone = sdes_value;
	    break;
	case RTCP_SDES_LOC:
	    sdes->loc = sdes_value;
	    break;
	case RTCP_SDES_TOOL:
	    sdes->tool = sdes_value;
	    break;
	case RTCP_SDES_NOTE:
	    sdes->note = sdes_value;
	    break;
	default:
	    TRACE_((sess->name, "Received unknown RTCP SDES type %d:%.*s",
		    sdes_type, sdes_value.slen, sdes_value.ptr));
	    break;
	}

	p += sdes_len;
    }
}