GF_Err gf_isom_get_audio_info(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex, u32 *SampleRate, u32 *Channels, u32 *bitsPerSample)
{
	GF_TrackBox *trak;
	GF_AudioSampleEntryBox *entry;
	GF_SampleDescriptionBox *stsd = NULL;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	if (trak->Media && trak->Media->information && trak->Media->information->sampleTable && trak->Media->information->sampleTable->SampleDescription)
		stsd = trak->Media->information->sampleTable->SampleDescription;
	if (!stsd) return movie->LastError = GF_ISOM_INVALID_FILE;
	if (!StreamDescriptionIndex || StreamDescriptionIndex > gf_list_count(stsd->child_boxes)) return movie->LastError = GF_BAD_PARAM;

	entry = (GF_AudioSampleEntryBox *)gf_list_get(stsd->child_boxes, StreamDescriptionIndex - 1);
	//no support for generic sample entries (eg, no MPEG4 descriptor)
	if (entry == NULL) return GF_BAD_PARAM;

	if (entry->internal_type != GF_ISOM_SAMPLE_ENTRY_AUDIO) return GF_BAD_PARAM;

	if (SampleRate) {
		(*SampleRate) = entry->samplerate_hi;
		if (entry->type==GF_ISOM_BOX_TYPE_MLPA) {
			u32 sr = entry->samplerate_hi;
			sr <<= 16;
			sr |= entry->samplerate_lo;
			(*SampleRate) = sr;
		}
	}
	if (Channels) (*Channels) = entry->channel_count;
	if (bitsPerSample) (*bitsPerSample) = (u8) entry->bitspersample;

	return GF_OK;
}