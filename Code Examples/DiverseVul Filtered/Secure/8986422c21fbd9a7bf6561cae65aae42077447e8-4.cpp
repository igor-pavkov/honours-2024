GF_Err pssh_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_ProtectionSystemHeaderBox *ptr = (GF_ProtectionSystemHeaderBox *)s;

	gf_bs_read_data(bs, (char *) ptr->SystemID, 16);
	ISOM_DECREASE_SIZE(ptr, 16);
	if (ptr->version > 0) {
		ptr->KID_count = gf_bs_read_u32(bs);
		ISOM_DECREASE_SIZE(ptr, 4);
		if (ptr->KID_count) {
			u32 i;
			if (ptr->size < ptr->KID_count * sizeof(bin128))
				return GF_ISOM_INVALID_FILE;
			ptr->KIDs = gf_malloc(ptr->KID_count*sizeof(bin128));
			if (!ptr->KIDs)
				return GF_OUT_OF_MEM;
			for (i=0; i<ptr->KID_count; i++) {
				gf_bs_read_data(bs, (char *) ptr->KIDs[i], 16);
				ISOM_DECREASE_SIZE(ptr, 16);
			}
		}
	}
	ptr->private_data_size = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);
	if (ptr->private_data_size) {
		if (ptr->size < ptr->private_data_size)
			return GF_ISOM_INVALID_FILE;
		ptr->private_data = gf_malloc(sizeof(char)*ptr->private_data_size);
		if (!ptr->private_data)
			return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, (char *) ptr->private_data, ptr->private_data_size);
		ISOM_DECREASE_SIZE(ptr, ptr->private_data_size);
	}
	return GF_OK;
}