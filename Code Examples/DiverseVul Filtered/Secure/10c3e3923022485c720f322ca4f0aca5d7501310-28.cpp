static void lease_match_parser(
	uint32_t num_files,
	const struct leases_db_file *files,
	void *private_data)
{
	struct lease_match_state *state =
		(struct lease_match_state *)private_data;
	uint32_t i;

	if (!state->file_existed) {
		/*
		 * Deal with name mismatch or
		 * possible dynamic share case separately
		 * to make code clearer.
		 */
		lease_match_parser_new_file(num_files,
						files,
						state);
		return;
	}

	/* File existed. */
	state->match_status = NT_STATUS_OK;

	for (i = 0; i < num_files; i++) {
		const struct leases_db_file *f = &files[i];

		/* Everything should be the same. */
		if (!file_id_equal(&state->id, &f->id)) {
			/* This should catch all dynamic share cases. */
			state->match_status = NT_STATUS_OPLOCK_NOT_GRANTED;
			break;
		}
		if (!strequal(f->servicepath, state->servicepath)) {
			state->match_status = NT_STATUS_INVALID_PARAMETER;
			break;
		}
		if (!strequal(f->base_name, state->fname->base_name)) {
			state->match_status = NT_STATUS_INVALID_PARAMETER;
			break;
		}
		if (!strequal(f->stream_name, state->fname->stream_name)) {
			state->match_status = NT_STATUS_INVALID_PARAMETER;
			break;
		}
	}

	if (NT_STATUS_IS_OK(state->match_status)) {
		/*
		 * Common case - just opening another handle on a
		 * file on a non-dynamic share.
		 */
		return;
	}

	if (NT_STATUS_EQUAL(state->match_status, NT_STATUS_INVALID_PARAMETER)) {
		/* Mismatched path. Error back to client. */
		return;
	}

	/*
	 * File id mismatch. Dynamic share case NT_STATUS_OPLOCK_NOT_GRANTED.
	 * Don't allow leases.
	 */

	state->match_status = leases_db_copy_file_ids(state->mem_ctx,
					num_files,
					files,
					&state->ids);
	if (!NT_STATUS_IS_OK(state->match_status)) {
		return;
	}

	state->num_file_ids = num_files;
	state->match_status = NT_STATUS_OPLOCK_NOT_GRANTED;
	return;
}