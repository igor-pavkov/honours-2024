GF_Err proj_type_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ProjectionTypeBox *ptr = (GF_ProjectionTypeBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	if (ptr->type==GF_ISOM_BOX_TYPE_CBMP) {
		gf_bs_write_u32(bs, ptr->layout);
		gf_bs_write_u32(bs, ptr->padding);
	}
	else if (ptr->type==GF_ISOM_BOX_TYPE_EQUI) {
		gf_bs_write_u32(bs, ptr->bounds_top);
		gf_bs_write_u32(bs, ptr->bounds_bottom);
		gf_bs_write_u32(bs, ptr->bounds_left);
		gf_bs_write_u32(bs, ptr->bounds_right);
	} else {
		gf_bs_write_u32(bs, ptr->crc);
		gf_bs_write_u32(bs, ptr->encoding_4cc);
	}
	return GF_OK;
}