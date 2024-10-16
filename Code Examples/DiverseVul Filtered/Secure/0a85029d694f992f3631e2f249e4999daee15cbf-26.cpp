GF_AVCConfig *gf_isom_svc_config_get(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	GF_TrackBox *trak;
	GF_MPEGVisualSampleEntryBox *entry;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !DescriptionIndex) return NULL;
	if (gf_isom_get_avc_svc_type(the_file, trackNumber, DescriptionIndex)==GF_ISOM_AVCTYPE_NONE)
		return NULL;
	entry = (GF_MPEGVisualSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex-1);
	if (!entry) return NULL;
	if (!entry->svc_config) return NULL;
	return AVC_DuplicateConfig(entry->svc_config->config);
}