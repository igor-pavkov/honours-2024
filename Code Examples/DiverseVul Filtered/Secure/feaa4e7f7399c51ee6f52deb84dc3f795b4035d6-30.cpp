static void r_kext_index_free(RKextIndex *index) {
	if (!index) {
		return;
	}

	int i = 0;
	RKext *kext;
	r_kext_index_foreach (index, i, kext) {
		r_kext_free (kext);
		index->entries[i] = NULL;
	}

	index->length = 0;
	R_FREE (index);
}