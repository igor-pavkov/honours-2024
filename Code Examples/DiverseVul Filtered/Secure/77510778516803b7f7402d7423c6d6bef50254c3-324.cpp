GF_Err leva_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_LevelAssignmentBox *ptr = (GF_LevelAssignmentBox*)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u8(bs, ptr->level_count);
	for (i = 0; i<ptr->level_count; i++) {
		gf_bs_write_u32(bs, ptr->levels[i].track_id);
		gf_bs_write_u8(bs, ptr->levels[i].padding_flag << 7 | (ptr->levels[i].type & 0x7F));
		if (ptr->levels[i].type == 0) {
			gf_bs_write_u32(bs, ptr->levels[i].grouping_type);
		}
		else if (ptr->levels[i].type == 1) {
			gf_bs_write_u32(bs, ptr->levels[i].grouping_type);
			gf_bs_write_u32(bs, ptr->levels[i].grouping_type_parameter);
		}
		else if (ptr->levels[i].type == 4) {
			gf_bs_write_u32(bs, ptr->levels[i].sub_track_id);
		}
	}
	return GF_OK;
}