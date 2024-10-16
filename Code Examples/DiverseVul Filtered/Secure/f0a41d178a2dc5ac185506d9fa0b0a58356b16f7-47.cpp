GF_Err gf_isom_get_rvc_config(GF_ISOFile *movie, u32 track, u32 sampleDescriptionIndex, u16 *rvc_predefined, u8 **data, u32 *size, const char **mime)
{
	GF_MPEGVisualSampleEntryBox *entry;
	GF_TrackBox *trak;

	if (!rvc_predefined || !data || !size) return GF_BAD_PARAM;
	*rvc_predefined = 0;

	trak = gf_isom_get_track_from_file(movie, track);
	if (!trak) return GF_BAD_PARAM;


	entry = (GF_MPEGVisualSampleEntryBox *) gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, sampleDescriptionIndex-1);
	if (!entry ) return GF_BAD_PARAM;
	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return GF_BAD_PARAM;

	GF_RVCConfigurationBox *rvcc = (GF_RVCConfigurationBox *)gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_RVCC);
	if (!rvcc) return GF_NOT_FOUND;

	*rvc_predefined = rvcc->predefined_rvc_config;
	if (rvcc->rvc_meta_idx) {
		if (!data || !size) return GF_OK;
		return gf_isom_extract_meta_item_mem(movie, GF_FALSE, track, rvcc->rvc_meta_idx, data, size, NULL, mime, GF_FALSE);
	}
	return GF_OK;
}