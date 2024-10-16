Bool gf_isom_is_nalu_based_entry(GF_MediaBox *mdia, GF_SampleEntryBox *_entry)
{
	GF_MPEGVisualSampleEntryBox *entry;
	if (!gf_isom_is_video_handler_type(mdia->handler->handlerType))
		return GF_FALSE;
	if (!_entry) return GF_FALSE;
	entry = (GF_MPEGVisualSampleEntryBox*)_entry;

	switch (_entry->type) {
	case GF_ISOM_BOX_TYPE_AVC1:
	case GF_ISOM_BOX_TYPE_AVC2:
	case GF_ISOM_BOX_TYPE_AVC3:
	case GF_ISOM_BOX_TYPE_AVC4:
	case GF_ISOM_BOX_TYPE_SVC1:
	case GF_ISOM_BOX_TYPE_SVC2:
	case GF_ISOM_BOX_TYPE_MVC1:
	case GF_ISOM_BOX_TYPE_MVC2:
	case GF_ISOM_BOX_TYPE_HVC1:
	case GF_ISOM_BOX_TYPE_HEV1:
	case GF_ISOM_BOX_TYPE_HVC2:
	case GF_ISOM_BOX_TYPE_HEV2:
	case GF_ISOM_BOX_TYPE_LHV1:
	case GF_ISOM_BOX_TYPE_LHE1:
	case GF_ISOM_BOX_TYPE_MHV1:
	case GF_ISOM_BOX_TYPE_MHC1:
	case GF_ISOM_BOX_TYPE_HVT1:
	case GF_ISOM_BOX_TYPE_LHT1:
		return GF_TRUE;
	case GF_ISOM_BOX_TYPE_GNRV:
	case GF_ISOM_BOX_TYPE_GNRA:
	case GF_ISOM_BOX_TYPE_GNRM:
		return GF_FALSE;
	default:
		break;
	}

	if (!gf_isom_is_video_handler_type(entry->internal_type))
		return GF_FALSE;

	if (entry->avc_config || entry->svc_config || entry->mvc_config || entry->hevc_config || entry->lhvc_config) {
		GF_ProtectionSchemeInfoBox *schi = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_SINF);
		if (!schi || !schi->scheme_type) return GF_TRUE;
		switch (schi->scheme_type->scheme_type) {
		case GF_ISOM_CENC_SCHEME:
		case GF_ISOM_CBC_SCHEME:
		case GF_ISOM_CENS_SCHEME:
		case GF_ISOM_CBCS_SCHEME:
			return GF_TRUE;
		default:
			break;
		}
	}
	return GF_FALSE;
}