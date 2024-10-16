GF_Err gf_isom_get_jp2_config(GF_ISOFile *movie, u32 trackNumber, u32 sampleDesc, u8 **out_dsi, u32 *out_size)
{
	GF_TrackBox *trak;
	GF_MPEGVisualSampleEntryBox *entry;
	GF_BitStream *bs;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->SampleDescription) return GF_ISOM_INVALID_FILE;
	entry = (GF_MPEGVisualSampleEntryBox *) gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, sampleDesc-1);
	if (!entry || !entry->jp2h) return GF_BAD_PARAM;
	if (!entry->jp2h->ihdr) return GF_ISOM_INVALID_FILE;

	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	gf_isom_box_array_write((GF_Box*)entry->jp2h, entry->jp2h->child_boxes, bs);
	gf_bs_get_content(bs, out_dsi, out_size);
	gf_bs_del(bs);
	return GF_OK;
}