GF_Err odrb_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_OMADRMRightsObjectBox *ptr = (GF_OMADRMRightsObjectBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_data(bs, ptr->oma_ro, ptr->oma_ro_size);
	return GF_OK;
}