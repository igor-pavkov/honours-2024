GF_Err gf_isom_vvc_config_update_ex(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex, GF_VVCConfig *cfg, u32 operand_type, Bool keep_xps)
{
	u32 array_incomplete;
	GF_TrackBox *trak;
	GF_Err e;
	GF_MPEGVisualSampleEntryBox *entry;
	GF_SampleDescriptionBox *stsd;

	e = CanAccessMovie(the_file, GF_ISOM_OPEN_WRITE);
	if (e) return e;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Media || !DescriptionIndex) return GF_BAD_PARAM;
	stsd = trak->Media->information->sampleTable->SampleDescription;
	entry = (GF_MPEGVisualSampleEntryBox *)gf_list_get(stsd->child_boxes, DescriptionIndex-1);
	if (!entry) return GF_BAD_PARAM;
	switch (entry->type) {
	case GF_ISOM_BOX_TYPE_VVC1:
	case GF_ISOM_BOX_TYPE_VVI1:
		break;
	default:
		return GF_BAD_PARAM;
	}


	if (operand_type <= GF_ISOM_VVCC_SET_INBAND) {
		if ((operand_type != GF_ISOM_VVCC_SET_INBAND) && !entry->hevc_config) {
			entry->vvc_config = (GF_VVCConfigurationBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_VVCC);
			if (!entry->vvc_config) return GF_OUT_OF_MEM;
		}

		if (cfg) {
			if (entry->vvc_config->config) gf_odf_vvc_cfg_del(entry->vvc_config->config);
			entry->vvc_config->config = VVC_DuplicateConfig(cfg);
		} else {
			operand_type = GF_ISOM_VVCC_SET_INBAND;
		}
		array_incomplete = (operand_type==GF_ISOM_VVCC_SET_INBAND) ? 1 : 0;
		if (entry->vvc_config && nalu_cleanup_config(entry->vvc_config->config ? entry->vvc_config->config->param_array : NULL,
			(operand_type==GF_ISOM_VVCC_SET_INBAND),
			keep_xps)
		) {
			array_incomplete=1;
		}
		switch (entry->type) {
		case GF_ISOM_BOX_TYPE_VVC1:
		case GF_ISOM_BOX_TYPE_VVI1:
			entry->type = array_incomplete ? GF_ISOM_BOX_TYPE_VVI1 : GF_ISOM_BOX_TYPE_VVC1;
			break;
		}
	}
	return GF_OK;
}