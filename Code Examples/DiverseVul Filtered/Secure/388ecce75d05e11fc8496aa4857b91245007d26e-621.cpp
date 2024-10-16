GF_Err video_sample_entry_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_MPEGVisualSampleEntryBox *ptr = (GF_MPEGVisualSampleEntryBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_ESDS:
		if (ptr->esd) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->esd = (GF_ESDBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_RINF:
		if (ptr->rinf) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->rinf = (GF_RestrictedSchemeInfoBox *) a;
		break;
	case GF_ISOM_BOX_TYPE_AVCC:
		if (ptr->avc_config) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->avc_config = (GF_AVCConfigurationBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_HVCC:
		if (ptr->hevc_config) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->hevc_config = (GF_HEVCConfigurationBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_SVCC:
		if (ptr->svc_config) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->svc_config = (GF_AVCConfigurationBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_MVCC:
		if (ptr->mvc_config) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->mvc_config = (GF_AVCConfigurationBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_LHVC:
		if (ptr->lhvc_config) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->lhvc_config = (GF_HEVCConfigurationBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_AV1C:
		if (ptr->av1_config) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->av1_config = (GF_AV1ConfigurationBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_VPCC:
		if (ptr->vp_config) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->vp_config = (GF_VPConfigurationBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_DVCC:
		if (ptr->dovi_config) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->dovi_config = (GF_DOVIConfigurationBox*)a;
		break;
	case GF_ISOM_BOX_TYPE_UUID:
		if (! memcmp(((GF_UnknownUUIDBox*)a)->uuid, GF_ISOM_IPOD_EXT, 16)) {
			if (ptr->ipod_ext) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->ipod_ext = (GF_UnknownUUIDBox *)a;
		} else {
			return GF_OK;
		}
		break;
	case GF_ISOM_BOX_TYPE_D263:
		if (ptr->cfg_3gpp) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->cfg_3gpp = (GF_3GPPConfigBox *)a;
		/*for 3GP config, remember sample entry type in config*/
		ptr->cfg_3gpp->cfg.type = ptr->type;
		break;

	case GF_ISOM_BOX_TYPE_JP2H:
		if (ptr->jp2h) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->jp2h = (GF_J2KHeaderBox *)a;
		return GF_OK;

	case GF_ISOM_BOX_TYPE_PASP:
	case GF_ISOM_BOX_TYPE_CLAP:
	case GF_ISOM_BOX_TYPE_COLR:
	case GF_ISOM_BOX_TYPE_MDCV:
	case GF_ISOM_BOX_TYPE_CLLI:
	case GF_ISOM_BOX_TYPE_CCST:
	case GF_ISOM_BOX_TYPE_AUXI:
	case GF_ISOM_BOX_TYPE_RVCC:
	case GF_ISOM_BOX_TYPE_M4DS:
		if (!gf_isom_box_check_unique(s->child_boxes, a)) {
			ERROR_ON_DUPLICATED_BOX(a, ptr)
		}
		return GF_OK;
	}
	return GF_OK;
}