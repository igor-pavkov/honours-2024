GF_Err srpp_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_SRTPProcessBox *ptr = (GF_SRTPProcessBox *)s;

	ISOM_DECREASE_SIZE(s, 16)
	ptr->encryption_algorithm_rtp = gf_bs_read_u32(bs);
	ptr->encryption_algorithm_rtcp = gf_bs_read_u32(bs);
	ptr->integrity_algorithm_rtp = gf_bs_read_u32(bs);
	ptr->integrity_algorithm_rtcp = gf_bs_read_u32(bs);
	return gf_isom_box_array_read(s, bs, srpp_on_child_box);
}