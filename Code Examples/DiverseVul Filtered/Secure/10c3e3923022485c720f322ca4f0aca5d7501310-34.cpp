static NTSTATUS grant_fsp_lease(struct files_struct *fsp,
				struct share_mode_lock *lck,
				const struct smb2_lease *lease,
				uint32_t *p_lease_idx,
				uint32_t granted)
{
	struct share_mode_data *d = lck->data;
	const struct GUID *client_guid = fsp_client_guid(fsp);
	struct share_mode_lease *tmp;
	NTSTATUS status;
	int idx;

	idx = find_share_mode_lease(d, client_guid, &lease->lease_key);

	if (idx != -1) {
		struct share_mode_lease *l = &d->leases[idx];
		bool do_upgrade;
		uint32_t existing, requested;

		fsp->lease = find_fsp_lease(fsp, &lease->lease_key, l);
		if (fsp->lease == NULL) {
			DEBUG(1, ("Did not find existing lease for file %s\n",
				  fsp_str_dbg(fsp)));
			return NT_STATUS_NO_MEMORY;
		}

		*p_lease_idx = idx;

		/*
		 * Upgrade only if the requested lease is a strict upgrade.
		 */
		existing = l->current_state;
		requested = lease->lease_state;

		/*
		 * Tricky: This test makes sure that "requested" is a
		 * strict bitwise superset of "existing".
		 */
		do_upgrade = ((existing & requested) == existing);

		/*
		 * Upgrade only if there's a change.
		 */
		do_upgrade &= (granted != existing);

		/*
		 * Upgrade only if other leases don't prevent what was asked
		 * for.
		 */
		do_upgrade &= (granted == requested);

		/*
		 * only upgrade if we are not in breaking state
		 */
		do_upgrade &= !l->breaking;

		DEBUG(10, ("existing=%"PRIu32", requested=%"PRIu32", "
			   "granted=%"PRIu32", do_upgrade=%d\n",
			   existing, requested, granted, (int)do_upgrade));

		if (do_upgrade) {
			l->current_state = granted;
			l->epoch += 1;
		}

		/* Ensure we're in sync with current lease state. */
		fsp_lease_update(lck, fsp_client_guid(fsp), fsp->lease);
		return NT_STATUS_OK;
	}

	/*
	 * Create new lease
	 */

	tmp = talloc_realloc(d, d->leases, struct share_mode_lease,
			     d->num_leases+1);
	if (tmp == NULL) {
		/*
		 * See [MS-SMB2]
		 */
		return NT_STATUS_INSUFFICIENT_RESOURCES;
	}
	d->leases = tmp;

	fsp->lease = talloc_zero(fsp->conn->sconn, struct fsp_lease);
	if (fsp->lease == NULL) {
		return NT_STATUS_INSUFFICIENT_RESOURCES;
	}
	fsp->lease->ref_count = 1;
	fsp->lease->sconn = fsp->conn->sconn;
	fsp->lease->lease.lease_version = lease->lease_version;
	fsp->lease->lease.lease_key = lease->lease_key;
	fsp->lease->lease.lease_state = granted;
	fsp->lease->lease.lease_epoch = lease->lease_epoch + 1;

	*p_lease_idx = d->num_leases;

	d->leases[d->num_leases] = (struct share_mode_lease) {
		.client_guid = *client_guid,
		.lease_key = fsp->lease->lease.lease_key,
		.current_state = fsp->lease->lease.lease_state,
		.lease_version = fsp->lease->lease.lease_version,
		.epoch = fsp->lease->lease.lease_epoch,
	};

	status = leases_db_add(client_guid,
			       &lease->lease_key,
			       &fsp->file_id,
			       fsp->conn->connectpath,
			       fsp->fsp_name->base_name,
			       fsp->fsp_name->stream_name);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10, ("%s: leases_db_add failed: %s\n", __func__,
			   nt_errstr(status)));
		TALLOC_FREE(fsp->lease);
		return NT_STATUS_INSUFFICIENT_RESOURCES;
	}

	d->num_leases += 1;
	d->modified = true;

	return NT_STATUS_OK;
}