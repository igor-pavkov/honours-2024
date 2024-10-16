u32 gf_isom_is_media_encrypted(GF_ISOFile *the_file, u32 trackNumber, u32 sampleDescriptionIndex)
{
	GF_TrackBox *trak;
	u32 i, count;
	GF_ProtectionSchemeInfoBox *sinf;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return 0;
	count = gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes);
	for (i=0; i<count; i++) {
		if (sampleDescriptionIndex && (i+1 != sampleDescriptionIndex))
			continue;

		sinf = isom_get_sinf_entry(trak, i+1, 0, NULL);
		if (!sinf) continue;

		/*non-encrypted or non-ISMA*/
		if (!sinf->scheme_type) return 0;
		if (sinf->scheme_type->scheme_type == GF_ISOM_PIFF_SCHEME) return GF_ISOM_CENC_SCHEME;
		return sinf->scheme_type->scheme_type;
	}
	return 0;
}