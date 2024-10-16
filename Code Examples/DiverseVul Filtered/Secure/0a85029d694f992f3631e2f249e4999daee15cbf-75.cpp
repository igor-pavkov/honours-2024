GF_DOVIDecoderConfigurationRecord *gf_isom_dovi_config_get(GF_ISOFile* the_file, u32 trackNumber, u32 DescriptionIndex)
{
	GF_TrackBox* trak;
	GF_MPEGVisualSampleEntryBox *entry;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !DescriptionIndex) return NULL;
	entry = (GF_MPEGVisualSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex - 1);
	if (!entry || !entry->dovi_config) return NULL;
	return DOVI_DuplicateConfig(&entry->dovi_config->DOVIConfig);
}