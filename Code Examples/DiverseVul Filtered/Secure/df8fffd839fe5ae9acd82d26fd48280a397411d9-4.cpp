GF_Err Track_SetStreamDescriptor(GF_TrackBox *trak, u32 StreamDescriptionIndex, u32 DataReferenceIndex, GF_ESD *esd, u32 *outStreamIndex)
{
	GF_Err e;
	GF_MPEGSampleEntryBox *entry;
	GF_MPEGVisualSampleEntryBox *entry_v;
	GF_MPEGAudioSampleEntryBox *entry_a;
	GF_TrackReferenceBox *tref;
	GF_TrackReferenceTypeBox *dpnd;
	u16 tmpRef;

	entry = NULL;
	tref = NULL;

	if (!trak || !esd || (!outStreamIndex && !DataReferenceIndex) ) return GF_BAD_PARAM;
	if (!Track_IsMPEG4Stream(trak->Media->handler->handlerType)) return GF_ISOM_INVALID_MEDIA;


	esd->ESID = 0;
	//set SL to predefined if no url
	if (esd->URLString == NULL) {
		if (!esd->slConfig) esd->slConfig = (GF_SLConfig*) gf_odf_desc_new(GF_ODF_SLC_TAG);
		esd->slConfig->predefined = SLPredef_MP4;
		esd->slConfig->durationFlag = 0;
		esd->slConfig->useTimestampsFlag = 1;
	}

	//get the REF box if needed
	if (esd->dependsOnESID || (esd->OCRESID  && (esd->OCRESID != trak->moov->mov->es_id_default_sync)) ) {
		if (!trak->References) {
			tref = (GF_TrackReferenceBox *) gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_TREF);
			if (!tref) return GF_OUT_OF_MEM;
			e = trak_on_child_box((GF_Box*)trak, (GF_Box *)tref, GF_FALSE);
			if (e) return e;
		}
		tref = trak->References;
	}

	//Update Stream dependencies
	e = Track_FindRef(trak, GF_ISOM_REF_DECODE, &dpnd);
	if (e) return e;

	if (!dpnd && esd->dependsOnESID) {
		e = Track_FindRef(trak, GF_ISOM_REF_BASE, &dpnd);
		if (e) return e;
	}

	if (!dpnd && esd->dependsOnESID) {
		dpnd = (GF_TrackReferenceTypeBox *) gf_isom_box_new_parent(&tref->child_boxes, GF_ISOM_BOX_TYPE_REFT);
		dpnd->reference_type = GF_ISOM_BOX_TYPE_DPND;
		e = reftype_AddRefTrack(dpnd, esd->dependsOnESID, NULL);
		if (e) return e;
	} else if (dpnd && !esd->dependsOnESID) {
		Track_RemoveRef(trak, GF_ISOM_BOX_TYPE_DPND);
	}
	esd->dependsOnESID = 0;

	//Update GF_Clock dependencies
	e = Track_FindRef(trak, GF_ISOM_REF_OCR, &dpnd);
	if (e) return e;
	if (!dpnd && esd->OCRESID && (esd->OCRESID != trak->moov->mov->es_id_default_sync)) {
		dpnd = (GF_TrackReferenceTypeBox *) gf_isom_box_new_parent(&tref->child_boxes, GF_ISOM_BOX_TYPE_REFT);
		if (!dpnd) return GF_OUT_OF_MEM;
		dpnd->reference_type = GF_ISOM_BOX_TYPE_SYNC;
		e = reftype_AddRefTrack(dpnd, esd->OCRESID, NULL);
		if (e) return e;
	} else if (dpnd && !esd->OCRESID) {
		Track_RemoveRef(trak, GF_ISOM_BOX_TYPE_SYNC);
	} else if (dpnd && esd->OCRESID) {
		if (dpnd->trackIDCount != 1) return GF_ISOM_INVALID_MEDIA;
		dpnd->trackIDs[0] = esd->OCRESID;
	}
	esd->OCRESID = 0;

	//brand new case: we have to change the IPI desc
	if (esd->ipiPtr) {
		e = Track_FindRef(trak, GF_ISOM_REF_IPI, &dpnd);
		if (e) return e;
		if (!dpnd) {
			tmpRef = 0;
			dpnd = (GF_TrackReferenceTypeBox *) gf_isom_box_new_parent(&tref->child_boxes, GF_ISOM_BOX_TYPE_REFT);
			if (!dpnd) return GF_OUT_OF_MEM;
			dpnd->reference_type = GF_ISOM_BOX_TYPE_IPIR;
			e = reftype_AddRefTrack(dpnd, esd->ipiPtr->IPI_ES_Id, &tmpRef);
			if (e) return e;
			//and replace the tag and value...
			esd->ipiPtr->IPI_ES_Id = tmpRef;
			esd->ipiPtr->tag = GF_ODF_ISOM_IPI_PTR_TAG;
		} else {
			//Watch out! ONLY ONE IPI dependency is allowed per stream
			if (dpnd->trackIDCount != 1) return GF_ISOM_INVALID_MEDIA;
			//if an existing one is there, what shall we do ???
			//donno, erase it
			dpnd->trackIDs[0] = esd->ipiPtr->IPI_ES_Id;
			//and replace the tag and value...
			esd->ipiPtr->IPI_ES_Id = 1;
			esd->ipiPtr->tag = GF_ODF_ISOM_IPI_PTR_TAG;
		}
	}

	/*don't store the lang desc in ESD, use the media header language info*/
	if (esd->langDesc) {
		trak->Media->mediaHeader->packedLanguage[0] = (esd->langDesc->langCode>>16)&0xFF;
		trak->Media->mediaHeader->packedLanguage[1] = (esd->langDesc->langCode>>8)&0xFF;
		trak->Media->mediaHeader->packedLanguage[2] = (esd->langDesc->langCode)&0xFF;
		gf_odf_desc_del((GF_Descriptor *)esd->langDesc);
		esd->langDesc = NULL;
	}

	//we have a streamDescriptionIndex, use it
	if (StreamDescriptionIndex) {
		u32 entry_type;
		entry = (GF_MPEGSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, StreamDescriptionIndex - 1);
		if (!entry) return GF_ISOM_INVALID_FILE;

		entry_type = entry->type;
		GF_ProtectionSchemeInfoBox *sinf = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_SINF);
		if (sinf && sinf->original_format) entry_type = sinf->original_format->data_format;
		
		switch (entry_type) {
		case GF_ISOM_BOX_TYPE_MP4S:
			//OK, delete the previous ESD
			gf_odf_desc_del((GF_Descriptor *) entry->esd->desc);
			entry->esd->desc = esd;
			break;
		case GF_ISOM_BOX_TYPE_MP4V:
			entry_v = (GF_MPEGVisualSampleEntryBox*) entry;
			//OK, delete the previous ESD
			gf_odf_desc_del((GF_Descriptor *) entry_v->esd->desc);
			entry_v->esd->desc = esd;
			break;
		case GF_ISOM_BOX_TYPE_MP4A:
			entry_a = (GF_MPEGAudioSampleEntryBox*) entry;
            if (entry_a->esd) { // some non-conformant files may not have an ESD ...
                //OK, delete the previous ESD
                gf_odf_desc_del((GF_Descriptor *) entry_a->esd->desc);
                entry_a->esd->desc = esd;
            } else {
				// can't return OK here otherwise we can't know if esd hasn't been used
				// and need to be freed
				return GF_ISOM_INVALID_MEDIA;
			}
			break;
		case GF_ISOM_BOX_TYPE_AVC1:
		case GF_ISOM_BOX_TYPE_AVC2:
		case GF_ISOM_BOX_TYPE_AVC3:
		case GF_ISOM_BOX_TYPE_AVC4:
		case GF_ISOM_BOX_TYPE_SVC1:
		case GF_ISOM_BOX_TYPE_MVC1:
		case GF_ISOM_BOX_TYPE_HVC1:
		case GF_ISOM_BOX_TYPE_HEV1:
		case GF_ISOM_BOX_TYPE_HVC2:
		case GF_ISOM_BOX_TYPE_HEV2:
		case GF_ISOM_BOX_TYPE_LHE1:
		case GF_ISOM_BOX_TYPE_LHV1:
		case GF_ISOM_BOX_TYPE_HVT1:
		case GF_ISOM_BOX_TYPE_VVC1:
		case GF_ISOM_BOX_TYPE_VVI1:
			e = AVC_HEVC_UpdateESD((GF_MPEGVisualSampleEntryBox*)entry, esd);
			if (e) return e;
			break;
		case GF_ISOM_BOX_TYPE_LSR1:
			e = LSR_UpdateESD((GF_LASeRSampleEntryBox*)entry, esd);
			if (e) return e;
			break;
		case GF_ISOM_BOX_TYPE_AV01:
		case GF_ISOM_BOX_TYPE_AV1C:
		case GF_ISOM_BOX_TYPE_OPUS:
		case GF_ISOM_BOX_TYPE_DOPS:
		case GF_ISOM_BOX_TYPE_STXT:
		case GF_ISOM_BOX_TYPE_WVTT:
		case GF_ISOM_BOX_TYPE_STPP:
			if (esd) gf_odf_desc_del((GF_Descriptor *) esd);
			break;

		default:
			//silently fail, not an MPEG-4 esd
			gf_odf_desc_del((GF_Descriptor *) esd);
			return GF_OK;
		}
	} else {
		//need to check we're not in URL mode where only ONE description is allowed...
		StreamDescriptionIndex = gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes);
		if (StreamDescriptionIndex) {
			GF_ESD *old_esd=NULL;
			entry = (GF_MPEGSampleEntryBox*)gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, StreamDescriptionIndex - 1);
			if (!entry) return GF_ISOM_INVALID_FILE;
			//get ESD (only if present, do not emulate)
			Media_GetESD(trak->Media, StreamDescriptionIndex, &old_esd, GF_TRUE);
			if (old_esd && old_esd->URLString) return GF_BAD_PARAM;
		}

		//OK, check the handler and create the entry
		switch (trak->Media->handler->handlerType) {
        case GF_ISOM_MEDIA_AUXV:
        case GF_ISOM_MEDIA_PICT:
		case GF_ISOM_MEDIA_VISUAL:
			if ((esd->decoderConfig->objectTypeIndication==GF_CODECID_AVC) || (esd->decoderConfig->objectTypeIndication==GF_CODECID_SVC) || (esd->decoderConfig->objectTypeIndication==GF_CODECID_MVC)) {
				entry_v = (GF_MPEGVisualSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_AVC1);
				if (!entry_v) return GF_OUT_OF_MEM;
				e = AVC_HEVC_UpdateESD((GF_MPEGVisualSampleEntryBox*)entry_v, esd);
				if (e) return  e;
			} else if (esd->decoderConfig->objectTypeIndication==GF_CODECID_HEVC) {
				entry_v = (GF_MPEGVisualSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_HVC1);
				if (!entry_v) return GF_OUT_OF_MEM;
				e = AVC_HEVC_UpdateESD((GF_MPEGVisualSampleEntryBox*)entry_v, esd);
				if (e) return  e;
			} else if (esd->decoderConfig->objectTypeIndication==GF_CODECID_VVC) {
				entry_v = (GF_MPEGVisualSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_VVC1);
				if (!entry_v) return GF_OUT_OF_MEM;
				e = AVC_HEVC_UpdateESD((GF_MPEGVisualSampleEntryBox*)entry_v, esd);
				if (e) return  e;
			} else {
				entry_v = (GF_MPEGVisualSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_MP4V);
				if (!entry_v) return GF_OUT_OF_MEM;
				entry_v->esd = (GF_ESDBox *) gf_isom_box_new_parent(&entry_v->child_boxes, GF_ISOM_BOX_TYPE_ESDS);
				if (!entry_v->esd) return GF_OUT_OF_MEM;
				entry_v->esd->desc = esd;
			}

			//type cast possible now
			entry = (GF_MPEGSampleEntryBox*) entry_v;
			break;
		case GF_ISOM_MEDIA_AUDIO:
			if (esd->decoderConfig->objectTypeIndication == GF_CODECID_OPUS) {
				GF_MPEGAudioSampleEntryBox *opus = (GF_MPEGAudioSampleEntryBox *)gf_isom_box_new(GF_ISOM_BOX_TYPE_OPUS);
				if (!opus) return GF_OUT_OF_MEM;
				opus->cfg_opus = (GF_OpusSpecificBox *)gf_isom_box_new_parent(&opus->child_boxes, GF_ISOM_BOX_TYPE_DOPS);
				if (!opus->cfg_opus) return GF_OUT_OF_MEM;
				entry = (GF_MPEGSampleEntryBox*)opus;
				gf_odf_desc_del((GF_Descriptor *) esd);
			} else if (esd->decoderConfig->objectTypeIndication == GF_CODECID_AC3) {
				GF_MPEGAudioSampleEntryBox *ac3 = (GF_MPEGAudioSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_AC3);
				if (!ac3) return GF_OUT_OF_MEM;
				ac3->cfg_ac3 = (GF_AC3ConfigBox *) gf_isom_box_new_parent(&ac3->child_boxes, GF_ISOM_BOX_TYPE_DAC3);
				if (!ac3->cfg_ac3) return GF_OUT_OF_MEM;
				entry = (GF_MPEGSampleEntryBox*) ac3;
				gf_odf_desc_del((GF_Descriptor *) esd);
			} else if (esd->decoderConfig->objectTypeIndication==GF_CODECID_EAC3) {
				GF_MPEGAudioSampleEntryBox *eac3 = (GF_MPEGAudioSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_EC3);
				if (!eac3) return GF_OUT_OF_MEM;
				eac3->cfg_ac3 = (GF_AC3ConfigBox *) gf_isom_box_new_parent(&eac3->child_boxes, GF_ISOM_BOX_TYPE_DEC3);
				if (!eac3->cfg_ac3) return GF_OUT_OF_MEM;
				entry = (GF_MPEGSampleEntryBox*) eac3;
				gf_odf_desc_del((GF_Descriptor *) esd);
			} else {
				entry_a = (GF_MPEGAudioSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_MP4A);
				if (!entry_a) return GF_OUT_OF_MEM;
				entry_a->samplerate_hi = trak->Media->mediaHeader->timeScale;
				entry_a->esd = (GF_ESDBox *) gf_isom_box_new_parent(&entry_a->child_boxes, GF_ISOM_BOX_TYPE_ESDS);
				if (!entry_a->esd) return GF_OUT_OF_MEM;
				entry_a->esd->desc = esd;
				//type cast possible now
				entry = (GF_MPEGSampleEntryBox*) entry_a;
			}
			break;
		default:
			if ((esd->decoderConfig->streamType==0x03) && (esd->decoderConfig->objectTypeIndication==0x09)) {
				entry = (GF_MPEGSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_LSR1);
				if (!entry) return GF_OUT_OF_MEM;
				e = LSR_UpdateESD((GF_LASeRSampleEntryBox*)entry, esd);
				if (e) return  e;
			} else {
				entry = (GF_MPEGSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_MP4S);
				entry->esd = (GF_ESDBox *) gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_ESDS);
				if (!entry->esd) return GF_OUT_OF_MEM;
				entry->esd->desc = esd;
			}
			break;
		}
		entry->dataReferenceIndex = DataReferenceIndex;

		if (!trak->Media->information->sampleTable->SampleDescription->child_boxes)
			trak->Media->information->sampleTable->SampleDescription->child_boxes = gf_list_new();
		gf_list_add(trak->Media->information->sampleTable->SampleDescription->child_boxes, entry);
		
		e = stsd_on_child_box((GF_Box*)trak->Media->information->sampleTable->SampleDescription, (GF_Box *) entry, GF_FALSE);
		if (e) return e;
		if(outStreamIndex) *outStreamIndex = gf_list_count(trak->Media->information->sampleTable->SampleDescription->child_boxes);
	}
	return GF_OK;
}