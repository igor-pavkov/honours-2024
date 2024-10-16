GF_Err pasp_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_PixelAspectRatioBox *ptr = (GF_PixelAspectRatioBox*)s;
	ISOM_DECREASE_SIZE(ptr, 8);
	ptr->hSpacing = gf_bs_read_u32(bs);
	ptr->vSpacing = gf_bs_read_u32(bs);
	return GF_OK;
}