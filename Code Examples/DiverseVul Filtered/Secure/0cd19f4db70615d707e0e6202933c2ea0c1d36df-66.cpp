static void vp9_quantization_params(GF_BitStream *bs)
{
	/*base_q_idx = */gf_bs_read_int_log(bs, 8, "base_q_idx");
	vp9_delta_q(bs); // delta_q_y_dc
	vp9_delta_q(bs); // delta_q_uv_dc
	vp9_delta_q(bs); // delta_q_uv_ac
}