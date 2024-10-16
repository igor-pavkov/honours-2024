int sctp_verify_init(struct net *net, const struct sctp_endpoint *ep,
		     const struct sctp_association *asoc, enum sctp_cid cid,
		     struct sctp_init_chunk *peer_init,
		     struct sctp_chunk *chunk, struct sctp_chunk **errp)
{
	union sctp_params param;
	bool has_cookie = false;
	int result;

	/* Check for missing mandatory parameters. Note: Initial TSN is
	 * also mandatory, but is not checked here since the valid range
	 * is 0..2**32-1. RFC4960, section 3.3.3.
	 */
	if (peer_init->init_hdr.num_outbound_streams == 0 ||
	    peer_init->init_hdr.num_inbound_streams == 0 ||
	    peer_init->init_hdr.init_tag == 0 ||
	    ntohl(peer_init->init_hdr.a_rwnd) < SCTP_DEFAULT_MINWINDOW)
		return sctp_process_inv_mandatory(asoc, chunk, errp);

	sctp_walk_params(param, peer_init, init_hdr.params) {
		if (param.p->type == SCTP_PARAM_STATE_COOKIE)
			has_cookie = true;
	}

	/* There is a possibility that a parameter length was bad and
	 * in that case we would have stoped walking the parameters.
	 * The current param.p would point at the bad one.
	 * Current consensus on the mailing list is to generate a PROTOCOL
	 * VIOLATION error.  We build the ERROR chunk here and let the normal
	 * error handling code build and send the packet.
	 */
	if (param.v != (void *)chunk->chunk_end)
		return sctp_process_inv_paramlength(asoc, param.p, chunk, errp);

	/* The only missing mandatory param possible today is
	 * the state cookie for an INIT-ACK chunk.
	 */
	if ((SCTP_CID_INIT_ACK == cid) && !has_cookie)
		return sctp_process_missing_param(asoc, SCTP_PARAM_STATE_COOKIE,
						  chunk, errp);

	/* Verify all the variable length parameters */
	sctp_walk_params(param, peer_init, init_hdr.params) {
		result = sctp_verify_param(net, ep, asoc, param, cid,
					   chunk, errp);
		switch (result) {
		case SCTP_IERROR_ABORT:
		case SCTP_IERROR_NOMEM:
			return 0;
		case SCTP_IERROR_ERROR:
			return 1;
		case SCTP_IERROR_NO_ERROR:
		default:
			break;
		}

	} /* for (loop through all parameters) */

	return 1;
}