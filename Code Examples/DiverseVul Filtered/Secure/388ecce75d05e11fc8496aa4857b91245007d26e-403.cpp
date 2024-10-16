GF_Err tfhd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TrackFragmentHeaderBox *ptr = (GF_TrackFragmentHeaderBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->trackID);

	//The rest depends on the flags
	if (ptr->flags & GF_ISOM_TRAF_BASE_OFFSET) {
		gf_bs_write_u64(bs, ptr->base_data_offset);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_DESC) {
		gf_bs_write_u32(bs, ptr->sample_desc_index);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_DUR) {
		gf_bs_write_u32(bs, ptr->def_sample_duration);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_SIZE) {
		gf_bs_write_u32(bs, ptr->def_sample_size);
	}
	if (ptr->flags & GF_ISOM_TRAF_SAMPLE_FLAGS) {
		gf_bs_write_u32(bs, ptr->def_sample_flags);
	}
	return GF_OK;
}