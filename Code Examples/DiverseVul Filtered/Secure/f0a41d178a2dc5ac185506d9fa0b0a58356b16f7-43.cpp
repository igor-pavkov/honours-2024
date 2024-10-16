const GF_ContentLightLevelInfo *gf_isom_get_content_light_level_info(GF_ISOFile* movie, u32 trackNumber, u32 StreamDescriptionIndex)
{
	GF_TrackBox* trak;
	GF_SampleEntryBox* entry;
	GF_SampleDescriptionBox* stsd;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return NULL;

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) return NULL;
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) {
		return NULL;
	}
	entry = (GF_SampleEntryBox*)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	if (entry == NULL) return NULL;
	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_VIDEO) return NULL;

	GF_ContentLightLevelBox *cllib = (GF_ContentLightLevelBox *)gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_CLLI);
	if (!cllib) return NULL;
	return &cllib->clli;
}