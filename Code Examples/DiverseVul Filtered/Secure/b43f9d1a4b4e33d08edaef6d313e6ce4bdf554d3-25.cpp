void naludmx_probe_recovery_sei(GF_BitStream *bs, AVCState *avc)
{
	/*parse SEI*/
	while (gf_bs_available(bs)) {
		u32 ptype, psize;

		ptype = 0;
		while (1) {
			u8 v = gf_bs_read_int(bs, 8);
			ptype += v;
			if (v != 0xFF) break;
		}

		psize = 0;
		while (1) {
			u8 v = gf_bs_read_int(bs, 8);
			psize += v;
			if (v != 0xFF) break;
		}

		if (ptype==6) {
			avc->sei.recovery_point.frame_cnt = gf_bs_read_ue(bs);
			avc->sei.recovery_point.valid = 1;
			return;
		}

		gf_bs_skip_bytes(bs, psize);

		ptype = gf_bs_peek_bits(bs, 8, 0);
		if (ptype == 0x80) {
			break;
		}
	}
}