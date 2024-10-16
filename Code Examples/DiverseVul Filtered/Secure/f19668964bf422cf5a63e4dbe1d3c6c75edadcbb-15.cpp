GF_Err senc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	u32 sample_count, nb_crypt_samples;
	GF_SampleEncryptionBox *ptr = (GF_SampleEncryptionBox *) s;

	sample_count = gf_list_count(ptr->samp_aux_info);
	//temp patch until we cleanup the spec...
	nb_crypt_samples = 0;
	for (i = 0; i < sample_count; i++) {
		GF_CENCSampleAuxInfo *sai = (GF_CENCSampleAuxInfo *)gf_list_get(ptr->samp_aux_info, i);
		if (!sai->isNotProtected) nb_crypt_samples++;
	}
	if (!nb_crypt_samples) {
		ptr->size = 0;
		return GF_OK;
	}

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	//WARNING - PSEC (UUID) IS TYPECASTED TO SENC (FULL BOX) SO WE CANNOT USE USUAL FULL BOX FUNCTIONS
	gf_bs_write_u8(bs, ptr->version);
	gf_bs_write_u24(bs, ptr->flags);

	gf_bs_write_u32(bs, nb_crypt_samples);

	e = store_senc_info(ptr, bs);
	if (e) return e;

	for (i = 0; i < sample_count; i++) {
		GF_CENCSampleAuxInfo *sai = (GF_CENCSampleAuxInfo *)gf_list_get(ptr->samp_aux_info, i);
		if (sai->isNotProtected || !sai->cenc_data_size)
			continue;
		gf_bs_write_data(bs, sai->cenc_data, sai->cenc_data_size);
	}
	return GF_OK;
}