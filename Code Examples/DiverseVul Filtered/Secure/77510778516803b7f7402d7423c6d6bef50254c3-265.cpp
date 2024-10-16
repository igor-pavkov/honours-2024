GF_Err mhap_box_read(GF_Box *s,GF_BitStream *bs)
{
	u32 i;
	GF_MHACompatibleProfilesBox *ptr = (GF_MHACompatibleProfilesBox *) s;

	ISOM_DECREASE_SIZE(s, 1)
	ptr->num_profiles = gf_bs_read_u8(bs);
	if (!ptr->num_profiles) return GF_OK;

	ISOM_DECREASE_SIZE(s, ptr->num_profiles)
	ptr->compat_profiles = gf_malloc(sizeof(u8) * ptr->num_profiles);
	if (!ptr->compat_profiles) return GF_OUT_OF_MEM;
	for (i=0; i<ptr->num_profiles; i++) {
		ptr->compat_profiles[i] = gf_bs_read_u8(bs);
	}
	return GF_OK;
}