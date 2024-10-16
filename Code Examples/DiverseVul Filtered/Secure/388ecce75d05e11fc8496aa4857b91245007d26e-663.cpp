GF_Err ccst_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_CodingConstraintsBox *ptr = (GF_CodingConstraintsBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_int(bs, ptr->all_ref_pics_intra, 1);
	gf_bs_write_int(bs, ptr->intra_pred_used, 1);
	gf_bs_write_int(bs, ptr->max_ref_per_pic, 4);
	gf_bs_write_int(bs, 0, 26);
	return GF_OK;
}