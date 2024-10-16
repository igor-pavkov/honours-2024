static GF_Err store_meta_item_sample_ref_offsets(GF_ISOFile *movie, GF_List *writers, GF_MetaBox *meta)
{
	u32 i, count;
	TrackWriter *writer;
	GF_Box *stco;
	GF_SampleToChunkBox *stsc;

	if (!movie->moov) return GF_OK;
	if (!meta->item_locations) return GF_OK;
	if (!meta->use_item_sample_sharing) return GF_OK;

	//switch all our tables
	i=0;
	while ((writer = (TrackWriter*)gf_list_enum(writers, &i))) {
		//don't delete them !!!
		stsc = writer->stbl->SampleToChunk;
		stco = writer->stbl->ChunkOffset;
		s32 stsc_pos = gf_list_del_item(writer->stbl->child_boxes, stsc);
		s32 stco_pos = gf_list_del_item(writer->stbl->child_boxes, stco);
		writer->stbl->SampleToChunk = writer->stsc;
		writer->stbl->ChunkOffset = writer->stco;
		gf_list_insert(writer->stbl->child_boxes, writer->stsc, stsc_pos);
		gf_list_insert(writer->stbl->child_boxes, writer->stco, stco_pos);
		writer->stco = stco;
		writer->stsc = stsc;
	}

	count = gf_list_count(meta->item_locations->location_entries);
	for (i=0; i<count; i++) {
		u32 j;
		GF_ItemExtentEntry *entry;
		GF_ItemLocationEntry *iloc = (GF_ItemLocationEntry *)gf_list_get(meta->item_locations->location_entries, i);
		/*get item info*/
		GF_ItemInfoEntryBox *iinf = NULL;
		j=0;
		while ((iinf = (GF_ItemInfoEntryBox *)gf_list_enum(meta->item_infos->item_infos, &j))) {
			if (iinf->item_ID==iloc->item_ID) break;
		}
		if (!iinf || !iinf->tk_id || !iinf->sample_num) continue;

		entry = (GF_ItemExtentEntry *)gf_list_get(iloc->extent_entries, 0);
		if (!entry) continue;

		GF_ISOSample *samp = gf_isom_get_sample_info(movie, gf_isom_get_track_by_id(movie, iinf->tk_id), iinf->sample_num, NULL, &entry->extent_offset);
		if (samp) gf_isom_sample_del(&samp);
		entry->extent_offset -= iloc->base_offset;
	}

	//and re-switch our table. We have to do it that way because it is
	//needed when the moov is written first
	i=0;
	while ((writer = (TrackWriter*)gf_list_enum(writers, &i))) {
		//don't delete them !!!
		stsc = writer->stsc;
		stco = writer->stco;
		writer->stsc = writer->stbl->SampleToChunk;
		writer->stco = writer->stbl->ChunkOffset;
		s32 stsc_pos = gf_list_del_item(writer->stbl->child_boxes, writer->stsc);
		s32 stco_pos = gf_list_del_item(writer->stbl->child_boxes, writer->stco);

		writer->stbl->SampleToChunk = stsc;
		writer->stbl->ChunkOffset = stco;
		gf_list_insert(writer->stbl->child_boxes, stsc, stsc_pos);
		gf_list_insert(writer->stbl->child_boxes, stco, stco_pos);
	}
	return GF_OK;
}