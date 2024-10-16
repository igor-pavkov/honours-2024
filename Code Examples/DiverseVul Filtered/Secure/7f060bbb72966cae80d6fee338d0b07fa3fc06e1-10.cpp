static int avi_init_super_index(avi_t *AVI, unsigned char *idxtag, avisuperindex_chunk **si)
{
	int k;

	avisuperindex_chunk *sil = NULL;

	if ((sil = (avisuperindex_chunk *) gf_malloc (sizeof (avisuperindex_chunk))) == NULL) {
		AVI_errno = AVI_ERR_NO_MEM;
		return -1;
	}
	memcpy (sil->fcc, "indx", 4);
	sil->dwSize = 0; // size of this chunk
	sil->wLongsPerEntry = 4;
	sil->bIndexSubType = 0;
	sil->bIndexType = AVI_INDEX_OF_INDEXES;
	sil->nEntriesInUse = 0; // none are in use
	memcpy (sil->dwChunkId, idxtag, 4);
	memset (sil->dwReserved, 0, sizeof (sil->dwReserved));

	// NR_IXNN_CHUNKS == allow 32 indices which means 32 GB files -- arbitrary
	sil->aIndex = (avisuperindex_entry *) gf_malloc (sil->wLongsPerEntry * NR_IXNN_CHUNKS * sizeof (void*));
	if (!sil->aIndex) {
		AVI_errno = AVI_ERR_NO_MEM;
		return -1;
	}
	memset (sil->aIndex, 0, sil->wLongsPerEntry * NR_IXNN_CHUNKS * sizeof (u32));

	sil->stdindex = (avistdindex_chunk **)gf_malloc (NR_IXNN_CHUNKS * sizeof (avistdindex_chunk *));
	if (!sil->stdindex) {
		AVI_errno = AVI_ERR_NO_MEM;
		return -1;
	}
	for (k = 0; k < NR_IXNN_CHUNKS; k++) {
		sil->stdindex[k] = (avistdindex_chunk *) gf_malloc (sizeof (avistdindex_chunk));
		// gets rewritten later
		sil->stdindex[k]->qwBaseOffset = (u64)k * AVI->new_riff_threshold;
		sil->stdindex[k]->aIndex = NULL;
	}

	*si = sil;

	return 0;
}