GF_Err svhd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_SphericalVideoInfoBox *ptr = (GF_SphericalVideoInfoBox *)s;
	if ((u32)ptr->size >= (u32)0xFFFFFFFF) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid size %llu in svhd box\n", ptr->size));
		return GF_ISOM_INVALID_FILE;
	}
	ptr->string = gf_malloc(sizeof(char) * ((u32) ptr->size+1));
	if (!ptr->string) return GF_OUT_OF_MEM;
	gf_bs_read_data(bs, ptr->string, (u32) ptr->size);
	ptr->string[ptr->size] = 0;
	return GF_OK;
}