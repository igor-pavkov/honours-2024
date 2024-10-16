static int avi_add_std_index(avi_t *AVI, unsigned char *idxtag, unsigned char *strtag,
                             avistdindex_chunk *stdil)
{

	memcpy (stdil->fcc, idxtag, 4);
	stdil->dwSize = 4096;
	stdil->wLongsPerEntry = 2; //sizeof(avistdindex_entry)/sizeof(u32);
	stdil->bIndexSubType = 0;
	stdil->bIndexType = AVI_INDEX_OF_CHUNKS;
	stdil->nEntriesInUse = 0;

	// cp 00db ChunkId
	memcpy(stdil->dwChunkId, strtag, 4);

	//stdil->qwBaseOffset = AVI->video_superindex->aIndex[ cur_std_idx ]->qwOffset;

	stdil->aIndex = (avistdindex_entry *)gf_malloc(stdil->dwSize * sizeof (u32) * stdil->wLongsPerEntry);

	if (!stdil->aIndex) {
		AVI_errno = AVI_ERR_NO_MEM;
		return -1;
	}


	return 0;
}