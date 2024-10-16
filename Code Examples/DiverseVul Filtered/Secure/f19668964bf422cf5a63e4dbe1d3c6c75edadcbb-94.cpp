GF_Err tenc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TrackEncryptionBox *ptr = (GF_TrackEncryptionBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u8(bs, 0); //reserved

	if (!ptr->version) {
		gf_bs_write_u8(bs, 0); //reserved
	} else {
		gf_bs_write_int(bs, ptr->crypt_byte_block, 4);
		gf_bs_write_int(bs, ptr->skip_byte_block, 4);
	}
	gf_bs_write_u8(bs, ptr->isProtected);

	gf_bs_write_u8(bs, ptr->key_info[3]);
	gf_bs_write_data(bs, ptr->key_info + 4, 16);
	if ((ptr->isProtected == 1) && !ptr->key_info[3]) {
		gf_bs_write_u8(bs, ptr->key_info[20]);
		gf_bs_write_data(bs, ptr->key_info + 21, ptr->key_info[20]);
	}
	return GF_OK;
}