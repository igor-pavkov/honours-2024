GF_Err video_sample_entry_Size(GF_Box *s)
{
	GF_Err e;
	GF_MPEGVisualSampleEntryBox *ptr = (GF_MPEGVisualSampleEntryBox *)s;

	gf_isom_video_sample_entry_size((GF_VisualSampleEntryBox *)s);

	if (ptr->esd) {
		e = gf_isom_box_size((GF_Box *)ptr->esd);
		if (e) return e;
		ptr->size += ptr->esd->size;
	} else if (ptr->cfg_3gpp) {
		e = gf_isom_box_size((GF_Box *)ptr->cfg_3gpp);
		if (e) return e;
		ptr->size += ptr->cfg_3gpp->size;
	} else {
		switch (ptr->type) {
		case GF_ISOM_BOX_TYPE_AVC1:
		case GF_ISOM_BOX_TYPE_AVC2:
		case GF_ISOM_BOX_TYPE_AVC3:
		case GF_ISOM_BOX_TYPE_AVC4:
		case GF_ISOM_BOX_TYPE_SVC1:
		case GF_ISOM_BOX_TYPE_SVC2:
		case GF_ISOM_BOX_TYPE_MVC1:
		case GF_ISOM_BOX_TYPE_MVC2:
			if (!ptr->avc_config && !ptr->svc_config  && !ptr->mvc_config)
				return GF_ISOM_INVALID_FILE;
			break;
		case GF_ISOM_BOX_TYPE_VP08:
		case GF_ISOM_BOX_TYPE_VP09:
			if (!ptr->vp_config) {
				return GF_ISOM_INVALID_FILE;
			}
			break;
		case GF_ISOM_BOX_TYPE_AV01:
			if (!ptr->av1_config) {
				return GF_ISOM_INVALID_FILE;
			}
			break;
		case GF_ISOM_BOX_TYPE_HVC1:
		case GF_ISOM_BOX_TYPE_HEV1:
		case GF_ISOM_BOX_TYPE_HVC2:
		case GF_ISOM_BOX_TYPE_HEV2:
		case GF_ISOM_BOX_TYPE_LHV1:
		case GF_ISOM_BOX_TYPE_LHE1:
		//commented on purpose, HVT1 tracks have no config associated
//		case GF_ISOM_BOX_TYPE_HVT1:
//		case GF_ISOM_BOX_TYPE_HVT2:
			if (!ptr->hevc_config && !ptr->lhvc_config) {
				return GF_ISOM_INVALID_FILE;
			}
			break;
		default:
			break;
		}

		if (ptr->hevc_config && ptr->hevc_config->config) {
			e = gf_isom_box_size((GF_Box *)ptr->hevc_config);
			if (e) return e;
			ptr->size += ptr->hevc_config->size;
		}

		if (ptr->avc_config && ptr->avc_config->config) {
			e = gf_isom_box_size((GF_Box *) ptr->avc_config);
			if (e) return e;
			ptr->size += ptr->avc_config->size;
		}

		if (ptr->svc_config && ptr->svc_config->config) {
			e = gf_isom_box_size((GF_Box *) ptr->svc_config);
			if (e) return e;
			ptr->size += ptr->svc_config->size;
		}

		if (ptr->mvc_config && ptr->mvc_config->config) {
			e = gf_isom_box_size((GF_Box *) ptr->mvc_config);
			if (e) return e;
			ptr->size += ptr->mvc_config->size;
		}

		if (ptr->lhvc_config && ptr->lhvc_config->config) {
			e = gf_isom_box_size((GF_Box *) ptr->lhvc_config);
			if (e) return e;
			ptr->size += ptr->lhvc_config->size;
		}

		if (ptr->av1_config && ptr->av1_config->config) {
			e = gf_isom_box_size((GF_Box *)ptr->av1_config);
			if (e) return e;
			ptr->size += ptr->av1_config->size;
		}

		if (ptr->vp_config && ptr->vp_config->config) {
			e = gf_isom_box_size((GF_Box *)ptr->vp_config);
			if (e) return e;
			ptr->size += ptr->vp_config->size;
		}

		if (ptr->ipod_ext) {
			e = gf_isom_box_size((GF_Box *) ptr->ipod_ext);
			if (e) return e;
			ptr->size += ptr->ipod_ext->size;
		}
		if (ptr->descr) {
			e = gf_isom_box_size((GF_Box *) ptr->descr);
			if (e) return e;
			ptr->size += ptr->descr->size;
		}
	}
	if (ptr->pasp) {
		e = gf_isom_box_size((GF_Box *)ptr->pasp);
		if (e) return e;
		ptr->size += ptr->pasp->size;
	}
	if (ptr->colr) {
		e = gf_isom_box_size((GF_Box*)ptr->colr);
		if (e) return e;
		ptr->size += ptr->colr->size;
	}
	if (ptr->mdcv) {
		e = gf_isom_box_size((GF_Box*)ptr->mdcv);
		if (e) return e;
		ptr->size += ptr->mdcv->size;
	}
	if (ptr->clli) {
		e = gf_isom_box_size((GF_Box*)ptr->clli);
		if (e) return e;
		ptr->size += ptr->clli->size;
	}
	if (ptr->clap) {
		e = gf_isom_box_size((GF_Box *)ptr->clap);
		if (e) return e;
		ptr->size += ptr->clap->size;
	}
	if (ptr->ccst) {
		e = gf_isom_box_size((GF_Box *)ptr->ccst);
		if (e) return e;
		ptr->size += ptr->ccst->size;
	}
	if (ptr->auxi) {
		e = gf_isom_box_size((GF_Box *)ptr->auxi);
		if (e) return e;
		ptr->size += ptr->auxi->size;
	}
	if (ptr->rvcc) {
		e = gf_isom_box_size((GF_Box *)ptr->rvcc);
		if (e) return e;
		ptr->size += ptr->rvcc->size;
	}
	if (ptr->rinf) {
		e = gf_isom_box_size((GF_Box *)ptr->rinf);
		if (e) return e;
		ptr->size += ptr->rinf->size;
	}
	return gf_isom_box_array_size(s, ptr->protections);
}