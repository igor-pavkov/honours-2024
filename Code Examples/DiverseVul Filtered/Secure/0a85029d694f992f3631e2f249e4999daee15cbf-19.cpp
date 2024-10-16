GF_Err gf_isom_vp_config_new(GF_ISOFile *the_file, u32 trackNumber, GF_VPConfig *cfg, const char *URLname, const char *URNname, u32 *outDescriptionIndex, u32 vpx_type)
{
	GF_TrackBox *trak;
	GF_Err e;
	u32 dataRefIndex;
	GF_SampleDescriptionBox *stsd;
	GF_MPEGVisualSampleEntryBox *entry;

	e = CanAccessMovie(the_file, GF_ISOM_OPEN_WRITE);
	if (e) return e;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !cfg) return GF_BAD_PARAM;

	//get or create the data ref
	e = Media_FindDataRef(trak->Media->information->dataInformation->dref, (char *)URLname, (char *)URNname, &dataRefIndex);
	if (e) return e;
	if (!dataRefIndex) {
		e = Media_CreateDataRef(the_file, trak->Media->information->dataInformation->dref, (char *)URLname, (char *)URNname, &dataRefIndex);
		if (e) return e;
	}
	if (!the_file->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	stsd = trak->Media->information->sampleTable->SampleDescription;
	//create a new entry
	entry = (GF_MPEGVisualSampleEntryBox *)gf_isom_box_new_parent(&stsd->child_boxes, vpx_type);
	if (!entry) return GF_OUT_OF_MEM;
	
	entry->vp_config = (GF_VPConfigurationBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_VPCC);
	if (!entry->vp_config) return GF_OUT_OF_MEM;
	entry->vp_config->config = VP_DuplicateConfig(cfg);
	if (!entry->vp_config->config) return GF_OUT_OF_MEM;
	strncpy(entry->compressor_name, "\012VPC Coding", sizeof(entry->compressor_name)-1);
	entry->dataReferenceIndex = dataRefIndex;
	*outDescriptionIndex = gf_list_count(stsd->child_boxes);
	return e;
}