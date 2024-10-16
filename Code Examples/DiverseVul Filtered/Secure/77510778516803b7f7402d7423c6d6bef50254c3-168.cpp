void mhap_box_del(GF_Box *s)
{
	GF_MHACompatibleProfilesBox *ptr = (GF_MHACompatibleProfilesBox *) s;
	if (ptr->compat_profiles) gf_free(ptr->compat_profiles);
	gf_free(s);
}