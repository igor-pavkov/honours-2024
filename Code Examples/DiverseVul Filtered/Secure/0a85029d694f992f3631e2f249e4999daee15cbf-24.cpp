GF_ISOMAVCType gf_isom_get_avc_svc_type(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex)
{
	u32 type;
	GF_TrackBox *trak;
	GF_MPEGVisualSampleEntryBox *entry;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !trak->Media->handler || !DescriptionIndex) return GF_ISOM_AVCTYPE_NONE;
	if (!gf_isom_is_video_handler_type(trak->Media->handler->handlerType))
		return GF_ISOM_AVCTYPE_NONE;

	entry = (GF_MPEGVisualSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, DescriptionIndex-1);
	if (!entry) return GF_ISOM_AVCTYPE_NONE;

	type = entry->type;

	if (type == GF_ISOM_BOX_TYPE_ENCV) {
		GF_ProtectionSchemeInfoBox *sinf = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_SINF);
		if (sinf && sinf->original_format) type = sinf->original_format->data_format;
	}
	else if (type == GF_ISOM_BOX_TYPE_RESV) {
		if (entry->rinf && entry->rinf->original_format) type = entry->rinf->original_format->data_format;
	}

	switch (type) {
	case GF_ISOM_BOX_TYPE_AVC1:
	case GF_ISOM_BOX_TYPE_AVC2:
	case GF_ISOM_BOX_TYPE_AVC3:
	case GF_ISOM_BOX_TYPE_AVC4:
	case GF_ISOM_BOX_TYPE_SVC1:
	case GF_ISOM_BOX_TYPE_MVC1:
		break;
	default:
		return GF_ISOM_AVCTYPE_NONE;
	}
	if (entry->avc_config && !entry->svc_config && !entry->mvc_config) return GF_ISOM_AVCTYPE_AVC_ONLY;
	if (entry->avc_config && entry->svc_config) return GF_ISOM_AVCTYPE_AVC_SVC;
	if (entry->avc_config && entry->mvc_config) return GF_ISOM_AVCTYPE_AVC_MVC;
	if (!entry->avc_config && entry->svc_config) return GF_ISOM_AVCTYPE_SVC_ONLY;
	if (!entry->avc_config && entry->mvc_config) return GF_ISOM_AVCTYPE_MVC_ONLY;
	return GF_ISOM_AVCTYPE_NONE;
}