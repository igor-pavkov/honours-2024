static GF_Err gf_export_isom_copy_track(GF_MediaExporter *dumper, GF_ISOFile *infile, u32 inTrackNum, GF_ISOFile *outfile, Bool ResetDependencies, Bool AddToIOD)
{
	GF_ESD *esd;
	GF_InitialObjectDescriptor *iod;
	GF_ISOTrackID TrackID;
	u32 newTk, descIndex, i, ts, rate, pos, di, count, msubtype;
	u64 dur;
	GF_ISOSample *samp;

	if (!inTrackNum) {
		if (gf_isom_get_track_count(infile) != 1) return gf_export_message(dumper, GF_BAD_PARAM, "Please specify trackID to export");
		inTrackNum = 1;
	}
	//check the ID is available
	TrackID = gf_isom_get_track_id(infile, inTrackNum);
	newTk = gf_isom_get_track_by_id(outfile, TrackID);
	if (newTk) TrackID = 0;

	//get the ESD and remove dependencies
	esd = NULL;
	msubtype = gf_isom_get_media_subtype(infile, inTrackNum, 1);

	if (msubtype == GF_ISOM_SUBTYPE_MPEG4) {
		esd = gf_isom_get_esd(infile, inTrackNum, 1);
		if (esd && ResetDependencies) {
			esd->dependsOnESID = 0;
			esd->OCRESID = 0;
		}
	}

	newTk = gf_isom_new_track(outfile, TrackID, gf_isom_get_media_type(infile, inTrackNum), gf_isom_get_media_timescale(infile, inTrackNum));
	gf_isom_set_track_enabled(outfile, newTk, GF_TRUE);

	if (gf_isom_has_keep_utc_times(infile)) {
		u64 cdate, mdate;
		gf_isom_get_track_creation_time(infile, inTrackNum, &cdate, &mdate);
		gf_isom_set_track_creation_time(outfile, newTk, cdate, mdate);
	}

	if (esd) {
		gf_isom_new_mpeg4_description(outfile, newTk, esd, NULL, NULL, &descIndex);
		if ((esd->decoderConfig->streamType == GF_STREAM_VISUAL) || (esd->decoderConfig->streamType == GF_STREAM_SCENE)) {
			u32 w, h;
			gf_isom_get_visual_info(infile, inTrackNum, 1, &w, &h);
#ifndef GPAC_DISABLE_AV_PARSERS
			/*this is because so many files have reserved values of 320x240 from v1 ... */
			if (esd->decoderConfig->objectTypeIndication == GF_CODECID_MPEG4_PART2) {
				GF_M4VDecSpecInfo dsi;
				gf_m4v_get_config(esd->decoderConfig->decoderSpecificInfo->data, esd->decoderConfig->decoderSpecificInfo->dataLength, &dsi);
				w = dsi.width;
				h = dsi.height;
			}
#endif
			gf_isom_set_visual_info(outfile, newTk, 1, w, h);
		}
		else if ((esd->decoderConfig->streamType == GF_STREAM_TEXT) && (esd->decoderConfig->objectTypeIndication == GF_CODECID_SUBPIC)) {
			u32 w, h;
			s32 trans_x, trans_y;
			s16 layer;
			gf_isom_get_track_layout_info(infile, inTrackNum, &w, &h, &trans_x, &trans_y, &layer);
			gf_isom_set_track_layout_info(outfile, newTk, w << 16, h << 16, trans_x, trans_y, layer);
		}
		esd->decoderConfig->avgBitrate = 0;
		esd->decoderConfig->maxBitrate = 0;
	} else {
		gf_isom_clone_sample_description(outfile, newTk, infile, inTrackNum, 1, NULL, NULL, &descIndex);
	}

	pos = 0;
	rate = 0;
	ts = gf_isom_get_media_timescale(infile, inTrackNum);
	count = gf_isom_get_sample_count(infile, inTrackNum);
	for (i=0; i<count; i++) {
		samp = gf_isom_get_sample(infile, inTrackNum, i+1, &di);
		gf_isom_add_sample(outfile, newTk, descIndex, samp);
		if (esd) {
			rate += samp->dataLength;
			esd->decoderConfig->avgBitrate += samp->dataLength;
			if (esd->decoderConfig->bufferSizeDB<samp->dataLength) esd->decoderConfig->bufferSizeDB = samp->dataLength;
			if (samp->DTS - pos > ts) {
				if (esd->decoderConfig->maxBitrate<rate) esd->decoderConfig->maxBitrate = rate;
				rate = 0;
				pos = 0;
			}
		}
		gf_isom_sample_del(&samp);
		gf_set_progress("ISO File Export", i, count);
	}
	gf_set_progress("ISO File Export", count, count);

	if (msubtype == GF_ISOM_SUBTYPE_MPEG4_CRYP) {
		esd = gf_isom_get_esd(infile, inTrackNum, 1);
	} else if ((msubtype == GF_ISOM_SUBTYPE_AVC_H264)
	           || (msubtype == GF_ISOM_SUBTYPE_AVC2_H264)
	           || (msubtype == GF_ISOM_SUBTYPE_AVC3_H264)
	           || (msubtype == GF_ISOM_SUBTYPE_AVC4_H264)
	          ) {
		return gf_isom_set_pl_indication(outfile, GF_ISOM_PL_VISUAL, 0x0F);
	}
	/*likely 3gp or any non-MPEG-4 isomedia file*/
	else if (!esd) return gf_isom_remove_root_od(outfile);

	dur = gf_isom_get_media_duration(outfile, newTk);
	if (!dur) dur = ts;
	esd->decoderConfig->maxBitrate *= 8;
	esd->decoderConfig->avgBitrate = (u32) (esd->decoderConfig->avgBitrate * 8 * ts / dur);
	gf_isom_change_mpeg4_description(outfile, newTk, 1, esd);


	iod = (GF_InitialObjectDescriptor *) gf_isom_get_root_od(infile);
	switch (esd->decoderConfig->streamType) {
	case GF_STREAM_SCENE:
		if (iod && (iod->tag==GF_ODF_IOD_TAG)) {
			gf_isom_set_pl_indication(outfile, GF_ISOM_PL_SCENE, iod->scene_profileAndLevel);
			gf_isom_set_pl_indication(outfile, GF_ISOM_PL_GRAPHICS, iod->graphics_profileAndLevel);
		} else if (esd->decoderConfig->objectTypeIndication==GF_CODECID_MPEG4_PART2) {
			gf_export_message(dumper, GF_OK, "Warning: Scene PLs not found in original MP4 - defaulting to No Profile Specified");
			gf_isom_set_pl_indication(outfile, GF_ISOM_PL_SCENE, 0xFE);
			gf_isom_set_pl_indication(outfile, GF_ISOM_PL_GRAPHICS, 0xFE);
		}
		break;
	case GF_STREAM_VISUAL:
		if (iod && (iod->tag==GF_ODF_IOD_TAG)) {
			gf_isom_set_pl_indication(outfile, GF_ISOM_PL_VISUAL, iod->visual_profileAndLevel);
		}
#ifndef GPAC_DISABLE_AV_PARSERS
		else if (esd->decoderConfig->objectTypeIndication==GF_CODECID_MPEG4_PART2) {
			GF_M4VDecSpecInfo dsi;
			gf_m4v_get_config(esd->decoderConfig->decoderSpecificInfo->data, esd->decoderConfig->decoderSpecificInfo->dataLength, &dsi);
			gf_isom_set_pl_indication(outfile, GF_ISOM_PL_VISUAL, dsi.VideoPL);
		}
#endif
		else {
			gf_export_message(dumper, GF_OK, "Warning: Visual PLs not found in original MP4 - defaulting to No Profile Specified");
			gf_isom_set_pl_indication(outfile, GF_ISOM_PL_VISUAL, 0xFE);
		}
		break;
	case GF_STREAM_AUDIO:
		if (iod && (iod->tag==GF_ODF_IOD_TAG)) {
			gf_isom_set_pl_indication(outfile, GF_ISOM_PL_AUDIO, iod->audio_profileAndLevel);
		}
#ifndef GPAC_DISABLE_AV_PARSERS
		else if (esd->decoderConfig->objectTypeIndication==GF_CODECID_AAC_MPEG4) {
			GF_M4ADecSpecInfo cfg;
			gf_m4a_get_config(esd->decoderConfig->decoderSpecificInfo->data, esd->decoderConfig->decoderSpecificInfo->dataLength, &cfg);
			gf_isom_set_pl_indication(outfile, GF_ISOM_PL_AUDIO, cfg.audioPL);
		}
#endif
		else {
			gf_export_message(dumper, GF_OK, "Warning: Audio PLs not found in original MP4 - defaulting to No Profile Specified");
			gf_isom_set_pl_indication(outfile, GF_ISOM_PL_AUDIO, 0xFE);
		}
	default:
		break;
	}
	if (iod) gf_odf_desc_del((GF_Descriptor *) iod);
	gf_odf_desc_del((GF_Descriptor *)esd);

	if (AddToIOD) gf_isom_add_track_to_root_od(outfile, newTk);

	return GF_OK;
}