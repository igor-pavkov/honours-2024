GF_Err piff_tenc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_PIFFTrackEncryptionBox *ptr = (GF_PIFFTrackEncryptionBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, ptr->version);
	gf_bs_write_u24(bs, ptr->flags);

	gf_bs_write_int(bs, ptr->AlgorithmID, 24);
	gf_bs_write_u8(bs, ptr->key_info[3]);
	gf_bs_write_data(bs, ptr->key_info+4, 16);
	return GF_OK;
}