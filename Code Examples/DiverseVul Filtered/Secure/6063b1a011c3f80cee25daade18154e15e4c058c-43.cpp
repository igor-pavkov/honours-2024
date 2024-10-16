GF_Err audio_sample_entry_AddBox(GF_Box *s, GF_Box *a)
{
	GF_UnknownBox *wave = NULL;
	Bool drop_wave=GF_FALSE;
	GF_MPEGAudioSampleEntryBox *ptr = (GF_MPEGAudioSampleEntryBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_ESDS:
		if (ptr->esd) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->esd = (GF_ESDBox *)a;
		ptr->is_qtff = 0;
		break;
	case GF_ISOM_BOX_TYPE_SINF:
		gf_list_add(ptr->protections, a);
		break;
	case GF_ISOM_BOX_TYPE_DAMR:
	case GF_ISOM_BOX_TYPE_DEVC:
	case GF_ISOM_BOX_TYPE_DQCP:
	case GF_ISOM_BOX_TYPE_DSMV:
		ptr->cfg_3gpp = (GF_3GPPConfigBox *) a;
		/*for 3GP config, remember sample entry type in config*/
		ptr->cfg_3gpp->cfg.type = ptr->type;
		ptr->is_qtff = 0;
		break;

	case GF_ISOM_BOX_TYPE_DOPS:
		ptr->cfg_opus = (GF_OpusSpecificBox *)a;
		ptr->is_qtff = 0;
		break;
	case GF_ISOM_BOX_TYPE_DAC3:
		ptr->cfg_ac3 = (GF_AC3ConfigBox *) a;
		ptr->is_qtff = 0;
		break;
	case GF_ISOM_BOX_TYPE_DEC3:
		ptr->cfg_ac3 = (GF_AC3ConfigBox *) a;
		ptr->is_qtff = 0;
		break;
	case GF_ISOM_BOX_TYPE_MHA1:
	case GF_ISOM_BOX_TYPE_MHA2:
	case GF_ISOM_BOX_TYPE_MHM1:
	case GF_ISOM_BOX_TYPE_MHM2:
		ptr->cfg_mha = (GF_MHAConfigBox *) a;
		ptr->is_qtff = 0;
		break;

	case GF_ISOM_BOX_TYPE_UNKNOWN:
		wave = (GF_UnknownBox *)a;
		/*HACK for QT files: get the esds box from the track*/
		if (s->type == GF_ISOM_BOX_TYPE_MP4A) {
			if (ptr->esd) ERROR_ON_DUPLICATED_BOX(a, ptr)

			//wave subboxes may have been properly parsed
 			if ((wave->original_4cc == GF_QT_BOX_TYPE_WAVE) && gf_list_count(wave->other_boxes)) {
 				u32 i;
                for (i =0; i<gf_list_count(wave->other_boxes); i++) {
                    GF_Box *inner_box = (GF_Box *)gf_list_get(wave->other_boxes, i);
                    if (inner_box->type == GF_ISOM_BOX_TYPE_ESDS) {
                        ptr->esd = (GF_ESDBox *)inner_box;
 						if (ptr->is_qtff & 1<<16) {
                        	gf_list_rem(a->other_boxes, i);
                        	drop_wave=GF_TRUE;
						}
                    }
                }
				if (drop_wave) {
					gf_isom_box_del(a);
                	ptr->is_qtff = 0;
					return GF_OK;
				}
                ptr->is_qtff = 2;
                return gf_isom_box_add_default(s, a);
            }
            gf_isom_box_del(a);
            return GF_ISOM_INVALID_MEDIA;

		}
 		ptr->is_qtff &= ~(1<<16);

 		if ((wave->original_4cc == GF_QT_BOX_TYPE_WAVE) && gf_list_count(wave->other_boxes)) {
			ptr->is_qtff = 2;
		}
		return gf_isom_box_add_default(s, a);
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
		else if ((s->type == GF_ISOM_BOX_TYPE_MHA1) || (s->type == GF_ISOM_BOX_TYPE_MHA2)) {
			cfg_ptr = (GF_Box **) &ptr->cfg_mha;
			subtype = GF_ISOM_BOX_TYPE_MHAC;
		}

		if (cfg_ptr) {
			if (*cfg_ptr) ERROR_ON_DUPLICATED_BOX(a, ptr)

			//wave subboxes may have been properly parsed
 			if (gf_list_count(a->other_boxes)) {
 				u32 i;
                for (i =0; i<gf_list_count(a->other_boxes); i++) {
                    GF_Box *inner_box = (GF_Box *)gf_list_get(a->other_boxes, i);
                    if (inner_box->type == subtype) {
						*cfg_ptr = inner_box;
 						if (ptr->is_qtff & 1<<16) {
                        	gf_list_rem(a->other_boxes, i);
                        	drop_wave=GF_TRUE;
						}
						break;
                    }
                }
				if (drop_wave) {
					gf_isom_box_del(a);
                	ptr->is_qtff = 0;
					return GF_OK;
				}
                ptr->is_qtff = 2;
                return gf_isom_box_add_default(s, a);
            }
		}
	}
 		ptr->is_qtff = 2;
		return gf_isom_box_add_default(s, a);
	default:
		return gf_isom_box_add_default(s, a);
	}
	return GF_OK;
}