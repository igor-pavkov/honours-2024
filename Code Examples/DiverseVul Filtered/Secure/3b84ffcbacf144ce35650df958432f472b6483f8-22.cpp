GF_Err gf_isom_get_adobe_protection_info(GF_ISOFile *the_file, u32 trackNumber, u32 sampleDescriptionIndex, u32 *outOriginalFormat, u32 *outSchemeType, u32 *outSchemeVersion, const char **outMetadata)
{
	GF_TrackBox *trak;
	GF_ProtectionSchemeInfoBox *sinf;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_ADOBE_SCHEME, NULL);

	if (!sinf) return GF_BAD_PARAM;

	if (outOriginalFormat) {
		*outOriginalFormat = sinf->original_format->data_format;
		if (IsMP4Description(sinf->original_format->data_format)) *outOriginalFormat = GF_ISOM_SUBTYPE_MPEG4;
	}
	if (outSchemeType) *outSchemeType = sinf->scheme_type->scheme_type;
	if (outSchemeVersion) *outSchemeVersion = sinf->scheme_type->scheme_version;

	if (outMetadata) {
		*outMetadata = NULL;
		if (sinf->info && sinf->info->adkm && sinf->info->adkm->header && sinf->info->adkm->header->std_enc_params && sinf->info->adkm->header->std_enc_params->key_info
			&& sinf->info->adkm->header->std_enc_params->key_info->params && sinf->info->adkm->header->std_enc_params->key_info->params->metadata)
		{
			*outMetadata = sinf->info->adkm->header->std_enc_params->key_info->params->metadata;
		}
	}

	return GF_OK;
}