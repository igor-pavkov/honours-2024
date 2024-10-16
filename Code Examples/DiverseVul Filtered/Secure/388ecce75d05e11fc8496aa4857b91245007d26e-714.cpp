GF_Err tfhd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TrackFragmentHeaderBox *ptr = (GF_TrackFragmentHeaderBox *)s;

	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->trackID = gf_bs_read_u32(bs);

	//The rest depends on the flags
	if (ptr->flags & GF_ISOM_TRAF_BASE_OFFSET) {
		ISOM_DECREASE_SIZE(ptr, 8);
		ptr->base_data_offset = gf_bs_read_u64(bs);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_DESC) {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->sample_desc_index = gf_bs_read_u32(bs);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_DUR) {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->def_sample_duration = gf_bs_read_u32(bs);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_SIZE) {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->def_sample_size = gf_bs_read_u32(bs);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_FLAGS) {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->def_sample_flags = gf_bs_read_u32(bs);
	}
	return GF_OK;
}