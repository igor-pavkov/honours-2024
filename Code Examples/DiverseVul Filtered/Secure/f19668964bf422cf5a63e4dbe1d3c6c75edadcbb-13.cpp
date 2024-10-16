GF_Err senc_box_size(GF_Box *s)
{
	u32 sample_count;
	u32 i, nb_crypt_samples;
	GF_SampleEncryptionBox *ptr = (GF_SampleEncryptionBox*)s;
	sample_count = gf_list_count(ptr->samp_aux_info);
	//temp patch until we cleanup the spec...
	nb_crypt_samples=0;
	for (i = 0; i < sample_count; i++) {
		GF_CENCSampleAuxInfo *sai = (GF_CENCSampleAuxInfo *)gf_list_get(ptr->samp_aux_info, i);
		if (!sai->isNotProtected) nb_crypt_samples++;
	}

	if (!nb_crypt_samples) {
		ptr->size = 0;
		return GF_OK;
	}

	//WARNING - PSEC (UUID) IS TYPECASTED TO SENC (FULL BOX) SO WE CANNOT USE USUAL FULL BOX FUNCTIONS
	ptr->size += 4; //version and flags

	ptr->size += 4; //sample count
	for (i = 0; i < sample_count; i++) {
		GF_CENCSampleAuxInfo *sai = (GF_CENCSampleAuxInfo *)gf_list_get(ptr->samp_aux_info, i);
		if (sai->isNotProtected)
			continue;
		ptr->size += sai->cenc_data_size;
	}
	return GF_OK;
}