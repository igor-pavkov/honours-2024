static GF_Err isom_set_protected_entry(GF_ISOFile *the_file, u32 trackNumber, u32 desc_index, u8 version, u32 flags,
        u32 scheme_type, u32 scheme_version, char *scheme_uri, Bool is_isma, GF_ProtectionSchemeInfoBox **out_sinf)
{
	u32 original_format;
	GF_Err e;
	GF_SampleEntryBox *sea;
	GF_ProtectionSchemeInfoBox *sinf;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	e = Media_GetSampleDesc(trak->Media, desc_index, &sea, NULL);
	if (e) return e;

	original_format = sea->type;
	if (original_format==GF_ISOM_BOX_TYPE_GNRA) {
		original_format = ((GF_GenericAudioSampleEntryBox*)sea)->EntryType;
	} else if (original_format==GF_ISOM_BOX_TYPE_GNRV) {
		original_format = ((GF_GenericVisualSampleEntryBox*)sea)->EntryType;
	} else if (original_format==GF_ISOM_BOX_TYPE_GNRM) {
		original_format = ((GF_GenericSampleEntryBox*)sea)->EntryType;
	}

	/* Replacing the Media Type */
	switch (sea->type) {
	case GF_ISOM_BOX_TYPE_MP4A:
	case GF_ISOM_BOX_TYPE_DAMR:
	case GF_ISOM_BOX_TYPE_DEVC:
	case GF_ISOM_BOX_TYPE_DQCP:
	case GF_ISOM_BOX_TYPE_DSMV:
	case GF_ISOM_BOX_TYPE_AC3:
	case GF_ISOM_BOX_TYPE_EC3:
		sea->type = GF_ISOM_BOX_TYPE_ENCA;
		break;
	case GF_ISOM_BOX_TYPE_MP4V:
	case GF_ISOM_BOX_TYPE_D263:
		sea->type = GF_ISOM_BOX_TYPE_ENCV;
		break;
	/*special case for AVC1*/
	case GF_ISOM_BOX_TYPE_AVC1:
	case GF_ISOM_BOX_TYPE_AVC2:
	case GF_ISOM_BOX_TYPE_AVC3:
	case GF_ISOM_BOX_TYPE_AVC4:
	case GF_ISOM_BOX_TYPE_SVC1:
	case GF_ISOM_BOX_TYPE_MVC1:
		if (is_isma)
			original_format = GF_ISOM_BOX_TYPE_264B;
		sea->type = GF_ISOM_BOX_TYPE_ENCV;
		break;
	case GF_ISOM_BOX_TYPE_HVC1:
	case GF_ISOM_BOX_TYPE_HEV1:
	case GF_ISOM_BOX_TYPE_HVC2:
	case GF_ISOM_BOX_TYPE_HEV2:
	case GF_ISOM_BOX_TYPE_LHE1:
	case GF_ISOM_BOX_TYPE_LHV1:
	case GF_ISOM_BOX_TYPE_HVT1:
		if (is_isma)
			original_format = GF_ISOM_BOX_TYPE_265B;
		sea->type = GF_ISOM_BOX_TYPE_ENCV;
		break;
	case GF_ISOM_BOX_TYPE_VVC1:
	case GF_ISOM_BOX_TYPE_VVI1:
		sea->type = GF_ISOM_BOX_TYPE_ENCV;
		break;
	case GF_ISOM_BOX_TYPE_VP09:
		sea->type = GF_ISOM_BOX_TYPE_ENCV;
		break;
	case GF_ISOM_BOX_TYPE_AV01:
		sea->type = GF_ISOM_BOX_TYPE_ENCV;
		break;
	case GF_ISOM_BOX_TYPE_MP4S:
	case GF_ISOM_BOX_TYPE_LSR1:
		sea->type = GF_ISOM_BOX_TYPE_ENCS;
		break;
	case GF_ISOM_BOX_TYPE_STXT:
	case GF_ISOM_BOX_TYPE_WVTT:
	case GF_ISOM_BOX_TYPE_STPP:
		sea->type = GF_ISOM_BOX_TYPE_ENCT;
		break;
	case GF_ISOM_BOX_TYPE_ENCA:
	case GF_ISOM_BOX_TYPE_ENCV:
	case GF_ISOM_BOX_TYPE_ENCT:
	case GF_ISOM_BOX_TYPE_ENCM:
	case GF_ISOM_BOX_TYPE_ENCF:
	case GF_ISOM_BOX_TYPE_ENCS:
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] cannot set protection entry: file is already encrypted.\n"));
		return GF_BAD_PARAM;
	default:
		if (sea->internal_type == GF_ISOM_SAMPLE_ENTRY_AUDIO) {
			sea->type = GF_ISOM_BOX_TYPE_ENCA;
		} else if (sea->internal_type == GF_ISOM_SAMPLE_ENTRY_VIDEO) {
			sea->type = GF_ISOM_BOX_TYPE_ENCV;
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] unsupported entry type %s for CENC.\n", gf_4cc_to_str(sea->type) ));
			return GF_BAD_PARAM;
		}
	}

	sinf = (GF_ProtectionSchemeInfoBox *)gf_isom_box_new_parent(&sea->child_boxes, GF_ISOM_BOX_TYPE_SINF);
	if (!sinf) return GF_OUT_OF_MEM;

	sinf->scheme_type = (GF_SchemeTypeBox *)gf_isom_box_new_parent(&sinf->child_boxes, GF_ISOM_BOX_TYPE_SCHM);
	if (!sinf->scheme_type) return GF_OUT_OF_MEM;

	sinf->scheme_type->version = version;
	sinf->scheme_type->flags = flags;
	sinf->scheme_type->scheme_type = scheme_type;
	sinf->scheme_type->scheme_version = scheme_version;
	if (scheme_uri && (sinf->scheme_type->flags == 1)) {
		sinf->scheme_type->URI = (char *)gf_malloc(sizeof(char)*strlen(scheme_uri));
		if (!sinf->scheme_type->URI) return GF_OUT_OF_MEM;
		memmove(sinf->scheme_type->URI, scheme_uri, strlen(scheme_uri));
	}

	sinf->original_format = (GF_OriginalFormatBox *)gf_isom_box_new_parent(&sinf->child_boxes, GF_ISOM_BOX_TYPE_FRMA);
	if (!sinf->original_format) return GF_OUT_OF_MEM;
	sinf->original_format->data_format = original_format;

	//common to isma, cenc and oma
	sinf->info = (GF_SchemeInformationBox *)gf_isom_box_new_parent(&sinf->child_boxes, GF_ISOM_BOX_TYPE_SCHI);

	*out_sinf = sinf;
	return GF_OK;
}