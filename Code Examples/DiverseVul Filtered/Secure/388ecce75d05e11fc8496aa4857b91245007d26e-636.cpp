GF_Err audio_sample_entry_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_UnknownBox *wave = NULL;
	Bool drop_wave=GF_FALSE;
	GF_MPEGAudioSampleEntryBox *ptr = (GF_MPEGAudioSampleEntryBox *)s;

	switch (a->type) {
	case GF_ISOM_BOX_TYPE_ESDS:
		if (ptr->esd) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->esd = (GF_ESDBox *)a;
		ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_NONE;
		break;

	case GF_ISOM_BOX_TYPE_DAMR:
	case GF_ISOM_BOX_TYPE_DEVC:
	case GF_ISOM_BOX_TYPE_DQCP:
	case GF_ISOM_BOX_TYPE_DSMV:
		if (ptr->cfg_3gpp) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->cfg_3gpp = (GF_3GPPConfigBox *) a;
		/*for 3GP config, remember sample entry type in config*/
		ptr->cfg_3gpp->cfg.type = ptr->type;
		ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_NONE;
		break;

	case GF_ISOM_BOX_TYPE_DOPS:
		if (ptr->cfg_opus) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->cfg_opus = (GF_OpusSpecificBox *)a;
		ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_NONE;
		break;
	case GF_ISOM_BOX_TYPE_DAC3:
		if (ptr->cfg_ac3) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->cfg_ac3 = (GF_AC3ConfigBox *) a;
		ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_NONE;
		break;
	case GF_ISOM_BOX_TYPE_DEC3:
		if (ptr->cfg_ac3) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->cfg_ac3 = (GF_AC3ConfigBox *) a;
		break;
	case GF_ISOM_BOX_TYPE_MHAC:
		if (ptr->cfg_mha) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->cfg_mha = (GF_MHAConfigBox *) a;
		ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_NONE;
		break;
	case GF_ISOM_BOX_TYPE_DFLA:
		if (ptr->cfg_flac) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->cfg_flac = (GF_FLACConfigBox *) a;
		ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_NONE;
		break;

	case GF_ISOM_BOX_TYPE_UNKNOWN:
		wave = (GF_UnknownBox *)a;
		/*HACK for QT files: get the esds box from the track*/
		if (s->type == GF_ISOM_BOX_TYPE_MP4A) {
			if (ptr->esd) ERROR_ON_DUPLICATED_BOX(a, ptr)

			//wave subboxes may have been properly parsed
 			if ((wave->original_4cc == GF_QT_BOX_TYPE_WAVE) && gf_list_count(wave->child_boxes)) {
 				u32 i;
                for (i =0; i<gf_list_count(wave->child_boxes); i++) {
                    GF_Box *inner_box = (GF_Box *)gf_list_get(wave->child_boxes, i);
                    if (inner_box->type == GF_ISOM_BOX_TYPE_ESDS) {
                        ptr->esd = (GF_ESDBox *)inner_box;
 						if (ptr->qtff_mode & GF_ISOM_AUDIO_QTFF_CONVERT_FLAG) {
                        	gf_list_rem(a->child_boxes, i);
                        	drop_wave=GF_TRUE;
                        	ptr->compression_id = 0;
                        	gf_list_add(ptr->child_boxes, inner_box);
						}
                    }
                }
				if (drop_wave) {
					gf_isom_box_del_parent(&ptr->child_boxes, a);
                	ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_NONE;
					ptr->version = 0;
					return GF_OK;
				}
                ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_ON_EXT_VALID;
                return GF_OK;
            }
            gf_isom_box_del_parent(&ptr->child_boxes, a);
            return GF_ISOM_INVALID_MEDIA;

		}
 		ptr->qtff_mode &= ~GF_ISOM_AUDIO_QTFF_CONVERT_FLAG;

 		if ((wave->original_4cc == GF_QT_BOX_TYPE_WAVE) && gf_list_count(wave->child_boxes)) {
			ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_ON_NOEXT;
		}
		return GF_OK;
	case GF_QT_BOX_TYPE_WAVE:
	{
		u32 subtype = 0;
		GF_Box **cfg_ptr = NULL;
		if (s->type == GF_ISOM_BOX_TYPE_MP4A) {
			cfg_ptr = (GF_Box **) &ptr->esd;
			subtype = GF_ISOM_BOX_TYPE_ESDS;
		}
		else if (s->type == GF_ISOM_BOX_TYPE_AC3) {
			cfg_ptr = (GF_Box **) &ptr->cfg_ac3;
			subtype = GF_ISOM_BOX_TYPE_DAC3;
		}
		else if (s->type == GF_ISOM_BOX_TYPE_EC3) {
			cfg_ptr = (GF_Box **) &ptr->cfg_ac3;
			subtype = GF_ISOM_BOX_TYPE_DEC3;
		}
		else if (s->type == GF_ISOM_BOX_TYPE_OPUS) {
			cfg_ptr = (GF_Box **) &ptr->cfg_opus;
			subtype = GF_ISOM_BOX_TYPE_DOPS;
		}
		else if ((s->type == GF_ISOM_BOX_TYPE_MHA1)
			|| (s->type == GF_ISOM_BOX_TYPE_MHA2)
			|| (s->type == GF_ISOM_BOX_TYPE_MHM1)
			|| (s->type == GF_ISOM_BOX_TYPE_MHM2)
		) {
			cfg_ptr = (GF_Box **) &ptr->cfg_mha;
			subtype = GF_ISOM_BOX_TYPE_MHAC;
		}

		if (cfg_ptr) {
			if (*cfg_ptr) ERROR_ON_DUPLICATED_BOX(a, ptr)

			//wave subboxes may have been properly parsed
 			if (gf_list_count(a->child_boxes)) {
 				u32 i;
                for (i =0; i<gf_list_count(a->child_boxes); i++) {
                    GF_Box *inner_box = (GF_Box *)gf_list_get(a->child_boxes, i);
                    if (inner_box->type == subtype) {
                        *cfg_ptr = inner_box;
 						if (ptr->qtff_mode & GF_ISOM_AUDIO_QTFF_CONVERT_FLAG) {
                        	gf_list_rem(a->child_boxes, i);
                        	drop_wave=GF_TRUE;
                        	gf_list_add(ptr->child_boxes, inner_box);
						}
						break;
                    }
                }
				if (drop_wave) {
					gf_isom_box_del_parent(&ptr->child_boxes, a);
                	ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_NONE;
					ptr->compression_id = 0;
					ptr->version = 0;
					return GF_OK;
				}
                ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_ON_EXT_VALID;
                return GF_OK;
            }
		}
	}
 		ptr->qtff_mode = GF_ISOM_AUDIO_QTFF_ON_EXT_VALID;
		return GF_OK;
	}
	return GF_OK;
}