GF_Err stts_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_TimeToSampleBox *ptr = (GF_TimeToSampleBox *)s;

#ifndef GPAC_DISABLE_ISOM_WRITE
	ptr->w_LastDTS = 0;
#endif

	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->nb_entries = gf_bs_read_u32(bs);
	if (ptr->size < ptr->nb_entries * 8) {
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
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Found stts entry with sample_delta=0 - forbidden ! Fixing to 1\n" ));
				ptr->entries[i].sampleDelta = 1;
			} else if (ptr->entries[i].sampleCount>1) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] more than one stts entry at the end of the track with sample_delta=0 - forbidden ! Fixing to 1\n" ));
				ptr->entries[i].sampleDelta = 1;
			}
		} else if ((s32) ptr->entries[i].sampleDelta < 0) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] stts entry %d has negative duration %d - forbidden ! Fixing to 1, sync may get lost (consider reimport raw media)\n", i, (s32) ptr->entries[i].sampleDelta ));
			ptr->entries[i].sampleDelta = 1;
		}
	}
	if (ptr->size<(ptr->nb_entries*8)) return GF_ISOM_INVALID_FILE;
	ISOM_DECREASE_SIZE(ptr, ptr->nb_entries*8);

	//remove the last sample delta.
#ifndef GPAC_DISABLE_ISOM_WRITE
	if (ptr->nb_entries) ptr->w_LastDTS -= ptr->entries[ptr->nb_entries-1].sampleDelta;
#endif
	return GF_OK;
}