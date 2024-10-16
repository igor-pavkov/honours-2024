GF_Err gf_isom_set_oma_protection(GF_ISOFile *the_file, u32 trackNumber, u32 desc_index,
                                  char *contentID, char *kms_URI, u32 encryption_type, u64 plainTextLength, char *textual_headers, u32 textual_headers_len,
                                  Bool selective_encryption, u32 KI_length, u32 IV_length)
{
	GF_ProtectionSchemeInfoBox *sinf;
	GF_Err e;

	//setup generic protection
	e = isom_set_protected_entry(the_file, trackNumber, desc_index, 0, 0, GF_ISOM_OMADRM_SCHEME, 0x00000200, NULL, GF_FALSE, &sinf);
	if (e) return e;

	sinf->info->odkm = (GF_OMADRMKMSBox *)gf_isom_box_new_parent(&sinf->info->child_boxes, GF_ISOM_BOX_TYPE_ODKM);
	if (!sinf->info->odkm) return GF_OUT_OF_MEM;
	sinf->info->odkm->fmt = (GF_OMADRMAUFormatBox*)gf_isom_box_new_parent(&sinf->info->odkm->child_boxes, GF_ISOM_BOX_TYPE_ODAF);
	if (!sinf->info->odkm->fmt) return GF_OUT_OF_MEM;
	sinf->info->odkm->fmt->selective_encryption = selective_encryption;
	sinf->info->odkm->fmt->key_indicator_length = KI_length;
	sinf->info->odkm->fmt->IV_length = IV_length;

	sinf->info->odkm->hdr = (GF_OMADRMCommonHeaderBox*)gf_isom_box_new_parent(&sinf->info->odkm->child_boxes, GF_ISOM_BOX_TYPE_OHDR);
	if (!sinf->info->odkm->hdr) return GF_OUT_OF_MEM;
	sinf->info->odkm->hdr->EncryptionMethod = encryption_type;
	sinf->info->odkm->hdr->PaddingScheme = (encryption_type==0x01) ? 1 : 0;
	sinf->info->odkm->hdr->PlaintextLength = plainTextLength;
	if (contentID) sinf->info->odkm->hdr->ContentID = gf_strdup(contentID);
	if (kms_URI) sinf->info->odkm->hdr->RightsIssuerURL = gf_strdup(kms_URI);
	if (textual_headers) {
		sinf->info->odkm->hdr->TextualHeaders = (char*)gf_malloc(sizeof(char)*textual_headers_len);
		if (!sinf->info->odkm->hdr->TextualHeaders) return GF_OUT_OF_MEM;
		memcpy(sinf->info->odkm->hdr->TextualHeaders, textual_headers, sizeof(char)*textual_headers_len);
		sinf->info->odkm->hdr->TextualHeadersLen = textual_headers_len;
	}
	return GF_OK;
}