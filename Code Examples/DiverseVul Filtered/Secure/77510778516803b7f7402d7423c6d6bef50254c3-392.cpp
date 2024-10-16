GF_Err leva_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_LevelAssignmentBox *ptr = (GF_LevelAssignmentBox*)s;

	ISOM_DECREASE_SIZE(ptr, 1)
	ptr->level_count = gf_bs_read_u8(bs);
	//each level is at least 5 bytes
	if (ptr->size / 5 < ptr->level_count)
		return GF_ISOM_INVALID_FILE;

	GF_SAFE_ALLOC_N(ptr->levels, ptr->level_count, GF_LevelAssignment);
	if (!ptr->levels) return GF_OUT_OF_MEM;

	for (i = 0; i < ptr->level_count; i++) {
		GF_LevelAssignment *level = &ptr->levels[i];
		u8 tmp;
		if (!level || ptr->size < 5) return GF_BAD_PARAM;
		ISOM_DECREASE_SIZE(ptr, 5)

		level->track_id = gf_bs_read_u32(bs);
		tmp = gf_bs_read_u8(bs);
		level->padding_flag = tmp >> 7;
		level->type = tmp & 0x7F;
		if (level->type == 0) {
			ISOM_DECREASE_SIZE(ptr, 4)
			level->grouping_type = gf_bs_read_u32(bs);
		}
		else if (level->type == 1) {
			ISOM_DECREASE_SIZE(ptr, 8)
			level->grouping_type = gf_bs_read_u32(bs);
			level->grouping_type_parameter = gf_bs_read_u32(bs);
		}
		else if (level->type == 4) {
			ISOM_DECREASE_SIZE(ptr, 4)
			level->sub_track_id = gf_bs_read_u32(bs);
		}
	}
	return GF_OK;
}