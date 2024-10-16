GF_Err gf_media_export_six(GF_MediaExporter *dumper)
{
	GF_ESD *esd;
	char szName[1000], szMedia[1000];
	FILE *media, *six;
	u32 track, i, di, count, pos, header_size;
	//u32 mtype;
#if !defined(GPAC_DISABLE_TTXT) && !defined(GPAC_DISABLE_VTT)
	u32 mstype;
#endif
	const char *szRootName;
	//Bool isText;

	if (!(track = gf_isom_get_track_by_id(dumper->file, dumper->trackID))) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_AUTHOR, ("Wrong track ID %d for file %s \n", dumper->trackID, gf_isom_get_filename(dumper->file)));
		return GF_BAD_PARAM;
	}
	if (!track) return gf_export_message(dumper, GF_BAD_PARAM, "Invalid track ID %d", dumper->trackID);

	if (dumper->flags & GF_EXPORT_PROBE_ONLY) {
		dumper->flags |= GF_EXPORT_NHML_FULL;
		return GF_OK;
	}
	esd = gf_isom_get_esd(dumper->file, track, 1);

	sprintf(szMedia, "%s.media", dumper->out_name);
	media = gf_fopen(szMedia, "wb");
	if (!media) {
		if (esd) gf_odf_desc_del((GF_Descriptor *) esd);
		return gf_export_message(dumper, GF_IO_ERR, "Error opening %s for writing - check disk access & permissions", szMedia);
	}

	sprintf(szName, "%s.six", dumper->out_name);
	szRootName = "stream";

	six = gf_fopen(szName, "wt");
	if (!six) {
		gf_fclose(media);
		if (esd) gf_odf_desc_del((GF_Descriptor *) esd);
		return gf_export_message(dumper, GF_IO_ERR, "Error opening %s for writing - check disk access & permissions", szName);
	}
	/*
		mtype = gf_isom_get_media_type(dumper->file, track);
		if (mtype==GF_ISOM_MEDIA_TEXT || mtype == GF_ISOM_MEDIA_SUBM || mtype == GF_ISOM_MEDIA_SUBT) {
			isText = GF_TRUE;
		} else {
			isText = GF_FALSE;
		}
	*/
#if !defined(GPAC_DISABLE_TTXT) && !defined(GPAC_DISABLE_VTT)
	mstype = gf_isom_get_media_subtype(dumper->file, track, 1);
#endif

	/*write header*/
	gf_fprintf(six, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	gf_fprintf(six, "<%s timescale=\"%d\" ", szRootName, gf_isom_get_media_timescale(dumper->file, track) );
	gf_fprintf(six, "file=\"%s\" ", szMedia);
	gf_fprintf(six, ">\n");
	header_size = 0;
	if (esd) {
		if (esd->decoderConfig->decoderSpecificInfo  && esd->decoderConfig->decoderSpecificInfo->data) {
#if !defined(GPAC_DISABLE_TTXT) && !defined(GPAC_DISABLE_VTT)
			if (mstype == GF_ISOM_SUBTYPE_WVTT || mstype == GF_ISOM_SUBTYPE_STXT) {
				gf_webvtt_dump_header_boxed(media,
				                            esd->decoderConfig->decoderSpecificInfo->data+4,
				                            esd->decoderConfig->decoderSpecificInfo->dataLength,
				                            &header_size);
			} else
#endif
			{
				gf_fwrite(esd->decoderConfig->decoderSpecificInfo->data, esd->decoderConfig->decoderSpecificInfo->dataLength, media);
				header_size = esd->decoderConfig->decoderSpecificInfo->dataLength;
			}
		}
		gf_odf_desc_del((GF_Descriptor *) esd);
	} else {
		GF_GenericSampleDescription *sdesc = gf_isom_get_generic_sample_description(dumper->file, track, 1);
		if (sdesc) {
			header_size = sdesc->extension_buf_size;
			gf_free(sdesc);
		}
	}
	gf_fprintf(six, "<header range-begin=\"0\" range-end=\"%d\"/>\n", header_size-1);

	pos = header_size;
	count = gf_isom_get_sample_count(dumper->file, track);
	for (i=0; i<count; i++) {
		GF_ISOSample *samp = gf_isom_get_sample(dumper->file, track, i+1, &di);
		if (!samp) break;

		if (media) {
			gf_fwrite(samp->data, samp->dataLength, media);
		}

		gf_fprintf(six, "<unit time=\""LLU"\" ", samp->DTS);
		if (samp->IsRAP==RAP) gf_fprintf(six, "rap=\"1\" ");
		else if (samp->IsRAP==RAP_NO) gf_fprintf(six, "rap=\"0\" ");
		gf_fprintf(six, "range-begin=\"%d\" ", pos);
		gf_fprintf(six, "range-end=\"%d\" ", pos+samp->dataLength-1);
		gf_fprintf(six, "/>\n");

		pos += samp->dataLength;
		gf_isom_sample_del(&samp);
		gf_set_progress("SIX Export", i+1, count);
		if (dumper->flags & GF_EXPORT_DO_ABORT) break;
	}
	gf_fprintf(six, "</%s>\n", szRootName);
	if (media) gf_fclose(media);
	gf_fclose(six);
	return GF_OK;

}