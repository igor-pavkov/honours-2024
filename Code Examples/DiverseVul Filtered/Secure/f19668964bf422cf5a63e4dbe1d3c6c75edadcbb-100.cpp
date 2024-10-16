GF_Err aeib_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_AdobeEncryptionInfoBox *ptr = (GF_AdobeEncryptionInfoBox*)s;
	u32 len;

	len = (u32) ptr->size - 1;
	if (len) {
		ptr->enc_algo = (char *)gf_malloc(len*sizeof(char));
		if (!ptr->enc_algo) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->enc_algo, len);
	}
	ptr->key_length = gf_bs_read_u8(bs);
	ptr->size = 0;
	return GF_OK;
}