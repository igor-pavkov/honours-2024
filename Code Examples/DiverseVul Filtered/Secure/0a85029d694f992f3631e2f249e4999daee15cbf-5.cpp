void gf_isom_oinf_del_entry(void *entry)
{
	GF_OperatingPointsInformation* ptr = (GF_OperatingPointsInformation *)entry;
	if (!ptr) return;
	if (ptr->profile_tier_levels) {
		while (gf_list_count(ptr->profile_tier_levels)) {
			LHEVC_ProfileTierLevel *ptl = (LHEVC_ProfileTierLevel *)gf_list_get(ptr->profile_tier_levels, 0);
			gf_free(ptl);
			gf_list_rem(ptr->profile_tier_levels, 0);
		}
		gf_list_del(ptr->profile_tier_levels);
	}
	if (ptr->operating_points) {
		while (gf_list_count(ptr->operating_points)) {
			LHEVC_OperatingPoint *op = (LHEVC_OperatingPoint *)gf_list_get(ptr->operating_points, 0);
			gf_free(op);
			gf_list_rem(ptr->operating_points, 0);
		}
		gf_list_del(ptr->operating_points);
	}
	if (ptr->dependency_layers) {
		while (gf_list_count(ptr->dependency_layers)) {
			LHEVC_DependentLayer *dep = (LHEVC_DependentLayer *)gf_list_get(ptr->dependency_layers, 0);
			gf_free(dep);
			gf_list_rem(ptr->dependency_layers, 0);
		}
		gf_list_del(ptr->dependency_layers);
	}
	gf_free(ptr);
	return;
}