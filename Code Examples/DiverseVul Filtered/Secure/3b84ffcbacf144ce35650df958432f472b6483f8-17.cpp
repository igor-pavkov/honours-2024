GF_Err gf_isom_cenc_get_sample_aux_info(GF_ISOFile *the_file, u32 trackNumber, u32 sampleNumber, u32 sampleDescIndex, u32 *container_type, u8 **out_buffer, u32 *outSize)
{
	GF_TrackBox *trak;
	GF_SampleTableBox *stbl;
	GF_SampleEncryptionBox *senc = NULL;
	u32 type, scheme_type = -1;
	GF_CENCSampleAuxInfo *a_sai;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	stbl = trak->Media->information->sampleTable;
	if (!stbl)
		return GF_BAD_PARAM;

	type = 0;
	senc = trak->sample_encryption;
	//no senc is OK
	if (senc) {
		if ((senc->type == GF_ISOM_BOX_TYPE_UUID) && (((GF_UUIDBox *)senc)->internal_4cc == GF_ISOM_BOX_UUID_PSEC)) {
			type = GF_ISOM_BOX_UUID_PSEC;
		} else if (senc->type == GF_ISOM_BOX_TYPE_SENC) {
			type = GF_ISOM_BOX_TYPE_SENC;
		}

		if (container_type) *container_type = type;
	}

	if (!out_buffer) return GF_OK; /*we need only container_type*/

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	sampleNumber -= trak->sample_count_at_seg_start;
#endif

	gf_isom_get_cenc_info(the_file, trackNumber, sampleDescIndex, NULL, &scheme_type, NULL);

	/*get sample auxiliary information by saiz/saio rather than by parsing senc box*/
	if (gf_isom_cenc_has_saiz_saio_track(stbl, scheme_type)) {
		return isom_cenc_get_sai_by_saiz_saio(trak->Media, sampleNumber, scheme_type, out_buffer, outSize);
	}
	if (!senc)
		return GF_OK;

	//senc is not loaded by default, do it now
	if (!gf_list_count(senc->samp_aux_info)) {
		GF_Err e = senc_Parse(trak->Media->information->dataHandler->bs, trak, NULL, senc);
		if (e) return e;
	}

	a_sai = NULL;
	switch (type) {
	case GF_ISOM_BOX_UUID_PSEC:
		if (senc)
			a_sai = (GF_CENCSampleAuxInfo *)gf_list_get(senc->samp_aux_info, sampleNumber-1);
		break;
	case GF_ISOM_BOX_TYPE_SENC:
		if (senc)
			a_sai = (GF_CENCSampleAuxInfo *)gf_list_get(senc->samp_aux_info, sampleNumber-1);
		break;
	}
	//not present, check we use constant IV and no IV size
	if (!a_sai) {
		const u8 *key_info=NULL;
		u32 key_info_size=0;
		u8 IV_size=0, constant_IV_size=0;
		Bool is_Protected;

		gf_isom_get_sample_cenc_info_internal(trak, NULL, senc, sampleNumber, &is_Protected, NULL, NULL, &key_info, &key_info_size);
		if (!key_info) {
			IV_size = key_info_size; //piff default
		} else {
			IV_size = key_info[3];
			if (!IV_size)
				constant_IV_size = key_info[20];
		}
		if (!IV_size && constant_IV_size)
			return GF_OK;
		return GF_NOT_FOUND;
	}

	if (*outSize < a_sai->cenc_data_size) {
		*out_buffer = gf_realloc(*out_buffer, sizeof(char) * a_sai->cenc_data_size);
		if (! *out_buffer) return GF_OUT_OF_MEM;
		*outSize = a_sai->cenc_data_size;
	}
	memcpy(*out_buffer, a_sai->cenc_data, a_sai->cenc_data_size);
	return GF_OK;
}