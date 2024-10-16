GF_Err stri_box_read(GF_Box *s, GF_BitStream *bs)
{
	size_t i;
	GF_SubTrackInformationBox *ptr = (GF_SubTrackInformationBox *)s;
	ISOM_DECREASE_SIZE(ptr, 8)
	ptr->switch_group = gf_bs_read_u16(bs);
	ptr->alternate_group = gf_bs_read_u16(bs);
	ptr->sub_track_id = gf_bs_read_u32(bs);
	ptr->attribute_count = ptr->size / 4;
	if ((u64)ptr->attribute_count > (u64)SIZE_MAX/sizeof(u32)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid size %llu in stri\n", ptr->size));
		return GF_ISOM_INVALID_FILE;
	}
	GF_SAFE_ALLOC_N(ptr->attribute_list, (size_t)ptr->attribute_count, u32);
	if (!ptr->attribute_list) return GF_OUT_OF_MEM;
	for (i = 0; i < ptr->attribute_count; i++) {
		ISOM_DECREASE_SIZE(ptr, 4)
		ptr->attribute_list[i] = gf_bs_read_u32(bs);
	}
	return GF_OK;
}