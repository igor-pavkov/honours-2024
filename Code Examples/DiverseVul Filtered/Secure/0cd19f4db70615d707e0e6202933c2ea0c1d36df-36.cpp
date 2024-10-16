static void vp9_delta_q(GF_BitStream *bs) {
	Bool delta_coded = gf_bs_read_int_log(bs, 1, "delta_coded");
	if (delta_coded) {
		gf_bs_read_int_log(bs, 4, "delta_q");
	}
}