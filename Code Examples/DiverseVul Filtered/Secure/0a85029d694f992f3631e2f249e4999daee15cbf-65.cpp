static GF_Err gf_isom_svc_mvc_config_del(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex, Bool is_mvc)
{
	GF_TrackBox *trak;
	GF_Err e;
	GF_MPEGVisualSampleEntryBox *entry;

	e = CanAccessMovie(the_file, GF_ISOM_OPEN_WRITE);
	if (e) return e;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !DescriptionIndex) return GF_BAD_PARAM;
	entry = (GF_MPEGVisualSampleEntryBox *)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex-1);
	if (!entry) return GF_BAD_PARAM;
	switch (entry->type) {
	case GF_ISOM_BOX_TYPE_AVC1:
	case GF_ISOM_BOX_TYPE_AVC2:
	case GF_ISOM_BOX_TYPE_AVC3:
	case GF_ISOM_BOX_TYPE_AVC4:
	case GF_ISOM_BOX_TYPE_SVC1:
	case GF_ISOM_BOX_TYPE_MVC1:
		break;
	default:
		return GF_BAD_PARAM;
	}

	if (is_mvc && entry->mvc_config) {
		gf_isom_box_del_parent(&entry->child_boxes, (GF_Box*)entry->mvc_config);
		entry->mvc_config = NULL;
	}
	else if (!is_mvc && entry->svc_config) {
		gf_isom_box_del_parent(&entry->child_boxes, (GF_Box*)entry->svc_config);
		entry->svc_config = NULL;
	}
	AVC_RewriteESDescriptor(entry);
	return GF_OK;
}