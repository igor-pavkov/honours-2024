struct fsp_lease *find_fsp_lease(struct files_struct *new_fsp,
				 const struct smb2_lease_key *key,
				 const struct share_mode_lease *l)
{
	struct files_struct *fsp;

	/*
	 * TODO: Measure how expensive this loop is with thousands of open
	 * handles...
	 */

	for (fsp = file_find_di_first(new_fsp->conn->sconn, new_fsp->file_id);
	     fsp != NULL;
	     fsp = file_find_di_next(fsp)) {

		if (fsp == new_fsp) {
			continue;
		}
		if (fsp->oplock_type != LEASE_OPLOCK) {
			continue;
		}
		if (smb2_lease_key_equal(&fsp->lease->lease.lease_key, key)) {
			fsp->lease->ref_count += 1;
			return fsp->lease;
		}
	}

	/* Not found - must be leased in another smbd. */
	new_fsp->lease = talloc_zero(new_fsp->conn->sconn, struct fsp_lease);
	if (new_fsp->lease == NULL) {
		return NULL;
	}
	new_fsp->lease->ref_count = 1;
	new_fsp->lease->sconn = new_fsp->conn->sconn;
	new_fsp->lease->lease.lease_key = *key;
	new_fsp->lease->lease.lease_state = l->current_state;
	/*
	 * We internally treat all leases as V2 and update
	 * the epoch, but when sending breaks it matters if
	 * the requesting lease was v1 or v2.
	 */
	new_fsp->lease->lease.lease_version = l->lease_version;
	new_fsp->lease->lease.lease_epoch = l->epoch;
	return new_fsp->lease;
}