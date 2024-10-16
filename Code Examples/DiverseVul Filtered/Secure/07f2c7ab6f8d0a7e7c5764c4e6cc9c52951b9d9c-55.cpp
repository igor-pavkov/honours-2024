struct sctp_chunk *sctp_make_heartbeat_ack(const struct sctp_association *asoc,
					   const struct sctp_chunk *chunk,
					   const void *payload,
					   const size_t paylen)
{
	struct sctp_chunk *retval;

	retval  = sctp_make_control(asoc, SCTP_CID_HEARTBEAT_ACK, 0, paylen,
				    GFP_ATOMIC);
	if (!retval)
		goto nodata;

	retval->subh.hbs_hdr = sctp_addto_chunk(retval, paylen, payload);

	/* RFC 2960 6.4 Multi-homed SCTP Endpoints
	 *
	 * An endpoint SHOULD transmit reply chunks (e.g., SACK,
	 * HEARTBEAT ACK, * etc.) to the same destination transport
	 * address from which it * received the DATA or control chunk
	 * to which it is replying.
	 *
	 * [HBACK back to where the HEARTBEAT came from.]
	 */
	if (chunk)
		retval->transport = chunk->transport;

nodata:
	return retval;
}