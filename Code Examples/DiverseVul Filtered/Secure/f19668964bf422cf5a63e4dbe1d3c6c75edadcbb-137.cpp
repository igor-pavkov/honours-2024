GF_Err piff_psec_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_SampleEncryptionBox *ptr = (GF_SampleEncryptionBox *)s;

	ISOM_DECREASE_SIZE(ptr, 4);
	//PIFF PSEC extends UUID and fullbox
	ptr->version = gf_bs_read_u8(bs);
	ptr->flags = gf_bs_read_u24(bs);

	if (ptr->flags & 1) {
		ISOM_DECREASE_SIZE(ptr, 20);
		ptr->AlgorithmID = gf_bs_read_int(bs, 24);
		ptr->IV_size = gf_bs_read_u8(bs);
		gf_bs_read_data(bs, (char *) ptr->KID, 16);
	}
	if (ptr->IV_size == 0)
		ptr->IV_size = 8; //default to 8

	ptr->bs_offset = gf_bs_get_position(bs);

	/*u32 sample_count = */gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, 4);
	if (ptr->IV_size != 8 && ptr->IV_size != 16) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] PIFF PSEC box incorrect IV size: %u - shall be 8 or 16\n", ptr->IV_size));
		return GF_BAD_PARAM;
	}
	//as for senc, we skip parsing of the box until we have all saiz/saio info
	gf_bs_skip_bytes(bs, ptr->size);
	ptr->size = 0;
	return GF_OK;
}