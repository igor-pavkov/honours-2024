static void gf_isom_recreate_tables(GF_TrackBox *trak)
{
	u32 j;
	GF_Box *a;
	GF_SampleTableBox *stbl = trak->Media->information->sampleTable;

	if (stbl->ChunkOffset) {
		if (stbl->ChunkOffset->type==GF_ISOM_BOX_TYPE_CO64) {
			GF_ChunkLargeOffsetBox *co64 = (GF_ChunkLargeOffsetBox *)stbl->ChunkOffset;
			co64->nb_entries = 0;
		} else {
			GF_ChunkOffsetBox *stco = (GF_ChunkOffsetBox *)stbl->ChunkOffset;
			stco->nb_entries = 0;
		}
	}

	if (stbl->CompositionOffset) {
		stbl->CompositionOffset->nb_entries = 0;
		stbl->CompositionOffset->w_LastSampleNumber = 0;
		stbl->CompositionOffset->r_currentEntryIndex = 0;
		stbl->CompositionOffset->r_FirstSampleInEntry = 0;
		stbl->CompositionOffset->max_cts_delta = 0;
	}

	if (stbl->DegradationPriority) {
		stbl->DegradationPriority->nb_entries = 0;
	}

	if (stbl->PaddingBits) {
		stbl->PaddingBits->SampleCount = 0;
	}

	if (stbl->SampleDep) {
		stbl->SampleDep->sampleCount = 0;
	}

	if (stbl->SampleSize) {
		stbl->SampleSize->sampleSize = 0;
		stbl->SampleSize->sampleCount = 0;
	}

	if (stbl->SampleToChunk) {
		stbl->SampleToChunk->nb_entries = 0;
		stbl->SampleToChunk->currentIndex = 0;
		stbl->SampleToChunk->firstSampleInCurrentChunk = 0;
		stbl->SampleToChunk->currentChunk = 0;
		stbl->SampleToChunk->ghostNumber = 0;
		stbl->SampleToChunk->w_lastSampleNumber = 0;
		stbl->SampleToChunk->w_lastChunkNumber = 0;
	}

	if (stbl->ShadowSync) {
        gf_isom_box_del_parent(&stbl->child_boxes, (GF_Box *) stbl->ShadowSync);
        stbl->ShadowSync = (GF_ShadowSyncBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSH);
	}

	if (stbl->SyncSample) {
		stbl->SyncSample->nb_entries = 0;
		stbl->SyncSample->r_LastSyncSample = 0;
		stbl->SyncSample->r_LastSampleIndex = 0;
	}

	if (stbl->TimeToSample) {
		stbl->TimeToSample->nb_entries = 0;
		stbl->TimeToSample->r_FirstSampleInEntry = 0;
		stbl->TimeToSample->r_currentEntryIndex = 0;
		stbl->TimeToSample->r_CurrentDTS = 0;
	}

	gf_isom_box_array_del_parent(&stbl->child_boxes, stbl->sai_offsets);
	stbl->sai_offsets = NULL;

	gf_isom_box_array_del_parent(&stbl->child_boxes, stbl->sai_sizes);
	stbl->sai_sizes = NULL;

	gf_isom_box_array_del_parent(&stbl->child_boxes, stbl->sampleGroups);
	stbl->sampleGroups = NULL;

	if (trak->sample_encryption) {
		if (trak->Media->information->sampleTable->child_boxes) {
			gf_list_del_item(trak->Media->information->sampleTable->child_boxes, trak->sample_encryption);
		}
		gf_isom_box_del_parent(&trak->child_boxes, (GF_Box*)trak->sample_encryption);
		trak->sample_encryption = NULL;
	}

	j = stbl->nb_sgpd_in_stbl;
	while ((a = (GF_Box *)gf_list_enum(stbl->sampleGroupsDescription, &j))) {
		gf_isom_box_del_parent(&stbl->child_boxes, a);
		j--;
		gf_list_rem(stbl->sampleGroupsDescription, j);
	}

	if (stbl->traf_map) {
		for (j=0; j<stbl->traf_map->nb_entries; j++) {
			if (stbl->traf_map->frag_starts[j].moof_template)
				gf_free(stbl->traf_map->frag_starts[j].moof_template);
		}
		memset(stbl->traf_map->frag_starts, 0, sizeof(GF_TrafMapEntry)*stbl->traf_map->nb_alloc);
		stbl->traf_map->nb_entries = 0;
	}
}