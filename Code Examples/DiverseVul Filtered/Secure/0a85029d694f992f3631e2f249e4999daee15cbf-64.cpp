GF_VVCConfig *gf_isom_vvc_config_get(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	GF_TrackBox *trak;
	GF_MPEGVisualSampleEntryBox *entry;
	/*todo, add support for subpic track and nvcl tracks*/
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !DescriptionIndex) return NULL;
	if (gf_isom_get_vvc_type(the_file, trackNumber, DescriptionIndex)==GF_ISOM_VVCTYPE_NONE)
		return NULL;
	entry = (GF_MPEGVisualSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex-1);
	if (!entry) return NULL;
	if (!entry->vvc_config) return NULL;
	return VVC_DuplicateConfig(entry->vvc_config->config);
}