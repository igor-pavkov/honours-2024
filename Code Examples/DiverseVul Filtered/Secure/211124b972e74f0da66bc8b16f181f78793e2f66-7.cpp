static void dccp_print_ack_no(netdissect_options *ndo, const u_char *bp)
{
	const struct dccp_hdr *dh = (const struct dccp_hdr *)bp;
	const u_char *ackp = bp + dccp_basic_hdr_len(dh);
	uint64_t ackno;

	if (DCCPH_X(dh) != 0) {
		ND_TCHECK2(*ackp, 8);
		ackno = EXTRACT_48BITS(ackp + 2);
	} else {
		ND_TCHECK2(*ackp, 4);
		ackno = EXTRACT_24BITS(ackp + 1);
	}

	ND_PRINT((ndo, "(ack=%" PRIu64 ") ", ackno));
trunc:
	return;
}