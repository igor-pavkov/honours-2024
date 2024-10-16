GF_Err piff_tenc_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_PIFFTrackEncryptionBox *ptr = (GF_PIFFTrackEncryptionBox*)s;

	ISOM_DECREASE_SIZE(ptr, 4);
	//PIFF TENC extends UUID and fullbox
	ptr->version = gf_bs_read_u8(bs);
	ptr->flags = gf_bs_read_u24(bs);

	ISOM_DECREASE_SIZE(ptr, 20);
	ptr->AlgorithmID = gf_bs_read_int(bs, 24);
	ptr->key_info[0] = 0;
	ptr->key_info[1] = 0;
	ptr->key_info[2] = 0;
	ptr->key_info[3] = gf_bs_read_u8(bs);
	gf_bs_read_data(bs, ptr->key_info+4, 16);
	return GF_OK;
}