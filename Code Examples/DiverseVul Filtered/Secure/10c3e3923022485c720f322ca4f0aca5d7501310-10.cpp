static void lease_match_parser_new_file(
	uint32_t num_files,
	const struct leases_db_file *files,
	struct lease_match_state *state)
{
	uint32_t i;

	for (i = 0; i < num_files; i++) {
		const struct leases_db_file *f = &files[i];
		if (strequal(state->servicepath, f->servicepath)) {
			state->match_status = NT_STATUS_INVALID_PARAMETER;
			return;
		}
	}

	/* Dynamic share case. Break leases on all other files. */
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