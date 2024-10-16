GF_Err gf_isom_set_generic_protection(GF_ISOFile *the_file, u32 trackNumber, u32 desc_index, u32 scheme_type, u32 scheme_version, char *scheme_uri, char *kms_URI)
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

	if (kms_URI) {
		sinf->info->ikms = (GF_ISMAKMSBox *)gf_isom_box_new_parent(&sinf->info->child_boxes, GF_ISOM_BOX_TYPE_IKMS);
		sinf->info->ikms->URI = gf_strdup(kms_URI);
	}
	return GF_OK;
}