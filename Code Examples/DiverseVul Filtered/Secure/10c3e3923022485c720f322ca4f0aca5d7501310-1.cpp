static bool is_same_lease(const files_struct *fsp,
			  const struct share_mode_data *d,
			  const struct share_mode_entry *e,
			  const struct smb2_lease *lease)
{
	if (e->op_type != LEASE_OPLOCK) {
		return false;
	}
	if (lease == NULL) {
		return false;
	}

	return smb2_lease_equal(fsp_client_guid(fsp),
				&lease->lease_key,
				&d->leases[e->lease_idx].client_guid,
				&d->leases[e->lease_idx].lease_key);
}