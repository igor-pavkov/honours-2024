int find_share_mode_lease(struct share_mode_data *d,
			  const struct GUID *client_guid,
			  const struct smb2_lease_key *key)
{
	uint32_t i;

	for (i=0; i<d->num_leases; i++) {
		struct share_mode_lease *l = &d->leases[i];

		if (smb2_lease_equal(client_guid,
				     key,
				     &l->client_guid,
				     &l->lease_key)) {
			return i;
		}
	}

	return -1;
}