GF_Err gf_isom_get_omadrm_info(GF_ISOFile *the_file, u32 trackNumber, u32 sampleDescriptionIndex, u32 *outOriginalFormat,
                               u32 *outSchemeType, u32 *outSchemeVersion,
                               const char **outContentID, const char **outRightsIssuerURL, const char **outTextualHeaders, u32 *outTextualHeadersLen, u64 *outPlaintextLength, u32 *outEncryptionType, Bool *outSelectiveEncryption, u32 *outIVLength, u32 *outKeyIndicationLength)
{
	GF_TrackBox *trak;
	GF_ProtectionSchemeInfoBox *sinf;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, GF_ISOM_OMADRM_SCHEME, NULL);
	if (!sinf) return GF_OK;

	if (!sinf->info || !sinf->info->odkm || !sinf->info->odkm->hdr) return GF_NON_COMPLIANT_BITSTREAM;

	if (outOriginalFormat) {
		*outOriginalFormat = sinf->original_format->data_format;
		if (IsMP4Description(sinf->original_format->data_format)) *outOriginalFormat = GF_ISOM_SUBTYPE_MPEG4;
	}
	if (outSchemeType) *outSchemeType = sinf->scheme_type->scheme_type;
	if (outSchemeVersion) *outSchemeVersion = sinf->scheme_type->scheme_version;
	if (outContentID) *outContentID = sinf->info->odkm->hdr->ContentID;
	if (outRightsIssuerURL) *outRightsIssuerURL = sinf->info->odkm->hdr->RightsIssuerURL;
	if (outTextualHeaders) {
		*outTextualHeaders = sinf->info->odkm->hdr->TextualHeaders;
		if (outTextualHeadersLen) *outTextualHeadersLen = sinf->info->odkm->hdr->TextualHeadersLen;
	}
	if (outPlaintextLength) *outPlaintextLength = sinf->info->odkm->hdr->PlaintextLength;
	if (outEncryptionType) *outEncryptionType = sinf->info->odkm->hdr->EncryptionMethod;

	if (sinf->info && sinf->info->odkm && sinf->info->odkm->fmt) {
		if (outSelectiveEncryption) *outSelectiveEncryption = sinf->info->odkm->fmt->selective_encryption;
		if (outIVLength) *outIVLength = sinf->info->odkm->fmt->IV_length;
		if (outKeyIndicationLength) *outKeyIndicationLength = sinf->info->odkm->fmt->key_indicator_length;
	} else {
		if (outSelectiveEncryption) *outSelectiveEncryption = GF_FALSE;
		if (outIVLength) *outIVLength = 0;
		if (outKeyIndicationLength) *outKeyIndicationLength = 0;
	}
	return GF_OK;
}