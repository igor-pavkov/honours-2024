static enum sctp_ierror sctp_process_unk_param(
					const struct sctp_association *asoc,
					union sctp_params param,
					struct sctp_chunk *chunk,
					struct sctp_chunk **errp)
{
	int retval = SCTP_IERROR_NO_ERROR;

	switch (param.p->type & SCTP_PARAM_ACTION_MASK) {
	case SCTP_PARAM_ACTION_DISCARD:
		retval =  SCTP_IERROR_ERROR;
		break;
	case SCTP_PARAM_ACTION_SKIP:
		break;
	case SCTP_PARAM_ACTION_DISCARD_ERR:
		retval =  SCTP_IERROR_ERROR;
		fallthrough;
	case SCTP_PARAM_ACTION_SKIP_ERR:
		/* Make an ERROR chunk, preparing enough room for
		 * returning multiple unknown parameters.
		 */
		if (!*errp) {
			*errp = sctp_make_op_error_limited(asoc, chunk);
			if (!*errp) {
				/* If there is no memory for generating the
				 * ERROR report as specified, an ABORT will be
				 * triggered to the peer and the association
				 * won't be established.
				 */
				retval = SCTP_IERROR_NOMEM;
				break;
			}
		}

		if (!sctp_init_cause(*errp, SCTP_ERROR_UNKNOWN_PARAM,
				     ntohs(param.p->length)))
			sctp_addto_chunk(*errp, ntohs(param.p->length),
					 param.v);
		break;
	default:
		break;
	}

	return retval;
}