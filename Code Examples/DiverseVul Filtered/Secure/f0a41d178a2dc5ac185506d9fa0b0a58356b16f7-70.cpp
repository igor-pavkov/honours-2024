Bool gf_isom_enable_raw_pack(GF_ISOFile *the_file, u32 trackNumber, u32 pack_num_samples)
{
	u32 afmt, bps, nb_ch;
	Bool from_qt=GF_FALSE;
	GF_TrackBox *trak;
	GF_MPEGAudioSampleEntryBox *entry;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_FALSE;
	trak->pack_num_samples = 0;
	//we only activate sample packing for raw audio
	if (!trak->Media || !trak->Media->handler) return GF_FALSE;
	if (trak->Media->handler->handlerType != GF_ISOM_MEDIA_AUDIO) return GF_FALSE;
	//and sample duration of 1
	if (!trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->TimeToSample) return GF_FALSE;
	if (trak->Media->information->sampleTable->TimeToSample->nb_entries != 1) return GF_FALSE;
	if (!trak->Media->information->sampleTable->TimeToSample->entries) return GF_FALSE;
	if (trak->Media->information->sampleTable->TimeToSample->entries[0].sampleDelta != 1) return GF_FALSE;
	//and sample with constant size
	if (!trak->Media->information->sampleTable->SampleSize || !trak->Media->information->sampleTable->SampleSize->sampleSize) return GF_FALSE;
	trak->pack_num_samples = pack_num_samples;

	if (!pack_num_samples) return GF_FALSE;

	entry = gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, 0);
	if (!entry) return GF_FALSE;

	if (entry->internal_type!=GF_ISOM_SAMPLE_ENTRY_AUDIO) return GF_FALSE;

	//sanity check, some files have wrong stsz sampleSize for raw audio !
	afmt = gf_audio_fmt_from_isobmf(entry->type);
	bps = gf_audio_fmt_bit_depth(afmt) / 8;
	if (!bps) {
		//unknown format, try QTv2
		if (entry->qtff_mode && (entry->internal_type==GF_ISOM_SAMPLE_ENTRY_AUDIO)) {
			bps = entry->extensions[8]<<24 | entry->extensions[9]<<16 | entry->extensions[10]<<8 | entry->extensions[11];
			from_qt = GF_TRUE;
		}
	}
	nb_ch = entry->channel_count;
	if (entry->qtff_mode && (entry->version==2)) {
		//QTFFv2 audio, channel count is 32 bit, after 32bit size of struct and 64 bit samplerate
		//hence start at 12 in extensions
		nb_ch = entry->extensions[11]<<24 | entry->extensions[12]<<16 | entry->extensions[13]<<8 | entry->extensions[14];
	}

	if (bps) {
		u32 res = trak->Media->information->sampleTable->SampleSize->sampleSize % bps;
		if (res) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("ISOBMF: size mismatch for raw audio sample description: constant sample size %d but %d bytes per channel for %s%s!\n", trak->Media->information->sampleTable->SampleSize->sampleSize,
					bps,
					gf_4cc_to_str(entry->type),
					from_qt ? " (as indicated in QT sample description)" : ""
				));
			trak->Media->information->sampleTable->SampleSize->sampleSize = bps * nb_ch;
		}
	}
	return GF_TRUE;
}