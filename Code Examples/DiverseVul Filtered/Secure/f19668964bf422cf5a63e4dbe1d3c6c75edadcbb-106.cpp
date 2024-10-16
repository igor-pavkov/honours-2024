GF_Err senc_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_SampleEncryptionBox *ptr = (GF_SampleEncryptionBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	//WARNING - PSEC (UUID) IS TYPECASTED TO SENC (FULL BOX) SO WE CANNOT USE USUAL FULL BOX FUNCTIONS
	ptr->version = gf_bs_read_u8(bs);
	ptr->flags = gf_bs_read_u24(bs);

	ptr->bs_offset = gf_bs_get_position(bs);
	gf_bs_skip_bytes(bs, ptr->size);
	ptr->size = 0;
	ptr->load_needed = GF_TRUE;
	return GF_OK;
}