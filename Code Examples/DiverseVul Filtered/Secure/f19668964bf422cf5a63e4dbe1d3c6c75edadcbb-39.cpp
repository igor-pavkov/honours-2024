GF_Err aeib_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_AdobeEncryptionInfoBox *ptr = (GF_AdobeEncryptionInfoBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	if (ptr->enc_algo) {
		gf_bs_write_data(bs, (char *) ptr->enc_algo, (u32) strlen(ptr->enc_algo));
		gf_bs_write_u8(bs, 0); //string end
	}
	gf_bs_write_u8(bs, ptr->key_length);
	return GF_OK;
}