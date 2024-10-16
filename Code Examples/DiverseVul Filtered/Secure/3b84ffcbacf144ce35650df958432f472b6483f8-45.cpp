GF_Err gf_isom_get_ismacryp_info(GF_ISOFile *the_file, u32 trackNumber, u32 sampleDescriptionIndex, u32 *outOriginalFormat, u32 *outSchemeType, u32 *outSchemeVersion, const char **outSchemeURI, const char **outKMS_URI, Bool *outSelectiveEncryption, u32 *outIVLength, u32 *outKeyIndicationLength)
{
	GF_TrackBox *trak;
	GF_ProtectionSchemeInfoBox *sinf;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_ISMACRYP_SCHEME, NULL);
	if (!sinf) return GF_OK;

	if (outOriginalFormat) {
		*outOriginalFormat = sinf->original_format->data_format;
		if (IsMP4Description(sinf->original_format->data_format)) *outOriginalFormat = GF_ISOM_SUBTYPE_MPEG4;
	}
	if (outSchemeType) *outSchemeType = sinf->scheme_type->scheme_type;
	if (outSchemeVersion) *outSchemeVersion = sinf->scheme_type->scheme_version;
	if (outSchemeURI) *outSchemeURI = sinf->scheme_type->URI;

	if (sinf->info && sinf->info->ikms) {
		if (outKMS_URI) *outKMS_URI = sinf->info->ikms->URI;
	} else {
		if (outKMS_URI) *outKMS_URI = NULL;
	}
	if (sinf->info && sinf->info->isfm) {
		if (outSelectiveEncryption) *outSelectiveEncryption = sinf->info->isfm->selective_encryption;
		if (outIVLength) *outIVLength = sinf->info->isfm->IV_length;
		if (outKeyIndicationLength) *outKeyIndicationLength = sinf->info->isfm->key_indicator_length;
	} else {
		if (outSelectiveEncryption) *outSelectiveEncryption = GF_FALSE;
		if (outIVLength) *outIVLength = 0;
		if (outKeyIndicationLength) *outKeyIndicationLength = 0;
	}
	return GF_OK;
}