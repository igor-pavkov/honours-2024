GF_Err gf_isom_get_bitrate(GF_ISOFile *movie, u32 trackNumber, u32 sampleDescIndex, u32 *average_bitrate, u32 *max_bitrate, u32 *decode_buffer_size)
{
	GF_BitRateBox *a;
	u32 i, count, mrate, arate, dbsize, type;
	GF_SampleEntryBox *ent;
	GF_ProtectionSchemeInfoBox *sinf;
	GF_TrackBox *trak;
	GF_ESDBox *esd;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media) return GF_BAD_PARAM;

	mrate = arate = dbsize = 0;
	count = gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes);
	for (i=0; i<count; i++) {
		if ((sampleDescIndex>0) && (i+1 != sampleDescIndex)) continue;

		ent = (GF_SampleEntryBox *)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, i);
		if (!ent) return GF_BAD_PARAM;
		a = gf_isom_sample_entry_get_bitrate(ent, GF_FALSE);
		if (a) {
			if (mrate<a->maxBitrate) mrate = a->maxBitrate;
			if (arate<a->avgBitrate) arate = a->avgBitrate;
			if (dbsize<a->bufferSizeDB) dbsize = a->bufferSizeDB;
			continue;
		}
		type = ent->type;
		switch (type) {
		case GF_ISOM_BOX_TYPE_ENCV:
		case GF_ISOM_BOX_TYPE_ENCA:
		case GF_ISOM_BOX_TYPE_ENCS:
			sinf = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(ent->child_boxes, GF_ISOM_BOX_TYPE_SINF);
			if (sinf && sinf->original_format) type = sinf->original_format->data_format;
			break;
		}
		esd = NULL;
		switch (type) {
		case GF_ISOM_BOX_TYPE_MP4V:
			esd = ((GF_MPEGVisualSampleEntryBox *)ent)->esd;
			break;
		case GF_ISOM_BOX_TYPE_MP4A:
			esd = ((GF_MPEGAudioSampleEntryBox *)ent)->esd;
			break;
		case GF_ISOM_BOX_TYPE_MP4S:
			esd = ((GF_MPEGSampleEntryBox *)ent)->esd;
			break;
		}
		if (esd && esd->desc && esd->desc->decoderConfig) {
			if (mrate<esd->desc->decoderConfig->maxBitrate) mrate = esd->desc->decoderConfig->maxBitrate;
			if (arate<esd->desc->decoderConfig->avgBitrate) arate = esd->desc->decoderConfig->avgBitrate;
			if (dbsize<esd->desc->decoderConfig->bufferSizeDB) dbsize = esd->desc->decoderConfig->bufferSizeDB;
		}
	}
	if (average_bitrate) *average_bitrate = arate;
	if (max_bitrate) *max_bitrate = mrate;
	if (decode_buffer_size) *decode_buffer_size = dbsize;
	return GF_OK;
}