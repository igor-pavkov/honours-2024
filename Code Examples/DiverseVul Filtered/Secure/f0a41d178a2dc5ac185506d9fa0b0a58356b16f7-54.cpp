GF_GenericSampleDescription *gf_isom_get_generic_sample_description(GF_ISOFile *movie, u32 trackNumber, u32 StreamDescriptionIndex)
{
	GF_GenericVisualSampleEntryBox *entry;
	GF_GenericAudioSampleEntryBox *gena;
	GF_GenericSampleEntryBox *genm;
	GF_TrackBox *trak;
	GF_GenericSampleDescription *udesc;
	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !StreamDescriptionIndex || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable) return 0;

	entry = (GF_GenericVisualSampleEntryBox *)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, StreamDescriptionIndex-1);
	//no entry or MPEG entry:
	if (!entry || IsMP4Description(entry->type) ) return NULL;
	//if we handle the description return false
	switch (entry->type) {
	case GF_ISOM_SUBTYPE_3GP_AMR:
	case GF_ISOM_SUBTYPE_3GP_AMR_WB:
	case GF_ISOM_SUBTYPE_3GP_EVRC:
	case GF_ISOM_SUBTYPE_3GP_QCELP:
	case GF_ISOM_SUBTYPE_3GP_SMV:
	case GF_ISOM_SUBTYPE_3GP_H263:
		return NULL;
	case GF_ISOM_BOX_TYPE_GNRV:
		GF_SAFEALLOC(udesc, GF_GenericSampleDescription);
		if (!udesc) return NULL;
		if (entry->EntryType == GF_ISOM_BOX_TYPE_UUID) {
			memcpy(udesc->UUID, ((GF_UUIDBox*)entry)->uuid, sizeof(bin128));
		} else {
			udesc->codec_tag = entry->EntryType;
		}
		udesc->version = entry->version;
		udesc->revision = entry->revision;
		udesc->vendor_code = entry->vendor;
		udesc->temporal_quality = entry->temporal_quality;
		udesc->spatial_quality = entry->spatial_quality;
		udesc->width = entry->Width;
		udesc->height = entry->Height;
		udesc->h_res = entry->horiz_res;
		udesc->v_res = entry->vert_res;
		strcpy(udesc->compressor_name, entry->compressor_name);
		udesc->depth = entry->bit_depth;
		udesc->color_table_index = entry->color_table_index;
		if (entry->data_size) {
			udesc->extension_buf_size = entry->data_size;
			udesc->extension_buf = (char*)gf_malloc(sizeof(char) * entry->data_size);
			if (!udesc->extension_buf) {
				gf_free(udesc);
				return NULL;
			}
			memcpy(udesc->extension_buf, entry->data, entry->data_size);
		}
		return udesc;
	case GF_ISOM_BOX_TYPE_GNRA:
		gena = (GF_GenericAudioSampleEntryBox *)entry;
		GF_SAFEALLOC(udesc, GF_GenericSampleDescription);
		if (!udesc) return NULL;
		if (gena->EntryType == GF_ISOM_BOX_TYPE_UUID) {
			memcpy(udesc->UUID, ((GF_UUIDBox*)gena)->uuid, sizeof(bin128));
		} else {
			udesc->codec_tag = gena->EntryType;
		}
		udesc->version = gena->version;
		udesc->revision = gena->revision;
		udesc->vendor_code = gena->vendor;
		udesc->samplerate = gena->samplerate_hi;
		udesc->bits_per_sample = gena->bitspersample;
		udesc->nb_channels = gena->channel_count;
		if (gena->data_size) {
			udesc->extension_buf_size = gena->data_size;
			udesc->extension_buf = (char*)gf_malloc(sizeof(char) * gena->data_size);
			if (!udesc->extension_buf) {
				gf_free(udesc);
				return NULL;
			}
			memcpy(udesc->extension_buf, gena->data, gena->data_size);
		}
		return udesc;
	case GF_ISOM_BOX_TYPE_GNRM:
		genm = (GF_GenericSampleEntryBox *)entry;
		GF_SAFEALLOC(udesc, GF_GenericSampleDescription);
		if (!udesc) return NULL;
		if (genm->EntryType == GF_ISOM_BOX_TYPE_UUID) {
			memcpy(udesc->UUID, ((GF_UUIDBox*)genm)->uuid, sizeof(bin128));
		} else {
			udesc->codec_tag = genm->EntryType;
		}
		if (genm->data_size) {
			udesc->extension_buf_size = genm->data_size;
			udesc->extension_buf = (char*)gf_malloc(sizeof(char) * genm->data_size);
			if (!udesc->extension_buf) {
				gf_free(udesc);
				return NULL;
			}
			memcpy(udesc->extension_buf, genm->data, genm->data_size);
		}
		return udesc;
	}
	return NULL;
}