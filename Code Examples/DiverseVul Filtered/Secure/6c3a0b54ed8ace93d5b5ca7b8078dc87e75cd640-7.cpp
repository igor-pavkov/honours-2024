int reply_info_parse(sldns_buffer* pkt, struct alloc_cache* alloc,
        struct query_info* qinf, struct reply_info** rep, 
	struct regional* region, struct edns_data* edns)
{
	/* use scratch pad region-allocator during parsing. */
	struct msg_parse* msg;
	int ret;
	
	qinf->qname = NULL;
	qinf->local_alias = NULL;
	*rep = NULL;
	if(!(msg = regional_alloc(region, sizeof(*msg)))) {
		return LDNS_RCODE_SERVFAIL;
	}
	memset(msg, 0, sizeof(*msg));
	
	sldns_buffer_set_position(pkt, 0);
	if((ret = parse_packet(pkt, msg, region)) != 0) {
		return ret;
	}
	if((ret = parse_extract_edns(msg, edns, region)) != 0)
		return ret;

	/* parse OK, allocate return structures */
	/* this also performs dname decompression */
	if(!parse_create_msg(pkt, msg, alloc, qinf, rep, NULL)) {
		query_info_clear(qinf);
		reply_info_parsedelete(*rep, alloc);
		*rep = NULL;
		return LDNS_RCODE_SERVFAIL;
	}
	return 0;
}