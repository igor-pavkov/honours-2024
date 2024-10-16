log_reply_info(enum verbosity_value v, struct query_info *qinf,
	struct sockaddr_storage *addr, socklen_t addrlen, struct timeval dur,
	int cached, struct sldns_buffer *rmsg)
{
	char qname_buf[LDNS_MAX_DOMAINLEN+1];
	char clientip_buf[128];
	char rcode_buf[16];
	char type_buf[16];
	char class_buf[16];
	size_t pktlen;
	uint16_t rcode = FLAGS_GET_RCODE(sldns_buffer_read_u16_at(rmsg, 2));

	if(verbosity < v)
	  return;

	sldns_wire2str_rcode_buf((int)rcode, rcode_buf, sizeof(rcode_buf));
	addr_to_str(addr, addrlen, clientip_buf, sizeof(clientip_buf));
	if(rcode == LDNS_RCODE_FORMERR)
	{
		if(LOG_TAG_QUERYREPLY)
			log_reply("%s - - - %s - - - ", clientip_buf, rcode_buf);
		else	log_info("%s - - - %s - - - ", clientip_buf, rcode_buf);
	} else {
		if(qinf->qname)
			dname_str(qinf->qname, qname_buf);
		else	snprintf(qname_buf, sizeof(qname_buf), "null");
		pktlen = sldns_buffer_limit(rmsg);
		sldns_wire2str_type_buf(qinf->qtype, type_buf, sizeof(type_buf));
		sldns_wire2str_class_buf(qinf->qclass, class_buf, sizeof(class_buf));
		if(LOG_TAG_QUERYREPLY)
		     log_reply("%s %s %s %s %s " ARG_LL "d.%6.6d %d %d",
			clientip_buf, qname_buf, type_buf, class_buf,
			rcode_buf, (long long)dur.tv_sec, (int)dur.tv_usec, cached, (int)pktlen);
		else log_info("%s %s %s %s %s " ARG_LL "d.%6.6d %d %d",
			clientip_buf, qname_buf, type_buf, class_buf,
			rcode_buf, (long long)dur.tv_sec, (int)dur.tv_usec, cached, (int)pktlen);
	}
}