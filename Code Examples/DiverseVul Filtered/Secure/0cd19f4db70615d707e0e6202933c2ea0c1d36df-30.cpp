static void gf_hevc_vvc_parse_sei(char *buffer, u32 nal_size, HEVCState *hevc, VVCState *vvc)
{
	u32 ptype, psize, hdr;
	u64 start;
	GF_BitStream *bs;

	hdr = buffer[0];
	if (((hdr & 0x7e) >> 1) != GF_HEVC_NALU_SEI_PREFIX) return;

	bs = gf_bs_new(buffer, nal_size, GF_BITSTREAM_READ);
	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);

	gf_bs_read_int(bs, 16);

	/*parse SEI*/
	while (gf_bs_available(bs)) {
		u32 consumed;
		ptype = 0;
		while (gf_bs_peek_bits(bs, 8, 0)==0xFF) {
			gf_bs_read_int(bs, 8);
			ptype += 255;
		}
		ptype += gf_bs_read_int(bs, 8);
		psize = 0;
		while (gf_bs_peek_bits(bs, 8, 0)==0xFF) {
			gf_bs_read_int(bs, 8);
			psize += 255;
		}
		psize += gf_bs_read_int(bs, 8);

		start = gf_bs_get_position(bs);
		if (start+psize >= nal_size) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[%s] SEI user message type %d size error (%d but %d remain), skipping SEI message\n", hevc ? "HEVC" : "VVC", ptype, psize, nal_size-start));
			break;
		}

		switch (ptype) {
		case 4: /*user registered ITU-T T35*/
			if (hevc) {
				avc_parse_itu_t_t35_sei(bs, &hevc->sei.dovi);
			}
			break;
		case 144:
			//clli
			if (hevc) {
				gf_bs_read_data(bs, hevc->clli_data, 4);
				hevc->clli_valid = 1;
			} else {
				gf_bs_read_data(bs, vvc->clli_data, 4);
				vvc->clli_valid = 1;
			}
			break;
		case 137:
			//mdcv
			if (hevc) {
				gf_bs_read_data(bs, hevc->mdcv_data, 24);
				hevc->mdcv_valid = 1;
			} else {
				gf_bs_read_data(bs, vvc->mdcv_data, 24);
				vvc->mdcv_valid = 1;
			}
			break;
		default:
			break;
		}

		gf_bs_align(bs);
		consumed = (u32) (gf_bs_get_position(bs) - start);
		psize-=consumed;
		gf_bs_skip_bytes(bs, psize);
		if (gf_bs_available(bs) <= 2)
			break;
	}
	gf_bs_del(bs);
}