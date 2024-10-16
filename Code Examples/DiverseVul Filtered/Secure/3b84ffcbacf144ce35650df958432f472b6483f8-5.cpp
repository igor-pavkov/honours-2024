GF_Err gf_isom_set_cenc_protection(GF_ISOFile *the_file, u32 trackNumber, u32 desc_index, u32 scheme_type,
                                   u32 scheme_version, u32 default_IsEncrypted, u8 default_crypt_byte_block, u8 default_skip_byte_block,
								    u8 *key_info, u32 key_info_size)
{
	GF_Err e;
	GF_ProtectionSchemeInfoBox *sinf;

	if (!gf_cenc_validate_key_info(key_info, key_info_size))
		return GF_BAD_PARAM;

	//setup generic protection
	e = isom_set_protected_entry(the_file, trackNumber, desc_index, 0, 0, scheme_type, scheme_version, NULL, GF_FALSE, &sinf);
	if (e) return e;

	if (scheme_type==GF_ISOM_PIFF_SCHEME) {
		sinf->info->piff_tenc = (GF_PIFFTrackEncryptionBox *) gf_isom_box_new_parent(&sinf->info->child_boxes, GF_ISOM_BOX_UUID_TENC);
		if (!sinf->info->piff_tenc) return GF_OUT_OF_MEM;
		sinf->info->piff_tenc->AlgorithmID = 1;
		sinf->info->piff_tenc->key_info[0] = 0;
		sinf->info->piff_tenc->key_info[1] = 0;
		sinf->info->piff_tenc->key_info[2] = 0;
		sinf->info->piff_tenc->key_info[3] = key_info[3];
		memcpy(sinf->info->piff_tenc->key_info+4, key_info+4, 16*sizeof(char));
	}
	//tenc only for mkey
	else if (!key_info[0]) {
		if (key_info_size<20) return GF_BAD_PARAM;
		sinf->info->tenc = (GF_TrackEncryptionBox *)gf_isom_box_new_parent(&sinf->info->child_boxes, GF_ISOM_BOX_TYPE_TENC);
		if (!sinf->info->tenc) return GF_OUT_OF_MEM;

		sinf->info->tenc->isProtected = default_IsEncrypted;
		if ((scheme_type == GF_ISOM_CENS_SCHEME) || (scheme_type == GF_ISOM_CBCS_SCHEME)) {
			sinf->info->tenc->version = 1;
			sinf->info->tenc->crypt_byte_block = default_crypt_byte_block;
			sinf->info->tenc->skip_byte_block = default_skip_byte_block;
		}
		if (key_info_size>37) key_info_size = 37;
		memcpy(sinf->info->tenc->key_info, key_info, key_info_size);
	}
	return GF_OK;
}