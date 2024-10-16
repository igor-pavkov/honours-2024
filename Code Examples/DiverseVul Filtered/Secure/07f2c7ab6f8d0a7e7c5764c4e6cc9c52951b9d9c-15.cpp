struct sctp_chunk *sctp_make_abort_user(const struct sctp_association *asoc,
					struct msghdr *msg,
					size_t paylen)
{
	struct sctp_chunk *retval;
	void *payload = NULL;
	int err;

	retval = sctp_make_abort(asoc, NULL,
				 sizeof(struct sctp_errhdr) + paylen);
	if (!retval)
		goto err_chunk;

	if (paylen) {
		/* Put the msg_iov together into payload.  */
		payload = kmalloc(paylen, GFP_KERNEL);
		if (!payload)
			goto err_payload;

		err = memcpy_from_msg(payload, msg, paylen);
		if (err < 0)
			goto err_copy;
	}

	sctp_init_cause(retval, SCTP_ERROR_USER_ABORT, paylen);
	sctp_addto_chunk(retval, paylen, payload);

	if (paylen)
		kfree(payload);

	return retval;

err_copy:
	kfree(payload);
err_payload:
	sctp_chunk_free(retval);
	retval = NULL;
err_chunk:
	return retval;
}