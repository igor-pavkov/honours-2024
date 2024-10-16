Bool gf_isom_cenc_has_saiz_saio_full(GF_SampleTableBox *stbl, void *_traf, u32 scheme_type)
{
	u32 i, c1, c2;
	GF_List *sai_sizes, *sai_offsets;
	u32 sinf_fmt = 0;
	Bool has_saiz, has_saio;
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	GF_TrackFragmentBox *traf=(GF_TrackFragmentBox *)_traf;
#endif
	has_saiz = has_saio = GF_FALSE;

	if (stbl) {
		sai_sizes = stbl->sai_sizes;
		sai_offsets = stbl->sai_offsets;
	}
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
	else if (_traf) {
		sai_sizes = traf->sai_sizes;
		sai_offsets = traf->sai_offsets;
	}
#endif
	else
		return GF_FALSE;

	c1 = gf_list_count(sai_sizes);
	c2 = gf_list_count(sai_offsets);
	for (i = 0; i < c1; i++) {
		GF_SampleAuxiliaryInfoSizeBox *saiz = (GF_SampleAuxiliaryInfoSizeBox *)gf_list_get(sai_sizes, i);
		u32 saiz_aux_info_type = saiz->aux_info_type;
		if (!saiz_aux_info_type) saiz_aux_info_type = scheme_type;


		if (!saiz_aux_info_type) {
			GF_SampleEntryBox *entry = NULL;
			GF_ProtectionSchemeInfoBox *sinf = NULL;
			if (stbl) {
				entry = gf_list_get(stbl->SampleDescription->child_boxes, 0);
			} else {
				entry = gf_list_get(traf->trex->track->Media->information->sampleTable->SampleDescription->child_boxes, 0);
			}

			if (entry)
				sinf = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_SINF);

			if (sinf && sinf->scheme_type) {
				saiz_aux_info_type = sinf_fmt = sinf->scheme_type->scheme_type;
			}
		}
		if (!saiz_aux_info_type && (c1==1) && (c2==1)) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] saiz box without flags nor aux info type and no default scheme, ignoring\n"));
			continue;
		}

		switch (saiz_aux_info_type) {
		case GF_ISOM_CENC_SCHEME:
		case GF_ISOM_CBC_SCHEME:
		case GF_ISOM_CENS_SCHEME:
		case GF_ISOM_CBCS_SCHEME:
		case GF_ISOM_PIFF_SCHEME:
			has_saiz = GF_TRUE;
			break;
		}
	}

	for (i = 0; i < c2; i++) {
		GF_SampleAuxiliaryInfoOffsetBox *saio = (GF_SampleAuxiliaryInfoOffsetBox *)gf_list_get(sai_offsets, i);
		u32 saio_aux_info_type = saio->aux_info_type;
		if (!saio_aux_info_type) saio_aux_info_type = scheme_type;
		if (!saio_aux_info_type) saio_aux_info_type = sinf_fmt;

		if (!saio_aux_info_type) {
			GF_SampleEntryBox *entry = NULL;
			GF_ProtectionSchemeInfoBox *sinf = NULL;
			if (stbl) {
				entry = gf_list_get(stbl->SampleDescription->child_boxes, 0);
			} else {
				entry = gf_list_get(traf->trex->track->Media->information->sampleTable->SampleDescription->child_boxes, 0);
			}
			if (entry)
				sinf = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_SINF);

			if (sinf && sinf->scheme_type) {
				saio_aux_info_type = sinf_fmt = sinf->scheme_type->scheme_type;
			}
		}
		if (!saio_aux_info_type && (c1==1) && (c2==1)) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] saio box without flags nor aux info type and no default scheme, ignoring\n"));
			continue;
		}
		//special case for query on a file that has just been imported but not yet written: saio offset is NULL, we must use senc
		if (saio->entry_count && !saio->offsets)
			continue;
		switch (saio_aux_info_type) {
		case GF_ISOM_CENC_SCHEME:
		case GF_ISOM_CBC_SCHEME:
		case GF_ISOM_CENS_SCHEME:
		case GF_ISOM_CBCS_SCHEME:
		case GF_ISOM_PIFF_SCHEME:
			has_saio = GF_TRUE;
			break;
		}
	}
	return (has_saiz && has_saio);
}