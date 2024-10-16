static GF_Err ShiftOffset(GF_ISOFile *file, GF_List *writers, u64 offset)
{
	u32 i, j, k, l, last;
	TrackWriter *writer;
	GF_StscEntry *ent;

	if (file->meta) ShiftMetaOffset(file->meta, offset);
	if (file->moov && file->moov->meta) ShiftMetaOffset(file->moov->meta, offset);

	i=0;
	while ((writer = (TrackWriter *)gf_list_enum(writers, &i))) {
		if (writer->mdia->mediaTrack->meta) ShiftMetaOffset(writer->mdia->mediaTrack->meta, offset);

		//we have to proceed entry by entry in case a part of the media is not self-contained...
		for (j=0; j<writer->stsc->nb_entries; j++) {
			ent = &writer->stsc->entries[j];
			if ((writer->all_dref_mode==ISOM_DREF_EXT) || !Media_IsSelfContained(writer->mdia, ent->sampleDescriptionIndex))
				continue;

			//OK, get the chunk(s) number(s) and "shift" its (their) offset(s).
			if (writer->stco->type == GF_ISOM_BOX_TYPE_STCO) {
				GF_ChunkLargeOffsetBox *new_stco64 = NULL;
				GF_ChunkOffsetBox *stco = (GF_ChunkOffsetBox *) writer->stco;

				//be carefull for the last entry, nextChunk is set to 0 in edit mode...
				last = ent->nextChunk ? ent->nextChunk : stco->nb_entries + 1;
				for (k = ent->firstChunk; k < last; k++) {

					//we need to rewrite the table: only allocate co64 if not done previously and convert all offsets
					//to co64. Then (whether co64 was created or not) adjust the offset
					//Do not reassign table until we are done with the current sampleToChunk processing
					//since we have a test on stco->offsets[k-1], we need to keep stco untouched
					if (new_stco64 || file->force_co64 || (stco->offsets[k-1] + offset > 0xFFFFFFFF)) {
						if (!new_stco64) {
							new_stco64 = (GF_ChunkLargeOffsetBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_CO64);
							if (!new_stco64) return GF_OUT_OF_MEM;
							new_stco64->nb_entries = stco->nb_entries;
							new_stco64->offsets = (u64 *) gf_malloc(new_stco64->nb_entries * sizeof(u64));
							if (!new_stco64->offsets) return GF_OUT_OF_MEM;
							//copy over the stco table
							for (l = 0; l < new_stco64->nb_entries; l++) {
								new_stco64->offsets[l] = (u64) stco->offsets[l];
							}
						}
						new_stco64->offsets[k-1] += offset;
					} else {
						stco->offsets[k-1] += (u32) offset;
					}
				}
				if (new_stco64) {
					//done with this sampleToChunk entry, replace the box if we moved to co64
					gf_isom_box_del(writer->stco);
					writer->stco = (GF_Box *)new_stco64;
					new_stco64 = NULL;
				}
			} else {
				GF_ChunkLargeOffsetBox *stco64 = (GF_ChunkLargeOffsetBox *) writer->stco;
				//be carefull for the last entry ...
				last = ent->nextChunk ? ent->nextChunk : stco64->nb_entries + 1;
				for (k = ent->firstChunk; k < last; k++) {
					stco64->offsets[k-1] += offset;
				}
			}
		}
	}

	return GF_OK;

}