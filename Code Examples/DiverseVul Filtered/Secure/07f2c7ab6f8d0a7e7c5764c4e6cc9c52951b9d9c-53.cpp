static struct sctp_chunk *sctp_make_op_error_space(
					const struct sctp_association *asoc,
					const struct sctp_chunk *chunk,
					size_t size)
{
	struct sctp_chunk *retval;

	retval = sctp_make_control(asoc, SCTP_CID_ERROR, 0,
				   sizeof(struct sctp_errhdr) + size,
				   GFP_ATOMIC);
	if (!retval)
		goto nodata;

	/* RFC 2960 6.4 Multi-homed SCTP Endpoints
	 *
	 * An endpoint SHOULD transmit reply chunks (e.g., SACK,
	 * HEARTBEAT ACK, etc.) to the same destination transport
	 * address from which it received the DATA or control chunk
	 * to which it is replying.
	 *
	 */
	if (chunk)
		retval->transport = chunk->transport;

nodata:
	return retval;
}