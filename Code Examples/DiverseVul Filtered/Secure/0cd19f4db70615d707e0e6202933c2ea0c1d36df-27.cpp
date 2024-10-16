static void vp9_read_prob(GF_BitStream *bs)
{
	Bool prob_coded = gf_bs_read_int_log(bs, 1, "prob_coded");
	if (prob_coded) {
		gf_bs_read_int_log(bs, 8, "prob");
	}
}