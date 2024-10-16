static GF_Err isom_create_init_from_mem(const char *fileName, GF_ISOFile *file)
{
	u32 sample_rate=0;
	u32 nb_channels=0;
	u32 bps=0;
	//u32 atag=0;
	u32 nal_len=4;
	u32 width = 0;
	u32 height = 0;
	u32 timescale = 10000000;
	u64 tfdt = 0;
	char sz4cc[5];
	char CodecParams[2048];
	u32 CodecParamLen=0;
	char *sep, *val;
	GF_TrackBox *trak;
	GF_TrackExtendsBox *trex;
	GF_SampleTableBox *stbl;

	sz4cc[0] = 0;

	val = (char*) ( fileName + strlen("isobmff://") );
	while (1)  {
		sep = strchr(val, ' ');
		if (sep) sep[0] = 0;

		if (!strncmp(val, "4cc=", 4)) strcpy(sz4cc, val+4);
		else if (!strncmp(val, "init=", 5)) {
			char szH[3], *data = val+5;
			u32 i, len = (u32) strlen(data);
			for (i=0; i<len; i+=2) {
				u32 v;
				//init is hex-encoded so 2 input bytes for one output char
				szH[0] = data[i];
				szH[1] = data[i+1];
				szH[2] = 0;
				sscanf(szH, "%X", &v);
				CodecParams[CodecParamLen] = (char) v;
				CodecParamLen++;
			}
		}
		else if (!strncmp(val, "nal=", 4)) nal_len = atoi(val+4);
		else if (!strncmp(val, "bps=", 4)) bps = atoi(val+4);
		//else if (!strncmp(val, "atag=", 5)) atag = atoi(val+5);
		else if (!strncmp(val, "ch=", 3)) nb_channels = atoi(val+3);
		else if (!strncmp(val, "srate=", 6)) sample_rate = atoi(val+6);
		else if (!strncmp(val, "w=", 2)) width = atoi(val+2);
		else if (!strncmp(val, "h=", 2)) height = atoi(val+2);
		else if (!strncmp(val, "scale=", 6)) timescale = atoi(val+6);
		else if (!strncmp(val, "tfdt=", 5)) {
			sscanf(val+5, LLX, &tfdt);
		}
		if (!sep) break;
		sep[0] = ' ';
		val = sep+1;
	}
	if (!stricmp(sz4cc, "H264") || !stricmp(sz4cc, "AVC1")) {
	}
	else if (!stricmp(sz4cc, "AACL")) {
	}
	else {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Cannot convert smooth media type %s to ISO init segment\n", sz4cc));
		return GF_NOT_SUPPORTED;
	}

	file->moov = (GF_MovieBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_MOOV);
	if (!file->moov) return GF_OUT_OF_MEM;
	gf_list_add(file->TopBoxes, file->moov);
	file->moov->mov = file;
	file->is_smooth = GF_TRUE;
	file->moov->mvhd = (GF_MovieHeaderBox *) gf_isom_box_new_parent(&file->moov->child_boxes, GF_ISOM_BOX_TYPE_MVHD);
	if (!file->moov->mvhd) return GF_OUT_OF_MEM;
	file->moov->mvhd->timeScale = timescale;
	file->moov->mvex = (GF_MovieExtendsBox *) gf_isom_box_new_parent(&file->moov->child_boxes, GF_ISOM_BOX_TYPE_MVEX);
	if (!file->moov->mvex) return GF_OUT_OF_MEM;
	trex = (GF_TrackExtendsBox *) gf_isom_box_new_parent(&file->moov->mvex->child_boxes, GF_ISOM_BOX_TYPE_TREX);
	if (!trex) return GF_OUT_OF_MEM;

	trex->def_sample_desc_index = 1;
	trex->trackID = 1;
	gf_list_add(file->moov->mvex->TrackExList, trex);

	trak = (GF_TrackBox *) gf_isom_box_new_parent(&file->moov->child_boxes, GF_ISOM_BOX_TYPE_TRAK);
	if (!trak) return GF_OUT_OF_MEM;
	trak->moov = file->moov;
	gf_list_add(file->moov->trackList, trak);

	trak->Header = (GF_TrackHeaderBox *) gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_TKHD);
	if (!trak->Header) return GF_OUT_OF_MEM;
	trak->Header->trackID = 1;
	trak->Header->flags |= 1;
	trak->Header->width = width;
	trak->Header->height = height;

	trak->Media = (GF_MediaBox *) gf_isom_box_new_parent(&trak->child_boxes, GF_ISOM_BOX_TYPE_MDIA);
	if (!trak->Media) return GF_OUT_OF_MEM;
	trak->Media->mediaTrack = trak;
	trak->Media->mediaHeader = (GF_MediaHeaderBox *) gf_isom_box_new_parent(&trak->Media->child_boxes, GF_ISOM_BOX_TYPE_MDHD);
	if (!trak->Media->mediaHeader) return GF_OUT_OF_MEM;
	trak->Media->mediaHeader->timeScale = timescale;

	trak->Media->handler = (GF_HandlerBox *) gf_isom_box_new_parent(&trak->Media->child_boxes,GF_ISOM_BOX_TYPE_HDLR);
	if (!trak->Media->handler) return GF_OUT_OF_MEM;
    //we assume by default vide for handler type (only used for smooth streaming)
	trak->Media->handler->handlerType = width ? GF_ISOM_MEDIA_VISUAL : GF_ISOM_MEDIA_AUDIO;

	trak->Media->information = (GF_MediaInformationBox *) gf_isom_box_new_parent(&trak->Media->child_boxes, GF_ISOM_BOX_TYPE_MINF);
	if (!trak->Media->information) return GF_OUT_OF_MEM;
	trak->Media->information->sampleTable = (GF_SampleTableBox *) gf_isom_box_new_parent(&trak->Media->information->child_boxes, GF_ISOM_BOX_TYPE_STBL);
	if (!trak->Media->information->sampleTable) return GF_OUT_OF_MEM;

	stbl = trak->Media->information->sampleTable;
	stbl->SampleSize = (GF_SampleSizeBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSZ);
	if (!stbl->SampleSize) return GF_OUT_OF_MEM;
	stbl->TimeToSample = (GF_TimeToSampleBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STTS);
	if (!stbl->TimeToSample) return GF_OUT_OF_MEM;
	stbl->ChunkOffset = (GF_Box *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STCO);
	if (!stbl->ChunkOffset) return GF_OUT_OF_MEM;
	stbl->SampleToChunk = (GF_SampleToChunkBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSC);
	if (!stbl->SampleToChunk) return GF_OUT_OF_MEM;
	stbl->SyncSample = (GF_SyncSampleBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSS);
	if (!stbl->SyncSample) return GF_OUT_OF_MEM;
	stbl->SampleDescription = (GF_SampleDescriptionBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSD);
	if (!stbl->SampleDescription) return GF_OUT_OF_MEM;

	trak->dts_at_seg_start = tfdt;
	trak->dts_at_next_frag_start = tfdt;


	if (!stricmp(sz4cc, "H264") || !stricmp(sz4cc, "AVC1")) {
#ifndef GPAC_DISABLE_AV_PARSERS
		u32 pos = 0;
		u32 end, sc_size=0;
#endif
		GF_MPEGVisualSampleEntryBox *avc =  (GF_MPEGVisualSampleEntryBox *) gf_isom_box_new_parent(&stbl->SampleDescription->child_boxes, GF_ISOM_BOX_TYPE_AVC1);
		if (!avc) return GF_OUT_OF_MEM;
		avc->avc_config =  (GF_AVCConfigurationBox *) gf_isom_box_new_parent(&avc->child_boxes, GF_ISOM_BOX_TYPE_AVCC);
		if (!avc->avc_config) return GF_OUT_OF_MEM;

		avc->Width = width;
		avc->Height = height;

		avc->avc_config->config = gf_odf_avc_cfg_new();
		avc->avc_config->config->nal_unit_size = nal_len;
		avc->avc_config->config->configurationVersion = 1;

#ifndef GPAC_DISABLE_AV_PARSERS
		//locate pps and sps
		gf_media_nalu_next_start_code((u8 *) CodecParams, CodecParamLen, &sc_size);
		pos += sc_size;
		while (pos<CodecParamLen) {
			GF_NALUFFParam *slc;
			u8 nal_type;
			char *nal = &CodecParams[pos];
			end = gf_media_nalu_next_start_code(nal, CodecParamLen-pos, &sc_size);
			if (!end) end = CodecParamLen;

			GF_SAFEALLOC(slc, GF_NALUFFParam);
			if (!slc) break;
			slc->size = end;
			slc->data = gf_malloc(sizeof(char)*slc->size);
			if (!slc->data) return GF_OUT_OF_MEM;
			memcpy(slc->data, nal, sizeof(char)*slc->size);

			nal_type = nal[0] & 0x1F;
			if (nal_type == GF_AVC_NALU_SEQ_PARAM) {
/*				AVCState avcc;
				u32 idx = gf_avc_read_sps(slc->data, slc->size, &avcc, 0, NULL);
				avc->avc_config->config->profile_compatibility = avcc.sps[idx].prof_compat;
				avc->avc_config->config->AVCProfileIndication = avcc.sps[idx].profile_idc;
				avc->avc_config->config->AVCLevelIndication = avcc.sps[idx].level_idc;
				avc->avc_config->config->chroma_format = avcc.sps[idx].chroma_format;
				avc->avc_config->config->luma_bit_depth = 8 + avcc.sps[idx].luma_bit_depth_m8;
				avc->avc_config->config->chroma_bit_depth = 8 + avcc.sps[idx].chroma_bit_depth_m8;
*/

				gf_list_add(avc->avc_config->config->sequenceParameterSets, slc);
			} else {
				gf_list_add(avc->avc_config->config->pictureParameterSets, slc);
			}
			pos += slc->size + sc_size;
		}
#endif

		AVC_RewriteESDescriptor(avc);
	}
	else if (!stricmp(sz4cc, "AACL")) {
#ifndef GPAC_DISABLE_AV_PARSERS
		GF_M4ADecSpecInfo aacinfo;
#endif

		GF_MPEGAudioSampleEntryBox *aac =  (GF_MPEGAudioSampleEntryBox *) gf_isom_box_new_parent(&stbl->SampleDescription->child_boxes, GF_ISOM_BOX_TYPE_MP4A);
		if (!aac) return GF_OUT_OF_MEM;
		aac->esd = (GF_ESDBox *) gf_isom_box_new_parent(&aac->child_boxes, GF_ISOM_BOX_TYPE_ESDS);
		if (!aac->esd) return GF_OUT_OF_MEM;
		aac->esd->desc = gf_odf_desc_esd_new(2);
		if (!aac->esd->desc) return GF_OUT_OF_MEM;
#ifndef GPAC_DISABLE_AV_PARSERS
		memset(&aacinfo, 0, sizeof(GF_M4ADecSpecInfo));
		aacinfo.nb_chan = nb_channels;
		aacinfo.base_object_type = GF_M4A_AAC_LC;
		aacinfo.base_sr = sample_rate;
		gf_m4a_write_config(&aacinfo, &aac->esd->desc->decoderConfig->decoderSpecificInfo->data, &aac->esd->desc->decoderConfig->decoderSpecificInfo->dataLength);
#endif
		aac->esd->desc->decoderConfig->streamType = GF_STREAM_AUDIO;
		aac->esd->desc->decoderConfig->objectTypeIndication = GF_CODECID_AAC_MPEG4;
		aac->bitspersample = bps;
		aac->samplerate_hi = sample_rate;
		aac->channel_count = nb_channels;
	}

	return GF_OK;
}