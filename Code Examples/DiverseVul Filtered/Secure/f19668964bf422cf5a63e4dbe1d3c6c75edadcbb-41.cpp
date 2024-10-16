GF_Err piff_psec_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 sample_count;
	GF_SampleEncryptionBox *ptr = (GF_SampleEncryptionBox *) s;
	if (!s) return GF_BAD_PARAM;

	sample_count = gf_list_count(ptr->samp_aux_info);
	if (!sample_count) {
		ptr->size = 0;
		return GF_OK;
	}
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, ptr->version);
	gf_bs_write_u24(bs, ptr->flags);

	if (ptr->flags & 1) {
		gf_bs_write_int(bs, ptr->AlgorithmID, 24);
		gf_bs_write_u8(bs, ptr->IV_size);
		gf_bs_write_data(bs, (char *) ptr->KID, 16);
	}
	sample_count = gf_list_count(ptr->samp_aux_info);
	gf_bs_write_u32(bs, sample_count);
	if (sample_count) {
		u32 i;
		e = store_senc_info((GF_SampleEncryptionBox *)ptr, bs);
		if (e) return e;

		for (i = 0; i < sample_count; i++) {
			GF_CENCSampleAuxInfo *sai = (GF_CENCSampleAuxInfo *)gf_list_get(ptr->samp_aux_info, i);
			if (! sai->cenc_data_size) continue;
			gf_bs_write_data(bs, (char *)sai->cenc_data, sai->cenc_data_size);
		}
	}
	return GF_OK;
}