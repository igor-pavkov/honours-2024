GF_Err gf_isom_avc_config_new(GF_ISOFile *the_file, u32 trackNumber, GF_AVCConfig *cfg, const char *URLname, const char *URNname, u32 *outDescriptionIndex)
{
	GF_TrackBox *trak;
	GF_Err e;
	GF_SampleDescriptionBox *stsd;
	u32 dataRefIndex;
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
	if (!the_file->keep_utc && !gf_sys_is_test_mode() )
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	stsd = trak->Media->information->sampleTable->SampleDescription;

	//create a new entry
	entry = (GF_MPEGVisualSampleEntryBox *) gf_isom_box_new_parent(&stsd->child_boxes, GF_ISOM_BOX_TYPE_AVC1);
	if (!entry) return GF_OUT_OF_MEM;
	*outDescriptionIndex = gf_list_count(stsd->child_boxes);

	entry->avc_config = (GF_AVCConfigurationBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_AVCC);
	if (!entry->avc_config) return GF_OUT_OF_MEM;
	entry->avc_config->config = AVC_DuplicateConfig(cfg);
	if (!entry->avc_config->config) return GF_OUT_OF_MEM;
	entry->dataReferenceIndex = dataRefIndex;
	AVC_RewriteESDescriptor(entry);
	return e;
}