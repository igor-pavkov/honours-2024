GF_Err gf_vvc_get_sps_info(u8 *sps_data, u32 sps_size, u32 *sps_id, u32 *width, u32 *height, s32 *par_n, s32 *par_d)
{
	VVCState *vvc;
	s32 idx;
	GF_SAFEALLOC(vvc, VVCState);
	if (!vvc) return GF_OUT_OF_MEM;
	vvc->sps_active_idx = -1;

	GF_BitStream *bs = gf_bs_new(sps_data, sps_size, GF_BITSTREAM_READ);
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);

	u8 nal_unit_type, temporal_id, layer_id;

	vvc_parse_nal_header(bs, &nal_unit_type, &temporal_id, &layer_id);
	idx = gf_vvc_read_sps_bs_internal(bs, vvc, layer_id, NULL);
	gf_bs_del(bs);
	if (idx < 0) {
		gf_free(vvc);
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	if (sps_id) *sps_id = idx;

	if (width) *width = vvc->sps[idx].width;
	if (height) *height = vvc->sps[idx].height;
	if (par_n) *par_n = vvc->sps[idx].aspect_ratio_info_present_flag ? vvc->sps[idx].sar_width : (u32)-1;
	if (par_d) *par_d = vvc->sps[idx].aspect_ratio_info_present_flag ? vvc->sps[idx].sar_height : (u32)-1;
	gf_free(vvc);
	return GF_OK;
}