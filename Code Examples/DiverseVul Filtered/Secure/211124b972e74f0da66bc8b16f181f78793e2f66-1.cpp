static uint64_t dccp_seqno(const u_char *bp)
{
	const struct dccp_hdr *dh = (const struct dccp_hdr *)bp;
	uint64_t seqno;

	if (DCCPH_X(dh) != 0) {
		const struct dccp_hdr_ext *dhx = (const struct dccp_hdr_ext *)bp;
		seqno = EXTRACT_48BITS(dhx->dccph_seq);
	} else {
		seqno = EXTRACT_24BITS(dh->dccph_seq);
	}

	return seqno;
}