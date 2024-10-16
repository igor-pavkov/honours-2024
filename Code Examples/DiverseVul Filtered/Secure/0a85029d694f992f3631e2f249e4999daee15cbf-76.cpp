GF_Err gf_isom_oinf_write_entry(void *entry, GF_BitStream *bs)
{
	GF_OperatingPointsInformation* ptr = (GF_OperatingPointsInformation *)entry;
	u32 i, j, count;
	if (!ptr) return GF_OK;

	gf_bs_write_u16(bs, ptr->scalability_mask);
	gf_bs_write_int(bs, 0xFF, 2);//reserved
	count=gf_list_count(ptr->profile_tier_levels);
	gf_bs_write_int(bs, count, 6);
	for (i = 0; i < count; i++) {
		LHEVC_ProfileTierLevel *ptl = (LHEVC_ProfileTierLevel *)gf_list_get(ptr->profile_tier_levels, i);
		gf_bs_write_int(bs, ptl->general_profile_space, 2);
		gf_bs_write_int(bs, ptl->general_tier_flag, 1);
		gf_bs_write_int(bs, ptl->general_profile_idc, 5);
		gf_bs_write_u32(bs, ptl->general_profile_compatibility_flags);
		gf_bs_write_long_int(bs, ptl->general_constraint_indicator_flags, 48);
		gf_bs_write_u8(bs, ptl->general_level_idc);
	}
	count=gf_list_count(ptr->operating_points);
	gf_bs_write_u16(bs, count);
	for (i = 0; i < count; i++) {
		LHEVC_OperatingPoint *op = (LHEVC_OperatingPoint *)gf_list_get(ptr->operating_points, i);
		gf_bs_write_u16(bs, op->output_layer_set_idx);
		gf_bs_write_u8(bs, op->max_temporal_id);
		gf_bs_write_u8(bs, op->layer_count);
		for (j = 0; j < op->layer_count; j++) {
			gf_bs_write_u8(bs, op->layers_info[j].ptl_idx);
			gf_bs_write_int(bs, op->layers_info[j].layer_id, 6);
			op->layers_info[j].is_outputlayer ? gf_bs_write_int(bs, 0x1, 1)  : gf_bs_write_int(bs, 0x0, 1);
			op->layers_info[j].is_alternate_outputlayer ? gf_bs_write_int(bs, 0x1, 1)  : gf_bs_write_int(bs, 0x0, 1);
		}
		gf_bs_write_u16(bs, op->minPicWidth);
		gf_bs_write_u16(bs, op->minPicHeight);
		gf_bs_write_u16(bs, op->maxPicWidth);
		gf_bs_write_u16(bs, op->maxPicHeight);
		gf_bs_write_int(bs, op->maxChromaFormat, 2);
		gf_bs_write_int(bs, op->maxBitDepth - 8, 3);
		gf_bs_write_int(bs, 0x1, 1);//resereved
		op->frame_rate_info_flag ? gf_bs_write_int(bs, 0x1, 1)  : gf_bs_write_int(bs, 0x0, 1);
		op->bit_rate_info_flag ? gf_bs_write_int(bs, 0x1, 1)  : gf_bs_write_int(bs, 0x0, 1);
		if (op->frame_rate_info_flag) {
			gf_bs_write_u16(bs, op->avgFrameRate);
			gf_bs_write_int(bs, 0xFF, 6); //reserved
			gf_bs_write_int(bs, op->constantFrameRate, 2);
		}
		if (op->bit_rate_info_flag) {
			gf_bs_write_u32(bs, op->maxBitRate);
			gf_bs_write_u32(bs, op->avgBitRate);
		}
	}
	count=gf_list_count(ptr->dependency_layers);
	gf_bs_write_u8(bs, count);
	for (i = 0; i < count; i++) {
		LHEVC_DependentLayer *dep = (LHEVC_DependentLayer *)gf_list_get(ptr->dependency_layers, i);
		gf_bs_write_u8(bs, dep->dependent_layerID);
		gf_bs_write_u8(bs, dep->num_layers_dependent_on);
		for (j = 0; j < dep->num_layers_dependent_on; j++)
			gf_bs_write_u8(bs, dep->dependent_on_layerID[j]);
		for (j = 0; j < 16; j++) {
			if (ptr->scalability_mask & (1 << j))
				gf_bs_write_u8(bs, dep->dimension_identifier[j]);
		}
	}

	return GF_OK;
}