static int avi_ixnn_entry(avi_t *AVI, avistdindex_chunk *ch, avisuperindex_entry *en)
{
	int bl;
	u32 k;
	unsigned int max = ch->nEntriesInUse * sizeof (u32) * ch->wLongsPerEntry + 24; // header
	char *ix00 = (char *)gf_malloc (max);
	char dfcc[5];
	memcpy (dfcc, ch->fcc, 4);
	dfcc[4] = 0;

	bl = 0;

	if (en) {
		en->qwOffset = AVI->pos;
		en->dwSize = max;
		//en->dwDuration = ch->nEntriesInUse -1; // NUMBER OF stream ticks == frames for video/samples for audio
	}

#ifdef DEBUG_ODML
	//GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[avilib] ODML Write %s: Entries %ld size %d \n", dfcc, ch->nEntriesInUse, max));
#endif

	//OUTS(ch->fcc);
	//OUTD(max);
	OUTW(ch->wLongsPerEntry);
	OUTC(ch->bIndexSubType);
	OUTC(ch->bIndexType);
	OUTD(ch->nEntriesInUse);
	OUTS(ch->dwChunkId);
	OUTD(ch->qwBaseOffset&0xffffffff);
	OUTD((ch->qwBaseOffset>>32)&0xffffffff);
	OUTD(ch->dwReserved3);

	for (k = 0; k < ch->nEntriesInUse; k++) {
		OUTD(ch->aIndex[k].dwOffset);
		OUTD(ch->aIndex[k].dwSize);

	}
	avi_add_chunk (AVI, (unsigned char*)ch->fcc, (unsigned char*)ix00, max);

	gf_free(ix00);

	return 0;
}