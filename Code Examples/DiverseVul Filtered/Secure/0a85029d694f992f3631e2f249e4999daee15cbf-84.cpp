void VP9_RewriteESDescriptorEx(GF_MPEGVisualSampleEntryBox *vp9, GF_MediaBox *mdia)
{
	GF_BitRateBox *btrt = gf_isom_sample_entry_get_bitrate((GF_SampleEntryBox *)vp9, GF_FALSE);

	if (vp9->emul_esd) gf_odf_desc_del((GF_Descriptor *)vp9->emul_esd);
	vp9->emul_esd = gf_odf_desc_esd_new(2);
	vp9->emul_esd->decoderConfig->streamType = GF_STREAM_VISUAL;
	if (vp9->type == GF_ISOM_BOX_TYPE_VP08)
		vp9->emul_esd->decoderConfig->objectTypeIndication = GF_CODECID_VP8;
	else
		vp9->emul_esd->decoderConfig->objectTypeIndication = GF_CODECID_VP9;

	if (btrt) {
		vp9->emul_esd->decoderConfig->bufferSizeDB = btrt->bufferSizeDB;
		vp9->emul_esd->decoderConfig->avgBitrate = btrt->avgBitrate;
		vp9->emul_esd->decoderConfig->maxBitrate = btrt->maxBitrate;
	}

	if (vp9->vp_config) {
		GF_VPConfig *vp9_cfg = VP_DuplicateConfig(vp9->vp_config->config);
		if (vp9_cfg) {
			gf_odf_vp_cfg_write(vp9_cfg, &vp9->emul_esd->decoderConfig->decoderSpecificInfo->data, &vp9->emul_esd->decoderConfig->decoderSpecificInfo->dataLength, GF_FALSE);
			gf_odf_vp_cfg_del(vp9_cfg);
		}
	}
}