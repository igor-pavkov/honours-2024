GF_Err gf_hevc_change_vui(GF_HEVCConfig *hvcc, GF_VUIInfo *vui_info)
{
	GF_BitStream *orig, *mod;
	HEVCState hevc;
	u32 i, bit_offset, flag;
	s32 idx;
	GF_NALUFFParamArray *spss;
	GF_NALUFFParam *slc;
	orig = NULL;

	memset(&hevc, 0, sizeof(HEVCState));
	hevc.sps_active_idx = -1;

	i = 0;
	spss = NULL;
	while ((spss = (GF_NALUFFParamArray *)gf_list_enum(hvcc->param_array, &i))) {
		if (spss->type == GF_HEVC_NALU_SEQ_PARAM)
			break;
		spss = NULL;
	}
	if (!spss) return GF_NON_COMPLIANT_BITSTREAM;

	i = 0;
	while ((slc = (GF_NALUFFParam *)gf_list_enum(spss->nalus, &i))) {
		u8 *no_emulation_buf;
		u32 no_emulation_buf_size, emulation_bytes;

		/*SPS may still contains emulation bytes*/
		no_emulation_buf = gf_malloc((slc->size) * sizeof(char));
		no_emulation_buf_size = gf_media_nalu_remove_emulation_bytes(slc->data, no_emulation_buf, slc->size);

		idx = gf_hevc_read_sps_ex(no_emulation_buf, no_emulation_buf_size, &hevc, &bit_offset);
		if (idx < 0) {
			if (orig)
				gf_bs_del(orig);
			gf_free(no_emulation_buf);
			continue;
		}

		orig = gf_bs_new(no_emulation_buf, no_emulation_buf_size, GF_BITSTREAM_READ);
		mod = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

		/*copy over till vui flag*/
		assert(bit_offset >= 0);
		while (bit_offset) {
			flag = gf_bs_read_int(orig, 1);
			gf_bs_write_int(mod, flag, 1);
			bit_offset--;
		}

		avc_hevc_vvc_rewrite_vui(vui_info, orig, mod, GF_FALSE);

		/*finally copy over remaining*/
		while (gf_bs_bits_available(orig)) {
			flag = gf_bs_read_int(orig, 1);
			gf_bs_write_int(mod, flag, 1);
		}
		gf_bs_del(orig);
		orig = NULL;
		gf_free(no_emulation_buf);

		/*set anti-emulation*/
		gf_bs_get_content(mod, &no_emulation_buf, &no_emulation_buf_size);
		emulation_bytes = gf_media_nalu_emulation_bytes_add_count(no_emulation_buf, no_emulation_buf_size);
		if (no_emulation_buf_size + emulation_bytes > slc->size)
			slc->data = (char*)gf_realloc(slc->data, no_emulation_buf_size + emulation_bytes);

		slc->size = gf_media_nalu_add_emulation_bytes(no_emulation_buf, slc->data, no_emulation_buf_size);

		gf_bs_del(mod);
		gf_free(no_emulation_buf);
	}
	return GF_OK;
}