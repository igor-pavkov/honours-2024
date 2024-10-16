GF_Err gf_isom_track_cenc_add_sample_info(GF_ISOFile *the_file, u32 trackNumber, u32 container_type, u8 *buf, u32 len, Bool use_subsamples, Bool use_saio_32bit, Bool use_multikey)
{
	GF_SampleEncryptionBox *senc;
	GF_CENCSampleAuxInfo *sai;
	GF_SampleTableBox *stbl;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	stbl = trak->Media->information->sampleTable;
	if (!stbl) return GF_BAD_PARAM;

	switch (container_type) {
	case GF_ISOM_BOX_UUID_PSEC:
	case GF_ISOM_BOX_TYPE_SENC:
	case 0:
		senc = trak->sample_encryption;
		break;
	default:
		return GF_NOT_SUPPORTED;
	}

	if (len && buf) {
		GF_SAFEALLOC(sai, GF_CENCSampleAuxInfo);
		if (!sai) return GF_OUT_OF_MEM;
		sai->cenc_data_size = len;
		sai->cenc_data = gf_malloc(sizeof(u8) * len);
		if (!sai->cenc_data) {
			gf_free(sai);
			return GF_OUT_OF_MEM;
		}
		memcpy(sai->cenc_data, buf, len);

		gf_list_add(senc->samp_aux_info, sai);
	} else {
		GF_SAFEALLOC(sai, GF_CENCSampleAuxInfo);
		if (!sai) return GF_OUT_OF_MEM;
		gf_list_add(senc->samp_aux_info, sai);
		sai->isNotProtected = 1;
	}
	if (use_subsamples)
		senc->flags = 0x00000002;
	if (use_multikey)
		senc->version = 1;

#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	gf_isom_cenc_set_saiz_saio(senc, stbl, NULL, sai->cenc_data_size, use_saio_32bit, use_multikey);
#endif

	return GF_OK;
}