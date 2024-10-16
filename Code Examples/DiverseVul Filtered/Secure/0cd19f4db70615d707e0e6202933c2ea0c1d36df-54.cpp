GF_Err gf_avc_change_vui(GF_AVCConfig *avcc, GF_VUIInfo *vui_info)
{
	GF_BitStream *orig, *mod;
	AVCState avc;
	u32 i, bit_offset, flag;
	s32 idx;
	GF_AVCConfigSlot *slc;
	orig = NULL;

	memset(&avc, 0, sizeof(AVCState));
	avc.sps_active_idx = -1;

	i=0;
	while ((slc = (GF_AVCConfigSlot *)gf_list_enum(avcc->sequenceParameterSets, &i))) {
		u8 *no_emulation_buf = NULL;
		u32 no_emulation_buf_size = 0, emulation_bytes = 0;
		idx = gf_avc_read_sps(slc->data, slc->size, &avc, 0, &bit_offset);
		if (idx<0) {
			if ( orig )
				gf_bs_del(orig);
			continue;
		}

		/*SPS still contains emulation bytes*/
		no_emulation_buf = gf_malloc((slc->size - 1) * sizeof(char));
		no_emulation_buf_size = gf_media_nalu_remove_emulation_bytes(slc->data + 1, no_emulation_buf, slc->size - 1);

		orig = gf_bs_new(no_emulation_buf, no_emulation_buf_size, GF_BITSTREAM_READ);
		gf_bs_read_data(orig, no_emulation_buf, no_emulation_buf_size);
		gf_bs_seek(orig, 0);
		mod = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

		/*copy over till vui flag*/
		assert(bit_offset >= 8);
		while (bit_offset - 8/*bit_offset doesn't take care of the first byte (NALU type)*/) {
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
		gf_bs_get_content(mod, &no_emulation_buf, &flag);
		emulation_bytes = gf_media_nalu_emulation_bytes_add_count(no_emulation_buf, flag);
		if (flag+emulation_bytes+1>slc->size)
			slc->data = (char*)gf_realloc(slc->data, flag+emulation_bytes+1);
		slc->size = gf_media_nalu_add_emulation_bytes(no_emulation_buf, slc->data + 1, flag) + 1;

		gf_bs_del(mod);
		gf_free(no_emulation_buf);
	}
	return GF_OK;
}