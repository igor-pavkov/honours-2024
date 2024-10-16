GF_Err mhap_box_write(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_Err e;
	GF_MHACompatibleProfilesBox *ptr = (GF_MHACompatibleProfilesBox *) s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, ptr->num_profiles);
	for (i=0; i<ptr->num_profiles; i++) {
		gf_bs_write_u8(bs, ptr->compat_profiles[i]);
	}
	return GF_OK;
}