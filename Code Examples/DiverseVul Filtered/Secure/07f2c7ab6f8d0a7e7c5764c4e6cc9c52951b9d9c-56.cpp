struct sctp_chunk *sctp_make_abort(const struct sctp_association *asoc,
				   const struct sctp_chunk *chunk,
				   const size_t hint)
{
	struct sctp_chunk *retval;
	__u8 flags = 0;

	/* Set the T-bit if we have no association and 'chunk' is not
	 * an INIT (vtag will be reflected).
	 */
	if (!asoc) {
		if (chunk && chunk->chunk_hdr &&
		    chunk->chunk_hdr->type == SCTP_CID_INIT)
			flags = 0;
		else
			flags = SCTP_CHUNK_FLAG_T;
	}

	retval = sctp_make_control(asoc, SCTP_CID_ABORT, flags, hint,
				   GFP_ATOMIC);

	/* RFC 2960 6.4 Multi-homed SCTP Endpoints
	 *
	 * An endpoint SHOULD transmit reply chunks (e.g., SACK,
	 * HEARTBEAT ACK, * etc.) to the same destination transport
	 * address from which it * received the DATA or control chunk
	 * to which it is replying.
	 *
	 * [ABORT back to where the offender came from.]
	 */
	if (retval && chunk)
		retval->transport = chunk->transport;

	return retval;
}