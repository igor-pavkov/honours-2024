GF_Err video_sample_entry_Write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MPEGVisualSampleEntryBox *ptr = (GF_MPEGVisualSampleEntryBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_isom_video_sample_entry_write((GF_VisualSampleEntryBox *)s, bs);

	/*mp4v*/
	if (ptr->esd) {
		e = gf_isom_box_write((GF_Box *)ptr->esd, bs);
		if (e) return e;
	}
	/*mp4v*/
	else if (ptr->cfg_3gpp) {
		e = gf_isom_box_write((GF_Box *)ptr->cfg_3gpp, bs);
		if (e) return e;
	}
	/*avc or hevc or av1*/
	else {
		if (ptr->avc_config && ptr->avc_config->config) {
			e = gf_isom_box_write((GF_Box *) ptr->avc_config, bs);
			if (e) return e;
		}
		if (ptr->hevc_config && ptr->hevc_config->config) {
			e = gf_isom_box_write((GF_Box *) ptr->hevc_config, bs);
			if (e) return e;
		}
		if (ptr->ipod_ext)	{
			e = gf_isom_box_write((GF_Box *) ptr->ipod_ext, bs);
			if (e) return e;
		}
		if (ptr->descr)	{
			e = gf_isom_box_write((GF_Box *) ptr->descr, bs);
			if (e) return e;
		}
		if (ptr->svc_config && ptr->svc_config->config) {
			e = gf_isom_box_write((GF_Box *) ptr->svc_config, bs);
			if (e) return e;
		}
		if (ptr->mvc_config && ptr->mvc_config->config) {
			e = gf_isom_box_write((GF_Box *) ptr->mvc_config, bs);
			if (e) return e;
		}
		if (ptr->lhvc_config && ptr->lhvc_config->config) {
			e = gf_isom_box_write((GF_Box *) ptr->lhvc_config, bs);
			if (e) return e;
		}
		if (ptr->av1_config && ptr->av1_config->config) {
			e = gf_isom_box_write((GF_Box *)ptr->av1_config, bs);
			if (e) return e;
		}
		if (ptr->vp_config && ptr->vp_config->config) {
			e = gf_isom_box_write((GF_Box *)ptr->vp_config, bs);
			if (e) return e;
		}
	}
	if (ptr->clap) {
		e = gf_isom_box_write((GF_Box*)ptr->clap, bs);
		if (e) return e;
	}
	if (ptr->pasp) {
		e = gf_isom_box_write((GF_Box *)ptr->pasp, bs);
		if (e) return e;
	}
	if (ptr->colr) {
		e = gf_isom_box_write((GF_Box*)ptr->colr, bs);
		if (e) return e;
	}
	if (ptr->mdcv) {
		e = gf_isom_box_write((GF_Box*)ptr->mdcv, bs);
		if (e) return e;
	}
	if (ptr->clli) {
		e = gf_isom_box_write((GF_Box*)ptr->clli, bs);
		if (e) return e;
	}
	if (ptr->ccst) {
		e = gf_isom_box_write((GF_Box *)ptr->ccst, bs);
		if (e) return e;
	}
	if (ptr->auxi) {
		e = gf_isom_box_write((GF_Box *)ptr->auxi, bs);
		if (e) return e;
	}
	if (ptr->rvcc) {
		e = gf_isom_box_write((GF_Box *)ptr->rvcc, bs);
		if (e) return e;
	}
	if (ptr->rinf) {
		e = gf_isom_box_write((GF_Box *)ptr->rinf, bs);
		if (e) return e;
	}
	return gf_isom_box_array_write(s, ptr->protections, bs);
}