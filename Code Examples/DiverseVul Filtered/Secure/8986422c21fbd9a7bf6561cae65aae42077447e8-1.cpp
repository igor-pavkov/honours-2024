GF_Err piff_pssh_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_PIFFProtectionSystemHeaderBox *ptr = (GF_PIFFProtectionSystemHeaderBox*)s;

	ISOM_DECREASE_SIZE(ptr, 24);
	//PIFF PSSH extends UUID and fullbox
	ptr->version = gf_bs_read_u8(bs);
	ptr->flags = gf_bs_read_u24(bs);
	gf_bs_read_data(bs, (char *) ptr->SystemID, 16);
	ptr->private_data_size = gf_bs_read_u32(bs);

	if (ptr->size < sizeof(char)*ptr->private_data_size)
	    return GF_ISOM_INVALID_FILE;
	ptr->private_data = gf_malloc(sizeof(char)*ptr->private_data_size);
	if (!ptr->private_data)
	    return GF_OUT_OF_MEM;

	ISOM_DECREASE_SIZE(ptr, ptr->private_data_size);
	gf_bs_read_data(bs, (char *) ptr->private_data, ptr->private_data_size);
	return GF_OK;
}