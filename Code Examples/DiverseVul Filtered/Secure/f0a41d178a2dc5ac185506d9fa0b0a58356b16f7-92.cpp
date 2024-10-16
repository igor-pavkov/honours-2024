u32 gf_isom_get_nalu_length_field(GF_ISOFile *file, u32 track, u32 StreamDescriptionIndex)
{
	GF_TrackBox *trak;
	GF_SampleEntryBox *entry;
	GF_MPEGVisualSampleEntryBox *ve;
	GF_SampleDescriptionBox *stsd;

	trak = gf_isom_get_track_from_file(file, track);
	if (!trak) {
		file->LastError = GF_BAD_PARAM;
		return 0;
	}

	stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd || !StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) {
		file->LastError = GF_BAD_PARAM;
		return 0;
	}

	entry = (GF_SampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (!entry || ! gf_isom_is_nalu_based_entry(trak->Media, entry)) {
		file->LastError = GF_BAD_PARAM;
		return 0;
	}

	ve = (GF_MPEGVisualSampleEntryBox*)entry;
	if (ve->avc_config) return ve->avc_config->config->nal_unit_size;
	if (ve->svc_config) return ve->svc_config->config->nal_unit_size;
	if (ve->hevc_config) return ve->hevc_config->config->nal_unit_size;
	if (ve->lhvc_config) return ve->lhvc_config->config->nal_unit_size;
	return 0;
}