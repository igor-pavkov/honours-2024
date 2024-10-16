GF_Err video_sample_entry_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_MPEGVisualSampleEntryBox *ptr = (GF_MPEGVisualSampleEntryBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_ESDS:
		BOX_FIELD_ASSIGN(esd, GF_ESDBox)
		break;
	case GF_ISOM_BOX_TYPE_RINF:
		BOX_FIELD_ASSIGN(rinf, GF_RestrictedSchemeInfoBox)
		break;
	case GF_ISOM_BOX_TYPE_AVCC:
		BOX_FIELD_ASSIGN(avc_config, GF_AVCConfigurationBox)
		break;
	case GF_ISOM_BOX_TYPE_HVCC:
		BOX_FIELD_ASSIGN(hevc_config, GF_HEVCConfigurationBox)
		break;
	case GF_ISOM_BOX_TYPE_VVCC:
		BOX_FIELD_ASSIGN(vvc_config, GF_VVCConfigurationBox)
		break;
	case GF_ISOM_BOX_TYPE_SVCC:
		BOX_FIELD_ASSIGN(svc_config, GF_AVCConfigurationBox)
		break;
	case GF_ISOM_BOX_TYPE_MVCC:
		BOX_FIELD_ASSIGN(mvc_config, GF_AVCConfigurationBox)
		break;
	case GF_ISOM_BOX_TYPE_LHVC:
		BOX_FIELD_ASSIGN(lhvc_config, GF_HEVCConfigurationBox)
		break;
	case GF_ISOM_BOX_TYPE_AV1C:
		BOX_FIELD_ASSIGN(av1_config, GF_AV1ConfigurationBox)
		break;
	case GF_ISOM_BOX_TYPE_VPCC:
		BOX_FIELD_ASSIGN(vp_config, GF_VPConfigurationBox)
		break;
	case GF_ISOM_BOX_TYPE_DVCC:
	case GF_ISOM_BOX_TYPE_DVVC:
		BOX_FIELD_ASSIGN(dovi_config, GF_DOVIConfigurationBox)
		break;
	case GF_ISOM_BOX_TYPE_UUID:
		if (! memcmp(((GF_UnknownUUIDBox*)a)->uuid, GF_ISOM_IPOD_EXT, 16)) {
			BOX_FIELD_ASSIGN(ipod_ext, GF_UnknownUUIDBox)
		} else {
			return GF_OK;
		}
		break;
	case GF_ISOM_BOX_TYPE_D263:
		BOX_FIELD_ASSIGN(cfg_3gpp, GF_3GPPConfigBox)
		/*for 3GP config, remember sample entry type in config*/
		if (ptr->cfg_3gpp)
			ptr->cfg_3gpp->cfg.type = ptr->type;
		break;

	case GF_ISOM_BOX_TYPE_JP2H:
		BOX_FIELD_ASSIGN(jp2h, GF_J2KHeaderBox)
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
		if (!is_rem && !gf_isom_box_check_unique(s->child_boxes, a)) {
			ERROR_ON_DUPLICATED_BOX(a, ptr)
		}
		return GF_OK;
	}
	return GF_OK;
}