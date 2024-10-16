GF_Err stsd_box_read(GF_Box *s, GF_BitStream *bs)
{
	ISOM_DECREASE_SIZE(s, 4)
	gf_bs_read_u32(bs);

	return gf_isom_box_array_read_ex(s, bs, stsd_on_child_box, GF_ISOM_BOX_TYPE_STSD);
}