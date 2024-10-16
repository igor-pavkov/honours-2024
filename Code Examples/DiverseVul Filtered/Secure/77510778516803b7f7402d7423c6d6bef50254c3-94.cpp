GF_Err st3d_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_Stereo3DBox *ptr = (GF_Stereo3DBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, ptr->stereo_type);
	return GF_OK;
}