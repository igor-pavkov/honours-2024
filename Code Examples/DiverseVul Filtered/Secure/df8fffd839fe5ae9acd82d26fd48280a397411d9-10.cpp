GF_Err NewMedia(GF_MediaBox **mdia, u32 MediaType, u32 TimeScale)
{
	GF_MediaHeaderBox *mdhd;
	GF_Box *mediaInfo;
	GF_HandlerBox *hdlr;
	GF_MediaInformationBox *minf;
	GF_DataInformationBox *dinf;
	GF_SampleTableBox *stbl;
	GF_DataReferenceBox *dref;
	char *str="";

	GF_Err e;

	if (!mdia) return GF_BAD_PARAM;

	minf = *mdia ? (*mdia)->information : NULL;
	mdhd = *mdia ? (*mdia)->mediaHeader : NULL;
	hdlr = *mdia ? (*mdia)->handler : NULL;
	dinf =  minf ? minf->dataInformation : NULL;
	stbl = minf ? minf->sampleTable : NULL;
	dref = dinf ? dinf->dref : NULL;
	mediaInfo = minf ? minf->InfoHeader : NULL;

	//first create the media
	if (!*mdia) {
		*mdia = (GF_MediaBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_MDIA);
		if (! *mdia) { e = GF_OUT_OF_MEM; goto err_exit; }
	}
	if (!mdhd) {
		mdhd = (GF_MediaHeaderBox *) gf_isom_box_new_parent( & ((*mdia)->child_boxes), GF_ISOM_BOX_TYPE_MDHD);
		if (! mdhd) { e = GF_OUT_OF_MEM; goto err_exit; }
		e = mdia_on_child_box((GF_Box*)*mdia, (GF_Box *) mdhd, GF_FALSE);
		if (e) goto err_exit;
	}
	if (!hdlr) {
		hdlr = (GF_HandlerBox *) gf_isom_box_new_parent(& ((*mdia)->child_boxes), GF_ISOM_BOX_TYPE_HDLR);
		if (! hdlr) { e = GF_OUT_OF_MEM; goto err_exit; }
		e = mdia_on_child_box((GF_Box*)*mdia, (GF_Box *) hdlr, GF_FALSE);
		if (e) goto err_exit;
	}
	if (!minf) {
		minf = (GF_MediaInformationBox *) gf_isom_box_new_parent(& ((*mdia)->child_boxes), GF_ISOM_BOX_TYPE_MINF);
		if (! minf) { e = GF_OUT_OF_MEM; goto err_exit; }
		e = mdia_on_child_box((GF_Box*)*mdia, (GF_Box *) minf, GF_FALSE);
		if (e) goto err_exit;
	}
	if (!dinf) {
		dinf = (GF_DataInformationBox *) gf_isom_box_new_parent(&minf->child_boxes, GF_ISOM_BOX_TYPE_DINF);
		if (! dinf) { e = GF_OUT_OF_MEM; goto err_exit; }
		e = minf_on_child_box((GF_Box*)minf, (GF_Box *) dinf, GF_FALSE);
		if (e) goto err_exit;
	}

	if (!mediaInfo) {
		//"handler name" is for debugging purposes. Let's stick our name here ;)
		switch (MediaType) {
		case GF_ISOM_MEDIA_VISUAL:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_VMHD);
			str = "GPAC ISO Video Handler";
			break;
		case GF_ISOM_MEDIA_AUXV:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_VMHD);
			str = "GPAC ISO Auxiliary Video Handler";
			break;
		case GF_ISOM_MEDIA_PICT:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_VMHD);
			str = "GPAC ISO Picture Sequence Handler";
			break;
		case GF_ISOM_MEDIA_AUDIO:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_SMHD);
			str = "GPAC ISO Audio Handler";
			break;
		case GF_ISOM_MEDIA_HINT:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_HMHD);
			str = "GPAC ISO Hint Handler";
			break;
		case GF_ISOM_MEDIA_META:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_NMHD);
			str = "GPAC Timed MetaData Handler";
			break;
		case GF_ISOM_MEDIA_OD:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_NMHD);
			str = "GPAC MPEG-4 OD Handler";
			break;
		case GF_ISOM_MEDIA_OCR:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_NMHD);
			str = "GPAC MPEG-4 OCR Handler";
			break;
		case GF_ISOM_MEDIA_SCENE:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_NMHD);
			str = "GPAC MPEG-4 Scene Description Handler";
			break;
		case GF_ISOM_MEDIA_MPEG7:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_NMHD);
			str = "GPAC MPEG-4 MPEG-7 Handler";
			break;
		case GF_ISOM_MEDIA_OCI:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_NMHD);
			str = "GPAC MPEG-4 OCI Handler";
			break;
		case GF_ISOM_MEDIA_IPMP:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_NMHD);
			str = "GPAC MPEG-4 IPMP Handler";
			break;
		case GF_ISOM_MEDIA_MPEGJ:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_NMHD);
			str = "GPAC MPEG-4 MPEG-J Handler";
			break;
		case GF_ISOM_MEDIA_TEXT:
		case GF_ISOM_MEDIA_SUBT:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_NMHD);
			str = "GPAC Streaming Text Handler";
			break;
		case GF_ISOM_MEDIA_MPEG_SUBT:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_STHD);
			str = "GPAC MPEG Subtitle Handler";
			break;
		case GF_ISOM_MEDIA_DIMS:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_VMHD);
			MediaType = GF_ISOM_MEDIA_SCENE;
			str = "GPAC DIMS Handler";
			break;
		case GF_ISOM_MEDIA_TIMECODE:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_GMHD);
			str = "GPAC TMCD Handler";
			break;
		default:
			mediaInfo = gf_isom_box_new(GF_ISOM_BOX_TYPE_NMHD);
			str = "GPAC IsoMedia Handler";
			break;
		}
		if (! mediaInfo) { e = GF_OUT_OF_MEM; goto err_exit; }
		if (!minf->child_boxes) minf->child_boxes = gf_list_new();
		gf_list_add(minf->child_boxes, mediaInfo);

		e = minf_on_child_box((GF_Box*)minf, (GF_Box *) mediaInfo, GF_FALSE);
		if (e) goto err_exit;
	}

	mdhd->timeScale = TimeScale;
	hdlr->handlerType = MediaType;
	if (!hdlr->nameUTF8)
		hdlr->nameUTF8 = gf_strdup(str);

	if (!dref) {
		//Create a data reference WITHOUT DATA ENTRY (we don't know anything yet about the media Data)
		dref = (GF_DataReferenceBox *) gf_isom_box_new_parent(&dinf->child_boxes, GF_ISOM_BOX_TYPE_DREF);
		if (! dref) { e = GF_OUT_OF_MEM; goto err_exit; }
		e = dinf_on_child_box((GF_Box*)dinf, (GF_Box *)dref, GF_FALSE);
		if (e) goto err_exit;
	}

	if (!stbl) {
		//first set-up the sample table...
		stbl = (GF_SampleTableBox *) gf_isom_box_new_parent(&minf->child_boxes, GF_ISOM_BOX_TYPE_STBL);
		if (! stbl) { e = GF_OUT_OF_MEM; goto err_exit; }

		e = minf_on_child_box((GF_Box*)minf, (GF_Box *) stbl, GF_FALSE);
		if (e) goto err_exit;
	}
	if (!stbl->SampleDescription) {
		stbl->SampleDescription = (GF_SampleDescriptionBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSD);
		if (! stbl->SampleDescription) { e = GF_OUT_OF_MEM; goto err_exit; }
	}

	//by default create a regular table, 32 but offset and normal sample size
	if (!stbl->ChunkOffset) {
		stbl->ChunkOffset = gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STCO);
		if (! stbl->ChunkOffset) { e = GF_OUT_OF_MEM; goto err_exit; }
	}
	if (!stbl->SampleSize) {
		stbl->SampleSize = (GF_SampleSizeBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSZ);
		if (! stbl->SampleSize) { e = GF_OUT_OF_MEM; goto err_exit; }
	}
	if (!stbl->SampleToChunk) {
		stbl->SampleToChunk = (GF_SampleToChunkBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSC);
		if (! stbl->SampleToChunk) { e = GF_OUT_OF_MEM; goto err_exit; }
	}
	if (!stbl->TimeToSample) {
		stbl->TimeToSample = (GF_TimeToSampleBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STTS);
		if (! stbl->TimeToSample) { e = GF_OUT_OF_MEM; goto err_exit; }
	}
	if (!stbl->SampleDescription) {
		stbl->SampleDescription = (GF_SampleDescriptionBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSD);
		if (! stbl->SampleDescription) { e = GF_OUT_OF_MEM; goto err_exit; }
	}
	return GF_OK;

err_exit:
	if (mdhd) gf_isom_box_del_parent(& ((*mdia)->child_boxes), (GF_Box *)mdhd);
	if (minf) gf_isom_box_del_parent(& ((*mdia)->child_boxes), (GF_Box *)minf);
	if (hdlr) {
		gf_isom_box_del_parent(& ((*mdia)->child_boxes) , (GF_Box *)hdlr);
	}
	return e;

}