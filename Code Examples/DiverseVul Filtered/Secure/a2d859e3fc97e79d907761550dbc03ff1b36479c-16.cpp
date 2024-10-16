struct sctp_chunk *sctp_make_new_encap_port(const struct sctp_association *asoc,
					    const struct sctp_chunk *chunk)
{
	struct sctp_new_encap_port_hdr nep;
	struct sctp_chunk *retval;

	retval = sctp_make_abort(asoc, chunk,
				 sizeof(struct sctp_errhdr) + sizeof(nep));
	if (!retval)
		goto nodata;

	sctp_init_cause(retval, SCTP_ERROR_NEW_ENCAP_PORT, sizeof(nep));
	nep.cur_port = SCTP_INPUT_CB(chunk->skb)->encap_port;
	nep.new_port = chunk->transport->encap_port;
	sctp_addto_chunk(retval, sizeof(nep), &nep);

nodata:
	return retval;
}