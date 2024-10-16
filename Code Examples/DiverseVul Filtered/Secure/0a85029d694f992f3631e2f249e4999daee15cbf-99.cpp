static GF_Err gf_isom_svc_mvc_config_new(GF_ISOFile *the_file, u32 trackNumber, GF_AVCConfig *cfg, Bool is_mvc, char *URLname, char *URNname, u32 *outDescriptionIndex)
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
	e = Media_FindDataRef(trak->Media->information->dataInformation->dref, URLname, URNname, &dataRefIndex);
	if (e) return e;
	if (!dataRefIndex) {
		e = Media_CreateDataRef(the_file, trak->Media->information->dataInformation->dref, URLname, URNname, &dataRefIndex);
		if (e) return e;
	}
	if (!the_file->keep_utc)
		trak->Media->mediaHeader->modificationTime = gf_isom_get_mp4time();

	stsd = trak->Media->information->sampleTable->SampleDescription;
	//create a new entry
	if (is_mvc) {
		entry = (GF_MPEGVisualSampleEntryBox *) gf_isom_box_new_parent(&stsd->child_boxes, GF_ISOM_BOX_TYPE_MVC1);
		if (!entry) return GF_OUT_OF_MEM;
		entry->mvc_config = (GF_AVCConfigurationBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_MVCC);
		if (!entry->mvc_config) return GF_OUT_OF_MEM;
		entry->mvc_config->config = AVC_DuplicateConfig(cfg);
		if (!entry->mvc_config->config) return GF_OUT_OF_MEM;
	} else {
		entry = (GF_MPEGVisualSampleEntryBox *) gf_isom_box_new_parent(&stsd->child_boxes,GF_ISOM_BOX_TYPE_SVC1);
		if (!entry) return GF_OUT_OF_MEM;
		entry->svc_config = (GF_AVCConfigurationBox*)gf_isom_box_new_parent(&entry->child_boxes,GF_ISOM_BOX_TYPE_SVCC);
		if (!entry->svc_config) return GF_OUT_OF_MEM;
		entry->svc_config->config = AVC_DuplicateConfig(cfg);
		if (!entry->svc_config->config) return GF_OUT_OF_MEM;
	}
	entry->dataReferenceIndex = dataRefIndex;
	*outDescriptionIndex = gf_list_count(stsd->child_boxes);
	AVC_RewriteESDescriptor(entry);
	return e;
}