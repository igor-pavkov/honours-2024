GF_OperatingPointsInformation *gf_isom_oinf_new_entry()
{
	GF_OperatingPointsInformation* ptr;
	GF_SAFEALLOC(ptr, GF_OperatingPointsInformation);
	if (ptr) {
		ptr->profile_tier_levels = gf_list_new();
		ptr->operating_points = gf_list_new();
		ptr->dependency_layers = gf_list_new();
	}
	return ptr;

}