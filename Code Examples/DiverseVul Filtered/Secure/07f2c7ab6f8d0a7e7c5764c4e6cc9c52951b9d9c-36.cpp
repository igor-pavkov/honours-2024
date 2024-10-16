struct sctp_chunk *sctp_make_shutdown_complete(
					const struct sctp_association *asoc,
					const struct sctp_chunk *chunk)
{
	struct sctp_chunk *retval;
	__u8 flags = 0;

	/* Set the T-bit if we have no association (vtag will be
	 * reflected)
	 */
	flags |= asoc ? 0 : SCTP_CHUNK_FLAG_T;

	retval = sctp_make_control(asoc, SCTP_CID_SHUTDOWN_COMPLETE, flags,
				   0, GFP_ATOMIC);

	/* RFC 2960 6.4 Multi-homed SCTP Endpoints
	 *
	 * An endpoint SHOULD transmit reply chunks (e.g., SACK,
	 * HEARTBEAT ACK, * etc.) to the same destination transport
	 * address from which it * received the DATA or control chunk
	 * to which it is replying.
	 *
	 * [Report SHUTDOWN COMPLETE back to where the SHUTDOWN ACK
	 * came from.]
	 */
	if (retval && chunk)
		retval->transport = chunk->transport;

	return retval;
}