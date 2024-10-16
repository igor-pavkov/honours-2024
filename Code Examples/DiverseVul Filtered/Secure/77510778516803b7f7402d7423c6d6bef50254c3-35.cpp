GF_Err stts_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	Bool logged=GF_FALSE;
	GF_TimeToSampleBox *ptr = (GF_TimeToSampleBox *)s;

#ifndef GPAC_DISABLE_ISOM_WRITE
	ptr->w_LastDTS = 0;
#endif

	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->nb_entries = gf_bs_read_u32(bs);
	if (ptr->size / 8 < ptr->nb_entries || (u64)ptr->nb_entries > (u64)SIZE_MAX/sizeof(GF_SttsEntry)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in stts\n", ptr->nb_entries));
		return GF_ISOM_INVALID_FILE;
	}

	ptr->alloc_size = ptr->nb_entries;
	ptr->entries = gf_malloc(sizeof(GF_SttsEntry)*ptr->alloc_size);
	if (!ptr->entries) return GF_OUT_OF_MEM;

	for (i=0; i<ptr->nb_entries; i++) {
		ptr->entries[i].sampleCount = gf_bs_read_u32(bs);
		ptr->entries[i].sampleDelta = gf_bs_read_u32(bs);
#ifndef GPAC_DISABLE_ISOM_WRITE
		ptr->w_currentSampleNum += ptr->entries[i].sampleCount;
		ptr->w_LastDTS += (u64)ptr->entries[i].sampleCount * ptr->entries[i].sampleDelta;
#endif
		if (ptr->max_ts_delta<ptr->entries[i].sampleDelta)
			ptr->max_ts_delta = ptr->entries[i].sampleDelta;

		if (!ptr->entries[i].sampleDelta) {
			if ((i+1<ptr->nb_entries) ) {
				if (!logged) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Found stts entry with sample_delta=0 - forbidden ! Fixing to 1\n" ));
					logged=GF_TRUE;
				}
				ptr->entries[i].sampleDelta = 1;
			} else if (ptr->entries[i].sampleCount>1) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] more than one stts entry at the end of the track with sample_delta=0 - forbidden ! Fixing to 1\n" ));
				ptr->entries[i].sampleDelta = 1;
			}
		}
		//cf issue 1644: some media streams may have sample duration > 2^31 (ttml mostly), we cannot patch this
		//for now we disable the check, one opt could be to have the check only for some media types, or only for the first entry
#if 0
		else if ((s32) ptr->entries[i].sampleDelta < 0) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] stts entry %d has negative duration %d - forbidden ! Fixing to 1, sync may get lost (consider reimport raw media)\n", i, (s32) ptr->entries[i].sampleDelta ));
			ptr->entries[i].sampleDelta = 1;
		}
#endif

	}
	ISOM_DECREASE_SIZE(ptr, ptr->nb_entries*8);

	//remove the last sample delta.
#ifndef GPAC_DISABLE_ISOM_WRITE
	if (ptr->nb_entries) ptr->w_LastDTS -= ptr->entries[ptr->nb_entries-1].sampleDelta;
#endif
	return GF_OK;
}