bool sctp_verify_reconf(const struct sctp_association *asoc,
			struct sctp_chunk *chunk,
			struct sctp_paramhdr **errp)
{
	struct sctp_reconf_chunk *hdr;
	union sctp_params param;
	__be16 last = 0;
	__u16 cnt = 0;

	hdr = (struct sctp_reconf_chunk *)chunk->chunk_hdr;
	sctp_walk_params(param, hdr, params) {
		__u16 length = ntohs(param.p->length);

		*errp = param.p;
		if (cnt++ > 2)
			return false;
		switch (param.p->type) {
		case SCTP_PARAM_RESET_OUT_REQUEST:
			if (length < sizeof(struct sctp_strreset_outreq) ||
			    (last && last != SCTP_PARAM_RESET_RESPONSE &&
			     last != SCTP_PARAM_RESET_IN_REQUEST))
				return false;
			break;
		case SCTP_PARAM_RESET_IN_REQUEST:
			if (length < sizeof(struct sctp_strreset_inreq) ||
			    (last && last != SCTP_PARAM_RESET_OUT_REQUEST))
				return false;
			break;
		case SCTP_PARAM_RESET_RESPONSE:
			if ((length != sizeof(struct sctp_strreset_resp) &&
			     length != sizeof(struct sctp_strreset_resptsn)) ||
			    (last && last != SCTP_PARAM_RESET_RESPONSE &&
			     last != SCTP_PARAM_RESET_OUT_REQUEST))
				return false;
			break;
		case SCTP_PARAM_RESET_TSN_REQUEST:
			if (length !=
			    sizeof(struct sctp_strreset_tsnreq) || last)
				return false;
			break;
		case SCTP_PARAM_RESET_ADD_IN_STREAMS:
			if (length != sizeof(struct sctp_strreset_addstrm) ||
			    (last && last != SCTP_PARAM_RESET_ADD_OUT_STREAMS))
				return false;
			break;
		case SCTP_PARAM_RESET_ADD_OUT_STREAMS:
			if (length != sizeof(struct sctp_strreset_addstrm) ||
			    (last && last != SCTP_PARAM_RESET_ADD_IN_STREAMS))
				return false;
			break;
		default:
			return false;
		}

		last = param.p->type;
	}

	return true;
}