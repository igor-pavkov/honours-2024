static GF_Err WriteInterleaved(MovieWriter *mw, GF_BitStream *bs, Bool drift_inter)
{
	GF_Err e;
	u32 i;
	s32 moov_meta_pos=-1;
	GF_Box *a, *cprt_box=NULL;
	u64 firstSize, finalSize, offset, finalOffset;
	GF_List *writers = gf_list_new();
	GF_ISOFile *movie = mw->movie;

	//first setup the writers
	e = SetupWriters(mw, writers, 1);
	if (e) goto exit;


	if (movie->is_jp2) {
		gf_bs_write_u32(bs, 12);
		gf_bs_write_u32(bs, GF_ISOM_BOX_TYPE_JP);
		gf_bs_write_u32(bs, 0x0D0A870A);
	}
	if (movie->brand) {
		e = gf_isom_box_size((GF_Box *)movie->brand);
		if (e) goto exit;
		e = gf_isom_box_write((GF_Box *)movie->brand, bs);
		if (e) goto exit;
	}
	if (movie->pdin) {
		e = gf_isom_box_size((GF_Box *)movie->pdin);
		if (e) goto exit;
		e = gf_isom_box_write((GF_Box *)movie->pdin, bs);
		if (e) goto exit;
	}

	//write all boxes before moov
	i=0;
	while ((a = (GF_Box*)gf_list_enum(movie->TopBoxes, &i))) {
		switch (a->type) {
		case GF_ISOM_BOX_TYPE_MOOV:
		case GF_ISOM_BOX_TYPE_META:
			moov_meta_pos = i-1;
			break;
		case GF_ISOM_BOX_TYPE_FTYP:
		case GF_ISOM_BOX_TYPE_PDIN:
		case GF_ISOM_BOX_TYPE_MDAT:
			break;
		case GF_ISOM_BOX_TYPE_FREE:
			//for backward compat with old arch, keep copyright before moov
			if (((GF_FreeSpaceBox*)a)->dataSize>4) {
				GF_FreeSpaceBox *fr = (GF_FreeSpaceBox*) a;
				if ((fr->dataSize>20) && !strncmp(fr->data, "IsoMedia File", 13)) {
					cprt_box = a;
					break;
				}
			}
		default:
			if (moov_meta_pos<0) {
				e = gf_isom_box_size(a);
				if (e) goto exit;
				e = gf_isom_box_write(a, bs);
				if (e) goto exit;
			}
			break;
		}
	}

	e = DoInterleave(mw, writers, bs, 1, gf_bs_get_position(bs), drift_inter);
	if (e) goto exit;

	firstSize = GetMoovAndMetaSize(movie, writers);
	offset = firstSize;
	if (movie->mdat && movie->mdat->dataSize) offset += 8 + (movie->mdat->dataSize > 0xFFFFFFFF ? 8 : 0);
	e = ShiftOffset(movie, writers, offset);
	if (e) goto exit;
	//get the size and see if it has changed (eg, we moved to 64 bit offsets)
	finalSize = GetMoovAndMetaSize(movie, writers);
	if (firstSize != finalSize) {
		finalOffset = finalSize;
		if (movie->mdat && movie->mdat->dataSize) finalOffset += 8 + (movie->mdat->dataSize > 0xFFFFFFFF ? 8 : 0);
		//OK, now we're sure about the final size -> shift the offsets
		//we don't need to re-emulate, as the only thing that changed is the offset
		//so just shift the offset
		e = ShiftOffset(movie, writers, finalOffset - offset);
		if (e) goto exit;
		/*firstSize = */GetMoovAndMetaSize(movie, writers);
	}
	//get real sample offsets for meta items
	if (movie->meta) {
		store_meta_item_sample_ref_offsets(movie, writers, movie->meta);
	}
	//now write our stuff
	e = WriteMoovAndMeta(movie, writers, bs);
	if (e) goto exit;

	/*we have 8 extra bytes for large size (not computed in gf_isom_box_size) */
	if (movie->mdat && movie->mdat->dataSize) {
		if (movie->mdat->dataSize > 0xFFFFFFFF) movie->mdat->dataSize += 8;
		e = gf_isom_box_size((GF_Box *)movie->mdat);
		if (e) goto exit;
		e = gf_isom_box_write((GF_Box *)movie->mdat, bs);
		if (e) goto exit;
	}

	//we don't need the offset as we are writing...
	ResetWriters(writers);
	e = DoInterleave(mw, writers, bs, 0, 0, drift_inter);
	if (e) goto exit;

	//then the rest
	i=0;
	while ((a = (GF_Box*)gf_list_enum(movie->TopBoxes, &i))) {
		if ((i-1 < (u32) moov_meta_pos) && (a != cprt_box))
			continue;
		switch (a->type) {
		case GF_ISOM_BOX_TYPE_MOOV:
		case GF_ISOM_BOX_TYPE_META:
		case GF_ISOM_BOX_TYPE_FTYP:
		case GF_ISOM_BOX_TYPE_PDIN:
		case GF_ISOM_BOX_TYPE_MDAT:
			break;
		default:
			e = gf_isom_box_size(a);
			if (e) goto exit;
			e = gf_isom_box_write(a, bs);
			if (e) goto exit;
		}
	}

exit:
	CleanWriters(writers);
	gf_list_del(writers);
	return e;
}