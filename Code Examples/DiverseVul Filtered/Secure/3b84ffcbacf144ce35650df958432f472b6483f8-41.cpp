GF_ISMASample *gf_isom_get_ismacryp_sample(GF_ISOFile *the_file, u32 trackNumber, const GF_ISOSample *samp, u32 sampleDescriptionIndex)
{
	GF_TrackBox *trak;
	GF_ISMASampleFormatBox *fmt;
	GF_ProtectionSchemeInfoBox *sinf;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return NULL;

	sinf = isom_get_sinf_entry(trak, sampleDescriptionIndex, 0, NULL);
	if (!sinf) return NULL;

	/*ISMA*/
	if (sinf->scheme_type->scheme_type == GF_ISOM_ISMACRYP_SCHEME) {
		fmt = sinf->info->isfm;
		if (!fmt) return NULL;
		return gf_isom_ismacryp_sample_from_data(samp->data, samp->dataLength, sinf->info->isfm->selective_encryption, sinf->info->isfm->key_indicator_length, sinf->info->isfm->IV_length);
	}
	/*OMA*/
	else if (sinf->scheme_type->scheme_type == GF_ISOM_OMADRM_SCHEME ) {
		if (!sinf->info->odkm) return NULL;
		fmt = sinf->info->odkm->fmt;

		if (fmt) {
			return gf_isom_ismacryp_sample_from_data(samp->data, samp->dataLength, fmt->selective_encryption, fmt->key_indicator_length, fmt->IV_length);
		}
		/*OMA default: no selective encryption, one key, 128 bit IV*/
		return gf_isom_ismacryp_sample_from_data(samp->data, samp->dataLength, GF_FALSE, 0, 128);
	}
	return NULL;
}