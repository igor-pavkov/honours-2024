GF_Err gf_isom_set_adobe_protection(GF_ISOFile *the_file, u32 trackNumber, u32 desc_index, u32 scheme_type, u32 scheme_version, Bool is_selective_enc, char *metadata, u32 len)
{
	GF_ProtectionSchemeInfoBox *sinf;

	//setup generic protection
#ifndef GPAC_DISABLE_ISOM_WRITE
	GF_Err e;
	e = isom_set_protected_entry(the_file, trackNumber, desc_index, 1, 0, scheme_type, scheme_version, NULL, GF_FALSE, &sinf);
	if (e) return e;
#else
	return GF_NOT_SUPPORTED;
#endif

	sinf->info->adkm = (GF_AdobeDRMKeyManagementSystemBox *)gf_isom_box_new_parent(&sinf->info->child_boxes, GF_ISOM_BOX_TYPE_ADKM);
	if (!sinf->info->adkm) return GF_OUT_OF_MEM;

	sinf->info->adkm->header = (GF_AdobeDRMHeaderBox *)gf_isom_box_new_parent(&sinf->info->adkm->child_boxes, GF_ISOM_BOX_TYPE_AHDR);
	if (!sinf->info->adkm->header) return GF_OUT_OF_MEM;

	sinf->info->adkm->header->std_enc_params = (GF_AdobeStdEncryptionParamsBox *)gf_isom_box_new_parent(& sinf->info->adkm->header->child_boxes, GF_ISOM_BOX_TYPE_APRM);
	if (!sinf->info->adkm->header->std_enc_params) return GF_OUT_OF_MEM;

	sinf->info->adkm->header->std_enc_params->enc_info = (GF_AdobeEncryptionInfoBox *)gf_isom_box_new_parent(&sinf->info->adkm->header->std_enc_params->child_boxes, GF_ISOM_BOX_TYPE_AEIB);
	if (!sinf->info->adkm->header->std_enc_params->enc_info) return GF_OUT_OF_MEM;

	sinf->info->adkm->header->std_enc_params->enc_info->enc_algo = (char *)gf_malloc(8*sizeof(char));
	if (!sinf->info->adkm->header->std_enc_params->enc_info->enc_algo) return GF_OUT_OF_MEM;

	strcpy(sinf->info->adkm->header->std_enc_params->enc_info->enc_algo, "AES-CBC");
	sinf->info->adkm->header->std_enc_params->enc_info->key_length = 16;

	sinf->info->adkm->header->std_enc_params->key_info = (GF_AdobeKeyInfoBox *)gf_isom_box_new_parent(&sinf->info->adkm->header->std_enc_params->child_boxes, GF_ISOM_BOX_TYPE_AKEY);
	if (!sinf->info->adkm->header->std_enc_params->key_info) return GF_OUT_OF_MEM;

	sinf->info->adkm->header->std_enc_params->key_info->params = (GF_AdobeFlashAccessParamsBox *)gf_isom_box_new_parent(&sinf->info->adkm->header->std_enc_params->key_info->child_boxes, GF_ISOM_BOX_TYPE_FLXS);
	if (!sinf->info->adkm->header->std_enc_params->key_info->params) return GF_OUT_OF_MEM;

	if (metadata && len) {
		sinf->info->adkm->header->std_enc_params->key_info->params->metadata = (char *)gf_malloc((len+1)*sizeof(char));
		if (!sinf->info->adkm->header->std_enc_params->key_info->params->metadata) return GF_OUT_OF_MEM;

		strncpy(sinf->info->adkm->header->std_enc_params->key_info->params->metadata, metadata, len);
		sinf->info->adkm->header->std_enc_params->key_info->params->metadata[len] = 0;
	}

	sinf->info->adkm->au_format = (GF_AdobeDRMAUFormatBox *)gf_isom_box_new_parent(&sinf->info->adkm->child_boxes, GF_ISOM_BOX_TYPE_ADAF);
	if (!sinf->info->adkm->au_format) return GF_OUT_OF_MEM;

	sinf->info->adkm->au_format->selective_enc = is_selective_enc ? 0x10 : 0x00;
	sinf->info->adkm->au_format->IV_length = 16;

	return GF_OK;
}