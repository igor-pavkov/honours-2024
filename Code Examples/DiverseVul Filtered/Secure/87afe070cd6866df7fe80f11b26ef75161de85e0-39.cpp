static u64 do_size_top_boxes(char *inName, char *compress_top_boxes, u32 mode)
{
	FILE *in;
	u64 top_size = 0;
	Bool do_all = GF_FALSE;
	GF_BitStream *bs_in;
	if (!compress_top_boxes) return GF_BAD_PARAM;
	if (!strcmp(compress_top_boxes, "all") || !strcmp(compress_top_boxes, "*") || !strcmp(compress_top_boxes, "@"))
		do_all = GF_TRUE;

	in = gf_fopen(inName, "rb");
	if (!in) return GF_URL_ERROR;
	bs_in = gf_bs_from_file(in, GF_BITSTREAM_READ);
	while (gf_bs_available(bs_in)) {
		const char *stype;
		u32 hdr_size = 8;
		u64 lsize = gf_bs_read_u32(bs_in);
		u32 type = gf_bs_read_u32(bs_in);

		if (lsize==1) {
			lsize = gf_bs_read_u64(bs_in);
			hdr_size = 16;
		} else if (lsize==0) {
			lsize = gf_bs_available(bs_in) + 8;
		}
		stype = gf_4cc_to_str(type);
		if (do_all || strstr(compress_top_boxes, stype)) {
			//only count boxes
			if (mode==2) {
				top_size += 1;
			} else {
				top_size += lsize;
			}
		}
		gf_bs_skip_bytes(bs_in, lsize - hdr_size);
	}
	gf_bs_del(bs_in);
	gf_fclose(in);
	return top_size;

}