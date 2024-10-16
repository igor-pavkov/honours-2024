static GF_Err gf_isom_avc_config_update_ex(GF_ISOFile *the_file, u32 trackNumber, u32 DescriptionIndex, GF_AVCConfig *cfg, u32 op_type, Bool keep_xps)
{
	GF_TrackBox *trak;
	GF_Err e;
	u32 i;
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

	switch (op_type) {
	/*AVCC replacement*/
	case 0:
		if (!cfg) return GF_BAD_PARAM;
		if (!entry->avc_config) {
			entry->avc_config = (GF_AVCConfigurationBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_AVCC);
			if (!entry->avc_config) return GF_OUT_OF_MEM;
		}
		if (entry->avc_config->config) gf_odf_avc_cfg_del(entry->avc_config->config);
		entry->avc_config->config = AVC_DuplicateConfig(cfg);
		if (!entry->avc_config->config) return GF_OUT_OF_MEM;
		entry->type = GF_ISOM_BOX_TYPE_AVC1;
		break;
	/*SVCC replacement*/
	case 1:
		if (!cfg) return GF_BAD_PARAM;
		if (!entry->svc_config) {
			entry->svc_config = (GF_AVCConfigurationBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_SVCC);
			if (!entry->svc_config) return GF_OUT_OF_MEM;
		}
		if (entry->svc_config->config) gf_odf_avc_cfg_del(entry->svc_config->config);
		entry->svc_config->config = AVC_DuplicateConfig(cfg);
		if (!entry->svc_config->config) return GF_OUT_OF_MEM;
		entry->type = GF_ISOM_BOX_TYPE_AVC1;
		break;
	/*SVCC replacement and AVC removal*/
	case 2:
		if (!cfg) return GF_BAD_PARAM;
		if (entry->avc_config) {
			gf_isom_box_del_parent(&entry->child_boxes, (GF_Box*)entry->avc_config);
			entry->avc_config = NULL;
		}
		if (!entry->svc_config) {
			entry->svc_config = (GF_AVCConfigurationBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_SVCC);
			if (!entry->svc_config) return GF_OUT_OF_MEM;
		}
		if (entry->svc_config->config) gf_odf_avc_cfg_del(entry->svc_config->config);
		entry->svc_config->config = AVC_DuplicateConfig(cfg);
		if (!entry->svc_config->config) return GF_OUT_OF_MEM;
		entry->type = GF_ISOM_BOX_TYPE_SVC1;
		break;
	/*AVCC removal and switch to avc3*/
	case 3:
		if (!entry->avc_config || !entry->avc_config->config)
			return GF_BAD_PARAM;
		if (!keep_xps) {
			for (i=0; i<3; i++) {
				GF_AVCConfigurationBox *a_cfg = entry->avc_config;
				if (i==1) a_cfg = entry->svc_config;
				else if (i==2) a_cfg = entry->mvc_config;
				if (!a_cfg) continue;


				while (gf_list_count(a_cfg->config->sequenceParameterSets)) {
					GF_NALUFFParam *sl = (GF_NALUFFParam*)gf_list_get(a_cfg->config->sequenceParameterSets, 0);
					gf_list_rem(a_cfg->config->sequenceParameterSets, 0);
					if (sl->data) gf_free(sl->data);
					gf_free(sl);
				}

				while (gf_list_count(a_cfg->config->pictureParameterSets)) {
					GF_NALUFFParam *sl = (GF_NALUFFParam*)gf_list_get(a_cfg->config->pictureParameterSets, 0);
					gf_list_rem(a_cfg->config->pictureParameterSets, 0);
					if (sl->data) gf_free(sl->data);
					gf_free(sl);
				}

				while (gf_list_count(a_cfg->config->sequenceParameterSetExtensions)) {
					GF_NALUFFParam *sl = (GF_NALUFFParam*)gf_list_get(a_cfg->config->sequenceParameterSetExtensions, 0);
					gf_list_rem(a_cfg->config->sequenceParameterSetExtensions, 0);
					if (sl->data) gf_free(sl->data);
					gf_free(sl);
				}
			}
		}

		if (entry->type == GF_ISOM_BOX_TYPE_AVC1)
			entry->type = GF_ISOM_BOX_TYPE_AVC3;
		else if (entry->type == GF_ISOM_BOX_TYPE_AVC2)
			entry->type = GF_ISOM_BOX_TYPE_AVC4;
		break;
	/*MVCC replacement*/
	case 4:
		if (!cfg) return GF_BAD_PARAM;
		if (!entry->mvc_config) {
			entry->mvc_config = (GF_AVCConfigurationBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_MVCC);
			if (!entry->mvc_config) return GF_OUT_OF_MEM;
		}
		if (entry->mvc_config->config) gf_odf_avc_cfg_del(entry->mvc_config->config);
		entry->mvc_config->config = AVC_DuplicateConfig(cfg);
		if (!entry->mvc_config->config) return GF_OUT_OF_MEM;
		entry->type = GF_ISOM_BOX_TYPE_AVC1;
		e = gf_isom_check_mvc(the_file, trak, entry);
		if (e) return e;
		break;
	/*MVCC replacement and AVC removal*/
	case 5:
		if (!cfg) return GF_BAD_PARAM;
		if (entry->avc_config) {
			gf_isom_box_del_parent(&entry->child_boxes, (GF_Box*)entry->avc_config);
			entry->avc_config = NULL;
		}
		if (!entry->mvc_config) {
			entry->mvc_config = (GF_AVCConfigurationBox*)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_MVCC);
			if (!entry->mvc_config) return GF_OUT_OF_MEM;
		}
		if (entry->mvc_config->config) gf_odf_avc_cfg_del(entry->mvc_config->config);
		entry->mvc_config->config = AVC_DuplicateConfig(cfg);
		if (!entry->mvc_config->config) return GF_OUT_OF_MEM;
		entry->type = GF_ISOM_BOX_TYPE_MVC1;
		e = gf_isom_check_mvc(the_file, trak, entry);
		if (e) return e;
		break;
	}
	AVC_RewriteESDescriptor(entry);
	return GF_OK;
}