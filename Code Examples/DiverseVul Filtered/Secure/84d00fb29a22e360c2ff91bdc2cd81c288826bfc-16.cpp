static int jpc_dec_decode(jpc_dec_t *dec)
{
	jpc_ms_t *ms;
	const jpc_dec_mstabent_t *mstabent;
	int ret;
	jpc_cstate_t *cstate;

	if (!(cstate = jpc_cstate_create())) {
		return -1;
	}
	dec->cstate = cstate;

	/* Initially, we should expect to encounter a SOC marker segment. */
	dec->state = JPC_MHSOC;

	for (;;) {

		/* Get the next marker segment in the code stream. */
		if (!(ms = jpc_getms(dec->in, cstate))) {
			jas_eprintf("cannot get marker segment\n");
			return -1;
		}

		mstabent = jpc_dec_mstab_lookup(ms->id);
		assert(mstabent);

		/* Ensure that this type of marker segment is permitted
		  at this point in the code stream. */
		if (!(dec->state & mstabent->validstates)) {
			jas_eprintf("unexpected marker segment type\n");
			jpc_ms_destroy(ms);
			return -1;
		}

		/* Process the marker segment. */
		if (mstabent->action) {
			ret = (*mstabent->action)(dec, ms);
		} else {
			/* No explicit action is required. */
			ret = 0;
		}

		/* Destroy the marker segment. */
		jpc_ms_destroy(ms);

		if (ret < 0) {
			return -1;
		} else if (ret > 0) {
			break;
		}

	}

	return 0;
}