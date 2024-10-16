static HtPP *mig_hash_new(void) {
	HtPP *hash = sdb_ht_new ();
	if (!hash) {
		return NULL;
	}

	int i;
	for (i = 0; i < R_MIG_INDEX_LEN; i += 2) {
		const char *num = mig_index[i];
		const char *name = mig_index[i+1];
		sdb_ht_insert (hash, num, name);
	}

	return hash;
}