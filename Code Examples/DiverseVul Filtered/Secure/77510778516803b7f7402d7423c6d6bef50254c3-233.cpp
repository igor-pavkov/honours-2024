GF_Err url_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_DataEntryURLBox *ptr = (GF_DataEntryURLBox *)s;

	if (ptr->size) {
		u32 location_size = (u32) ptr->size;
		if (location_size < 1) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid size %llu in svhd box\n", ptr->size));
			return GF_ISOM_INVALID_FILE;
		}
		ptr->location = (char*)gf_malloc(location_size);
		if (! ptr->location) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->location, location_size);
		if (ptr->location[location_size-1]) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] url box location is not 0-terminated\n" ));
			return GF_ISOM_INVALID_FILE;
		}
	}
	return GF_OK;
}