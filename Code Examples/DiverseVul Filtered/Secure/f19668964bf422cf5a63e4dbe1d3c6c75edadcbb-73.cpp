GF_Err tenc_box_read(GF_Box *s, GF_BitStream *bs)
{
	u8 iv_size;
	GF_TrackEncryptionBox *ptr = (GF_TrackEncryptionBox*)s;

	ISOM_DECREASE_SIZE(ptr, 3);

	gf_bs_read_u8(bs); //reserved

	if (!ptr->version) {
		gf_bs_read_u8(bs); //reserved
	} else {
		ptr->crypt_byte_block = gf_bs_read_int(bs, 4);
		ptr->skip_byte_block = gf_bs_read_int(bs, 4);
	}
	ptr->isProtected = gf_bs_read_u8(bs);


	ISOM_DECREASE_SIZE(ptr, 17);

	ptr->key_info[0] = 0;
	ptr->key_info[1] = 0;
	ptr->key_info[2] = 0;
	ptr->key_info[3] = iv_size = gf_bs_read_u8(bs);
	gf_bs_read_data(bs, ptr->key_info+4, 16);
	if (!iv_size && ptr->isProtected) {
		ISOM_DECREASE_SIZE(ptr, 1);
		iv_size = ptr->key_info[20] = gf_bs_read_u8(bs);
		ISOM_DECREASE_SIZE(ptr, ptr->key_info[20]);
		if ((iv_size!=8) && (iv_size!=16)) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid constant IV size %d, must be 8 or 16\n", (u32) iv_size));
			ptr->key_info[20] = 16;
			return GF_NON_COMPLIANT_BITSTREAM;
		}
		gf_bs_read_data(bs, ptr->key_info+21, iv_size);
	}
	else if ((iv_size!=0) && (iv_size!=8) && (iv_size!=16)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid IV size %d, must be 0, 8 or 16\n", (u32) iv_size));
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	return GF_OK;
}