static RKextIndex *r_kext_index_new(RList *kexts) {
	if (!kexts) {
		return NULL;
	}

	int length = r_list_length (kexts);
	if (!length) {
		return NULL;
	}

	RKextIndex *index = R_NEW0 (RKextIndex);
	if (!index) {
		return NULL;
	}

	index->entries = malloc (length *sizeof(RKext*));
	if (!index->entries) {
		R_FREE (index);
		return NULL;
	}

	RListIter *iter;
	RKext *kext;
	int i = 0;
	r_list_foreach (kexts, iter, kext) {
		index->entries[i++] = kext;
	}
	index->length = i;

	return index;
}