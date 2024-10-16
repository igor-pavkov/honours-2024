static int avi_add_odml_index_entry_core(avi_t *AVI, int flags, u64 pos, unsigned int len, avistdindex_chunk *si)
{
	u32 cur_chunk_idx;
	// put new chunk into index
	si->nEntriesInUse++;
	cur_chunk_idx = si->nEntriesInUse-1;

	// need to fetch more memory
	if (cur_chunk_idx >= si->dwSize) {
		si->dwSize += 4096;
		si->aIndex = (avistdindex_entry *)gf_realloc ( si->aIndex, si->dwSize * sizeof (u32) * si->wLongsPerEntry);
	}

	if(len>AVI->max_len) AVI->max_len=len;

	// if bit 31 is set, it is NOT a keyframe
	if (flags != 0x10) {
		len |= 0x80000000;
	}

	si->aIndex [ cur_chunk_idx ].dwSize = len;
	si->aIndex [ cur_chunk_idx ].dwOffset = (u32) (pos - si->qwBaseOffset + 8);

	//GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] ODML: POS: 0x%lX\n", si->aIndex [ cur_chunk_idx ].dwOffset));

	return 0;
}