GF_HEVCConfig *gf_isom_hevc_config_get(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	GF_TrackBox *trak;
	GF_MPEGVisualSampleEntryBox *entry;
	if (gf_isom_get_reference_count(the_file, trackNumber, GF_ISOM_REF_TBAS)) {
		u32 ref_track;
		GF_Err e = gf_isom_get_reference(the_file, trackNumber, GF_ISOM_REF_TBAS, 1, &ref_track);
		if (e == GF_OK) {
			trackNumber = ref_track;
		}
	}
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !DescriptionIndex) return NULL;
	if (gf_isom_get_hevc_lhvc_type(the_file, trackNumber, DescriptionIndex)==GF_ISOM_HEVCTYPE_NONE)
		return NULL;
	entry = (GF_MPEGVisualSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex-1);
	if (!entry) return NULL;
	if (!entry->hevc_config) return NULL;
	return HEVC_DuplicateConfig(entry->hevc_config->config);
}