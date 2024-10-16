s32 gf_mpegh_get_mhas_pl(u8 *ptr, u32 size, u64 *ch_layout)
{
	s32 PL = -1;
	GF_BitStream *bs;
	u32 i;
	s32 sync_pos=-1;

	if (!ptr || !size) return 0;
	
	for (i=0; i<size-3; i++) {
		if ((ptr[i]==0xC0) && (ptr[i+1]== 0x01) && (ptr[i+2]==0xA5)) {
			sync_pos = i;
			break;
		}
	}
	if (sync_pos<0) return 0;
	if (ch_layout) *ch_layout = 0;
	bs = gf_bs_new(ptr, size, GF_BITSTREAM_READ);
	gf_bs_skip_bytes(bs, sync_pos);

	while (gf_bs_available(bs)) {
		u32 type = (u32) gf_mpegh_escaped_value(bs, 3, 8, 8);
		/*u64 label = */gf_mpegh_escaped_value(bs, 2, 8, 32);
		u64 mh_size = gf_mpegh_escaped_value(bs, 11, 24, 24);
		if (mh_size > gf_bs_available(bs))
			break;
		//MHAS config
		if (type==1) {
			PL = gf_bs_read_int(bs, 8);
			if (ch_layout) {
				u32 idx = gf_bs_read_int(bs, 5);
				if (idx==0x1f)
					gf_bs_read_int(bs, 24);
				/*idx = */gf_bs_read_int(bs, 3);
				gf_bs_read_int(bs, 1);
				gf_bs_read_int(bs, 1);

				//speaker config
				idx = gf_bs_read_int(bs, 2);
				if (idx == 0) {
					*ch_layout = gf_audio_fmt_get_layout_from_cicp( gf_bs_read_int(bs, 6) );
				}
			}
			break;
		}
		gf_bs_skip_bytes(bs, mh_size);
	}
	gf_bs_del(bs);
	return PL;
}