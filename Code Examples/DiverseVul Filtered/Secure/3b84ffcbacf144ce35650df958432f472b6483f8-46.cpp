GF_Err gf_isom_set_ismacryp_protection(GF_ISOFile *the_file, u32 trackNumber, u32 desc_index, u32 scheme_type,
                                       u32 scheme_version, char *scheme_uri, char *kms_URI,
                                       Bool selective_encryption, u32 KI_length, u32 IV_length)
{
	GF_Err e;
	GF_ProtectionSchemeInfoBox *sinf;

	//setup generic protection
	e = isom_set_protected_entry(the_file, trackNumber, desc_index, 0, 0, scheme_type, scheme_version, NULL, GF_TRUE, &sinf);
	if (e) return e;

	if (scheme_uri) {
		sinf->scheme_type->flags |= 0x000001;
		sinf->scheme_type->URI = gf_strdup(scheme_uri);
	}

	sinf->info->ikms = (GF_ISMAKMSBox *)gf_isom_box_new_parent(&sinf->info->child_boxes, GF_ISOM_BOX_TYPE_IKMS);
	if (!sinf->info->ikms) return GF_OUT_OF_MEM;
	sinf->info->ikms->URI = gf_strdup(kms_URI);
	if (!sinf->info->ikms->URI) return GF_OUT_OF_MEM;

	sinf->info->isfm = (GF_ISMASampleFormatBox *)gf_isom_box_new_parent(&sinf->info->child_boxes, GF_ISOM_BOX_TYPE_ISFM);
	if (!sinf->info->isfm) return GF_OUT_OF_MEM;

	sinf->info->isfm->selective_encryption = selective_encryption;
	sinf->info->isfm->key_indicator_length = KI_length;
	sinf->info->isfm->IV_length = IV_length;
	return GF_OK;
}