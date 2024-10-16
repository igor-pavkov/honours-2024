GF_Err video_sample_entry_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_MPEGVisualSampleEntryBox *mp4v = (GF_MPEGVisualSampleEntryBox*)s;
	GF_Err e;
	e = gf_isom_video_sample_entry_read((GF_VisualSampleEntryBox *)s, bs);
	if (e) return e;
	e = gf_isom_box_array_read(s, bs, video_sample_entry_on_child_box);
	if (e) return e;
	/*this is an AVC sample desc*/
	if (mp4v->avc_config || mp4v->svc_config || mp4v->mvc_config)
		AVC_RewriteESDescriptor(mp4v);
	/*this is an HEVC sample desc*/
	if (mp4v->hevc_config || mp4v->lhvc_config || (mp4v->type==GF_ISOM_BOX_TYPE_HVT1))
		HEVC_RewriteESDescriptor(mp4v);
	/*this is an AV1 sample desc*/
	if (mp4v->av1_config)
		AV1_RewriteESDescriptor(mp4v);
	/*this is a VP8-9 sample desc*/
	if (mp4v->vp_config)
		VP9_RewriteESDescriptor(mp4v);

	if (s->type==GF_ISOM_BOX_TYPE_ENCV) {
		GF_ProtectionSchemeInfoBox *sinf = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(s->child_boxes, GF_ISOM_BOX_TYPE_SINF);

		if (sinf && sinf->original_format) {
			u32 type = sinf->original_format->data_format;
			switch (type) {
			case GF_ISOM_SUBTYPE_3GP_H263:
				if (mp4v->cfg_3gpp) mp4v->cfg_3gpp->cfg.type = type;
				break;
			}
		}
	}
	return GF_OK;
}