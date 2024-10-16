void dccp_print(netdissect_options *ndo, const u_char *bp, const u_char *data2,
		u_int len)
{
	const struct dccp_hdr *dh;
	const struct ip *ip;
	const struct ip6_hdr *ip6;
	const u_char *cp;
	u_short sport, dport;
	u_int hlen;
	u_int fixed_hdrlen;
	uint8_t	dccph_type;

	dh = (const struct dccp_hdr *)bp;

	ip = (const struct ip *)data2;
	if (IP_V(ip) == 6)
		ip6 = (const struct ip6_hdr *)data2;
	else
		ip6 = NULL;

	/* make sure we have enough data to look at the X bit */
	cp = (const u_char *)(dh + 1);
	if (cp > ndo->ndo_snapend) {
		ND_PRINT((ndo, "[Invalid packet|dccp]"));
		return;
	}
	if (len < sizeof(struct dccp_hdr)) {
		ND_PRINT((ndo, "truncated-dccp - %u bytes missing!",
			  len - (u_int)sizeof(struct dccp_hdr)));
		return;
	}

	/* get the length of the generic header */
	fixed_hdrlen = dccp_basic_hdr_len(dh);
	if (len < fixed_hdrlen) {
		ND_PRINT((ndo, "truncated-dccp - %u bytes missing!",
			  len - fixed_hdrlen));
		return;
	}
	ND_TCHECK2(*dh, fixed_hdrlen);

	sport = EXTRACT_16BITS(&dh->dccph_sport);
	dport = EXTRACT_16BITS(&dh->dccph_dport);
	hlen = dh->dccph_doff * 4;

	if (ip6) {
		ND_PRINT((ndo, "%s.%d > %s.%d: ",
			  ip6addr_string(ndo, &ip6->ip6_src), sport,
			  ip6addr_string(ndo, &ip6->ip6_dst), dport));
	} else {
		ND_PRINT((ndo, "%s.%d > %s.%d: ",
			  ipaddr_string(ndo, &ip->ip_src), sport,
			  ipaddr_string(ndo, &ip->ip_dst), dport));
	}

	ND_PRINT((ndo, "DCCP"));

	if (ndo->ndo_qflag) {
		ND_PRINT((ndo, " %d", len - hlen));
		if (hlen > len) {
			ND_PRINT((ndo, " [bad hdr length %u - too long, > %u]",
				  hlen, len));
		}
		return;
	}

	/* other variables in generic header */
	if (ndo->ndo_vflag) {
		ND_PRINT((ndo, " (CCVal %d, CsCov %d, ", DCCPH_CCVAL(dh), DCCPH_CSCOV(dh)));
	}

	/* checksum calculation */
	if (ndo->ndo_vflag && ND_TTEST2(bp[0], len)) {
		uint16_t sum = 0, dccp_sum;

		dccp_sum = EXTRACT_16BITS(&dh->dccph_checksum);
		ND_PRINT((ndo, "cksum 0x%04x ", dccp_sum));
		if (IP_V(ip) == 4)
			sum = dccp_cksum(ndo, ip, dh, len);
		else if (IP_V(ip) == 6)
			sum = dccp6_cksum(ndo, ip6, dh, len);
		if (sum != 0)
			ND_PRINT((ndo, "(incorrect -> 0x%04x)",in_cksum_shouldbe(dccp_sum, sum)));
		else
			ND_PRINT((ndo, "(correct)"));
	}

	if (ndo->ndo_vflag)
		ND_PRINT((ndo, ")"));
	ND_PRINT((ndo, " "));

	dccph_type = DCCPH_TYPE(dh);
	switch (dccph_type) {
	case DCCP_PKT_REQUEST: {
		const struct dccp_hdr_request *dhr =
			(const struct dccp_hdr_request *)(bp + fixed_hdrlen);
		fixed_hdrlen += 4;
		if (len < fixed_hdrlen) {
			ND_PRINT((ndo, "truncated-%s - %u bytes missing!",
				  tok2str(dccp_pkt_type_str, "", dccph_type),
				  len - fixed_hdrlen));
			return;
		}
		ND_TCHECK(*dhr);
		ND_PRINT((ndo, "%s (service=%d) ",
			  tok2str(dccp_pkt_type_str, "", dccph_type),
			  EXTRACT_32BITS(&dhr->dccph_req_service)));
		break;
	}
	case DCCP_PKT_RESPONSE: {
		const struct dccp_hdr_response *dhr =
			(const struct dccp_hdr_response *)(bp + fixed_hdrlen);
		fixed_hdrlen += 12;
		if (len < fixed_hdrlen) {
			ND_PRINT((ndo, "truncated-%s - %u bytes missing!",
				  tok2str(dccp_pkt_type_str, "", dccph_type),
				  len - fixed_hdrlen));
			return;
		}
		ND_TCHECK(*dhr);
		ND_PRINT((ndo, "%s (service=%d) ",
			  tok2str(dccp_pkt_type_str, "", dccph_type),
			  EXTRACT_32BITS(&dhr->dccph_resp_service)));
		break;
	}
	case DCCP_PKT_DATA:
		ND_PRINT((ndo, "%s ", tok2str(dccp_pkt_type_str, "", dccph_type)));
		break;
	case DCCP_PKT_ACK: {
		fixed_hdrlen += 8;
		if (len < fixed_hdrlen) {
			ND_PRINT((ndo, "truncated-%s - %u bytes missing!",
				  tok2str(dccp_pkt_type_str, "", dccph_type),
				  len - fixed_hdrlen));
			return;
		}
		ND_PRINT((ndo, "%s ", tok2str(dccp_pkt_type_str, "", dccph_type)));
		break;
	}
	case DCCP_PKT_DATAACK: {
		fixed_hdrlen += 8;
		if (len < fixed_hdrlen) {
			ND_PRINT((ndo, "truncated-%s - %u bytes missing!",
				  tok2str(dccp_pkt_type_str, "", dccph_type),
				  len - fixed_hdrlen));
			return;
		}
		ND_PRINT((ndo, "%s ", tok2str(dccp_pkt_type_str, "", dccph_type)));
		break;
	}
	case DCCP_PKT_CLOSEREQ:
		fixed_hdrlen += 8;
		if (len < fixed_hdrlen) {
			ND_PRINT((ndo, "truncated-%s - %u bytes missing!",
				  tok2str(dccp_pkt_type_str, "", dccph_type),
				  len - fixed_hdrlen));
			return;
		}
		ND_PRINT((ndo, "%s ", tok2str(dccp_pkt_type_str, "", dccph_type)));
		break;
	case DCCP_PKT_CLOSE:
		fixed_hdrlen += 8;
		if (len < fixed_hdrlen) {
			ND_PRINT((ndo, "truncated-%s - %u bytes missing!",
				  tok2str(dccp_pkt_type_str, "", dccph_type),
				  len - fixed_hdrlen));
			return;
		}
		ND_PRINT((ndo, "%s ", tok2str(dccp_pkt_type_str, "", dccph_type)));
		break;
	case DCCP_PKT_RESET: {
		const struct dccp_hdr_reset *dhr =
			(const struct dccp_hdr_reset *)(bp + fixed_hdrlen);
		fixed_hdrlen += 12;
		if (len < fixed_hdrlen) {
			ND_PRINT((ndo, "truncated-%s - %u bytes missing!",
				  tok2str(dccp_pkt_type_str, "", dccph_type),
				  len - fixed_hdrlen));
			return;
		}
		ND_TCHECK(*dhr);
		ND_PRINT((ndo, "%s (code=%s) ",
			  tok2str(dccp_pkt_type_str, "", dccph_type),
			  dccp_reset_code(dhr->dccph_reset_code)));
		break;
	}
	case DCCP_PKT_SYNC:
		fixed_hdrlen += 8;
		if (len < fixed_hdrlen) {
			ND_PRINT((ndo, "truncated-%s - %u bytes missing!",
				  tok2str(dccp_pkt_type_str, "", dccph_type),
				  len - fixed_hdrlen));
			return;
		}
		ND_PRINT((ndo, "%s ", tok2str(dccp_pkt_type_str, "", dccph_type)));
		break;
	case DCCP_PKT_SYNCACK:
		fixed_hdrlen += 8;
		if (len < fixed_hdrlen) {
			ND_PRINT((ndo, "truncated-%s - %u bytes missing!",
				  tok2str(dccp_pkt_type_str, "", dccph_type),
				  len - fixed_hdrlen));
			return;
		}
		ND_PRINT((ndo, "%s ", tok2str(dccp_pkt_type_str, "", dccph_type)));
		break;
	default:
		ND_PRINT((ndo, "%s ", tok2str(dccp_pkt_type_str, "unknown-type-%u", dccph_type)));
		break;
	}

	if ((DCCPH_TYPE(dh) != DCCP_PKT_DATA) &&
			(DCCPH_TYPE(dh) != DCCP_PKT_REQUEST))
		dccp_print_ack_no(ndo, bp);

	if (ndo->ndo_vflag < 2)
		return;

	ND_PRINT((ndo, "seq %" PRIu64, dccp_seqno(bp)));

	/* process options */
	if (hlen > fixed_hdrlen){
		u_int optlen;
		cp = bp + fixed_hdrlen;
		ND_PRINT((ndo, " <"));

		hlen -= fixed_hdrlen;
		while(1){
			optlen = dccp_print_option(ndo, cp, hlen);
			if (!optlen)
				break;
			if (hlen <= optlen)
				break;
			hlen -= optlen;
			cp += optlen;
			ND_PRINT((ndo, ", "));
		}
		ND_PRINT((ndo, ">"));
	}
	return;
trunc:
	ND_PRINT((ndo, "%s", tstr));
	return;
}