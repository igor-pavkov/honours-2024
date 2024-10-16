GF_Err saiz_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_SampleAuxiliaryInfoSizeBox*ptr = (GF_SampleAuxiliaryInfoSizeBox*)s;

	if (ptr->flags & 1) {
		ISOM_DECREASE_SIZE(ptr, 8);
		ptr->aux_info_type = gf_bs_read_u32(bs);
		ptr->aux_info_type_parameter = gf_bs_read_u32(bs);
	}
	ISOM_DECREASE_SIZE(ptr, 5);
	ptr->default_sample_info_size = gf_bs_read_u8(bs);
	ptr->sample_count = gf_bs_read_u32(bs);

	if (ptr->default_sample_info_size == 0) {
		if (ptr->size < ptr->sample_count)
		    return GF_ISOM_INVALID_FILE;

		ptr->sample_info_size = gf_malloc(sizeof(u8)*ptr->sample_count);
		ptr->sample_alloc = ptr->sample_count;
		if (!ptr->sample_info_size)
		    return GF_OUT_OF_MEM;

		ISOM_DECREASE_SIZE(ptr, ptr->sample_count);
		gf_bs_read_data(bs, (char *) ptr->sample_info_size, ptr->sample_count);
	}
	return GF_OK;
}