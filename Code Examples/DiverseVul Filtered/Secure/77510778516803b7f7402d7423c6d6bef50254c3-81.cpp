GF_Err mhap_box_size(GF_Box *s)
{
	GF_MHACompatibleProfilesBox *ptr = (GF_MHACompatibleProfilesBox *) s;
	s->size += 1 + ptr->num_profiles;
	return GF_OK;
}