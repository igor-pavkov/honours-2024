u32 gf_isom_oinf_size_entry(void *entry)
{
	GF_OperatingPointsInformation* ptr = (GF_OperatingPointsInformation *)entry;
	u32 size = 0, i ,j, count;
	if (!ptr) return 0;

	size += 3; //scalability_mask + reserved + num_profile_tier_level
	count=gf_list_count(ptr->profile_tier_levels);
	size += count * 12; //general_profile_space + general_tier_flag + general_profile_idc + general_profile_compatibility_flags + general_constraint_indicator_flags + general_level_idc
	size += 2;//num_operating_points
	count=gf_list_count(ptr->operating_points);
	for (i = 0; i < count; i++) {
		LHEVC_OperatingPoint *op = (LHEVC_OperatingPoint *)gf_list_get(ptr->operating_points, i);
		size += 2/*output_layer_set_idx*/ + 1/*max_temporal_id*/ + 1/*layer_count*/;
		size += op->layer_count * 2;
		size += 9;
		if (op->frame_rate_info_flag) {
			size += 3;
		}
		if (op->bit_rate_info_flag) {
			size += 8;
		}
	}
	size += 1;//max_layer_count
	count=gf_list_count(ptr->dependency_layers);
	for (i = 0; i < count; i++) {
		LHEVC_DependentLayer *dep = (LHEVC_DependentLayer *)gf_list_get(ptr->dependency_layers, i);
		size += 1/*dependent_layerID*/ + 1/*num_layers_dependent_on*/;
		size += dep->num_layers_dependent_on * 1;//dependent_on_layerID
		for (j = 0; j < 16; j++) {
			if (ptr->scalability_mask & (1 << j))
				size += 1;//dimension_identifier
		}
	}
	return size;
}