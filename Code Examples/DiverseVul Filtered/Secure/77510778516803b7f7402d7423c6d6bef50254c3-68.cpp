GF_Err proj_type_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_ProjectionTypeBox *ptr = (GF_ProjectionTypeBox *)s;

	if (ptr->type==GF_ISOM_BOX_TYPE_CBMP) {
		ISOM_DECREASE_SIZE(ptr, 8)
		ptr->layout = gf_bs_read_u32(bs);
		ptr->padding = gf_bs_read_u32(bs);
	}
	else if (ptr->type==GF_ISOM_BOX_TYPE_EQUI) {
		ISOM_DECREASE_SIZE(ptr, 16)
		ptr->bounds_top = gf_bs_read_u32(bs);
		ptr->bounds_bottom = gf_bs_read_u32(bs);
		ptr->bounds_left = gf_bs_read_u32(bs);
		ptr->bounds_right = gf_bs_read_u32(bs);
	} else {
		ISOM_DECREASE_SIZE(ptr, 8)
		ptr->crc = gf_bs_read_u32(bs);
		ptr->encoding_4cc = gf_bs_read_u32(bs);
	}
	return gf_isom_box_array_read(s, bs);
}