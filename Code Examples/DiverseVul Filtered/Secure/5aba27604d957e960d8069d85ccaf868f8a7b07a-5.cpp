static GF_Err WriteFlat(MovieWriter *mw, u8 moovFirst, GF_BitStream *bs, Bool non_seakable, Bool for_fragments, GF_BitStream *moov_bs)
{
	GF_Err e;
	u32 i;
	u64 offset, finalOffset, totSize, begin, firstSize, finalSize;
	GF_Box *a, *cprt_box=NULL;
	GF_List *writers = gf_list_new();
	GF_ISOFile *movie = mw->movie;
	s32 moov_meta_pos=-1;

	//in case we did a read on the file while producing it, seek to end of edit
	totSize = gf_bs_get_size(bs);
	if (gf_bs_get_position(bs) != totSize) {
		gf_bs_seek(bs, totSize);
	}
	begin = totSize = 0;

	//first setup the writers
	e = SetupWriters(mw, writers, 0);
	if (e) goto exit;

	if (!moovFirst) {
		if ((movie->openMode == GF_ISOM_OPEN_WRITE) && !non_seakable) {
			begin = 0;
			totSize = gf_isom_datamap_get_offset(movie->editFileMap);
			/*start boxes have not been written yet, do it*/
			if (!totSize) {
				if (movie->is_jp2) {
					gf_bs_write_u32(movie->editFileMap->bs, 12);
					gf_bs_write_u32(movie->editFileMap->bs, GF_ISOM_BOX_TYPE_JP);
					gf_bs_write_u32(movie->editFileMap->bs, 0x0D0A870A);
					totSize += 12;
					begin += 12;
				}
				if (movie->brand) {
					e = gf_isom_box_size((GF_Box *)movie->brand);
					if (e) goto exit;
					e = gf_isom_box_write((GF_Box *)movie->brand, movie->editFileMap->bs);
					if (e) goto exit;
					totSize += movie->brand->size;
					begin += movie->brand->size;
				}
				if (movie->pdin) {
					e = gf_isom_box_size((GF_Box *)movie->pdin);
					if (e) goto exit;
					e = gf_isom_box_write((GF_Box *)movie->pdin, movie->editFileMap->bs);
					if (e) goto exit;
					totSize += movie->pdin->size;
					begin += movie->pdin->size;
				}
			} else {
				if (movie->is_jp2) begin += 12;
				if (movie->brand) begin += movie->brand->size;
				if (movie->pdin) begin += movie->pdin->size;
			}
			totSize -= begin;
		} else if (!non_seakable || for_fragments) {
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
			/*then progressive download*/
			if (movie->pdin) {
				e = gf_isom_box_size((GF_Box *)movie->pdin);
				if (e) goto exit;
				e = gf_isom_box_write((GF_Box *)movie->pdin, bs);
				if (e) goto exit;
			}
		}

		//if the moov is at the end, write directly
		i=0;
		while ((a = (GF_Box*)gf_list_enum(movie->TopBoxes, &i))) {
			switch (a->type) {
			/*written by hand*/
			case GF_ISOM_BOX_TYPE_MOOV:
			case GF_ISOM_BOX_TYPE_META:
				moov_meta_pos = i-1;
			case GF_ISOM_BOX_TYPE_FTYP:
			case GF_ISOM_BOX_TYPE_PDIN:
#ifndef GPAC_DISABLE_ISOM_ADOBE
			case GF_ISOM_BOX_TYPE_AFRA:
			case GF_ISOM_BOX_TYPE_ABST:
#endif
				break;
			case GF_ISOM_BOX_TYPE_MDAT:
				//in case we're capturing
				if (movie->openMode == GF_ISOM_OPEN_WRITE) {
					//emulate a write to recreate our tables (media data already written)
					e = DoWrite(mw, writers, bs, 1, begin);
					if (e) goto exit;
					continue;
				}
				if (non_seakable) {
					begin = gf_bs_get_position(bs);
					//do a sim pass to get the true mdat size
					e = DoWrite(mw, writers, bs, 1, begin);
					if (e) goto exit;

					if (movie->mdat->dataSize > 0xFFFFFFFF) {
						gf_bs_write_u32(bs, 1);
					} else {
						gf_bs_write_u32(bs, (u32) movie->mdat->dataSize + 8);
					}
					gf_bs_write_u32(bs, GF_ISOM_BOX_TYPE_MDAT);
					if (movie->mdat->dataSize > 0xFFFFFFFF) gf_bs_write_u64(bs, movie->mdat->dataSize + 8 + 8);
					//reset writers and write samples
					ResetWriters(writers);
					e = DoWrite(mw, writers, bs, 0, gf_bs_get_position(bs));
					if (e) goto exit;
					movie->mdat->size = movie->mdat->dataSize;
					totSize = 0;
				} else {
					//to avoid computing the size each time write always 4 + 4 + 8 bytes before
					begin = gf_bs_get_position(bs);
					gf_bs_write_u64(bs, 0);
					gf_bs_write_u64(bs, 0);
					e = DoWrite(mw, writers, bs, 0, gf_bs_get_position(bs));
					if (e) goto exit;
					totSize = gf_bs_get_position(bs) - begin;
				}
				break;

			case GF_ISOM_BOX_TYPE_FREE:
				//for backward compat with old arch, keep copyright before moov
				if (((GF_FreeSpaceBox*)a)->dataSize>4) {
					GF_FreeSpaceBox *fr = (GF_FreeSpaceBox*) a;
					if ((fr->dataSize>20) && !strncmp(fr->data, "IsoMedia File", 13)) {
						e = gf_isom_box_size(a);
						if (e) goto exit;
						e = gf_isom_box_write(a, bs);
						if (e) goto exit;
						cprt_box = a;
						break;
					}
				}
			default:
				if (moov_meta_pos < 0) {
					e = gf_isom_box_size(a);
					if (e) goto exit;
					e = gf_isom_box_write(a, bs);
					if (e) goto exit;
				}
				break;
			}
		}

		if (moov_bs) {
			e = DoWrite(mw, writers, bs, 1, movie->mdat->bsOffset);
			if (e) goto exit;

			firstSize = GetMoovAndMetaSize(movie, writers);

			offset = firstSize;
			e = ShiftOffset(movie, writers, offset);
			if (e) goto exit;
			//get the size and see if it has changed (eg, we moved to 64 bit offsets)
			finalSize = GetMoovAndMetaSize(movie, writers);
			if (firstSize != finalSize) {
				finalOffset = finalSize;
				//OK, now we're sure about the final size.
				//we don't need to re-emulate, as the only thing that changed is the offset
				//so just shift the offset
				e = ShiftOffset(movie, writers, finalOffset - offset);
				if (e) goto exit;
			}
		}
		//get real sample offsets for meta items
		if (movie->meta) {
			store_meta_item_sample_ref_offsets(movie, writers, movie->meta);
		}
		//OK, write the movie box.
		e = WriteMoovAndMeta(movie, writers, moov_bs ? moov_bs : bs);
		if (e) goto exit;

#ifndef GPAC_DISABLE_ISOM_ADOBE
		i=0;
		while ((a = (GF_Box*)gf_list_enum(movie->TopBoxes, &i))) {
			switch (a->type) {
			case GF_ISOM_BOX_TYPE_AFRA:
			case GF_ISOM_BOX_TYPE_ABST:
				e = gf_isom_box_size(a);
				if (e) goto exit;
				e = gf_isom_box_write(a, bs);
				if (e) goto exit;
				break;
			}
		}
#endif

		/*if data has been written, update mdat size*/
		if (totSize) {
			offset = gf_bs_get_position(bs);
			e = gf_bs_seek(bs, begin);
			if (e) goto exit;
			if (totSize > 0xFFFFFFFF) {
				gf_bs_write_u32(bs, 1);
			} else {
				gf_bs_write_u32(bs, (u32) totSize);
			}
			gf_bs_write_u32(bs, GF_ISOM_BOX_TYPE_MDAT);
			if (totSize > 0xFFFFFFFF) gf_bs_write_u64(bs, totSize);
			e = gf_bs_seek(bs, offset);
			movie->mdat->size = totSize;
		}

		//then the rest
		i = (u32) (moov_meta_pos + 1);
		while ((a = (GF_Box*)gf_list_enum(movie->TopBoxes, &i))) {
			if (a==cprt_box) continue;

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
		goto exit;
	}

	//nope, we have to write the moov first. The pb is that
	//1 - we don't know its size till the mdat is written
	//2 - we don't know the ofset at which the mdat will start...
	//3 - once the mdat is written, the chunkOffset table can have changed...

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
	/*then progressive dnload*/
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
		//for backward compat with old arch keep out copyright after moov
		case GF_ISOM_BOX_TYPE_FREE:
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

	//What we will do is first emulate the write from the beginning...
	//note: this will set the size of the mdat
	e = DoWrite(mw, writers, bs, 1, gf_bs_get_position(bs));
	if (e) goto exit;

	firstSize = GetMoovAndMetaSize(movie, writers);
	//offset = (firstSize > 0xFFFFFFFF ? firstSize + 8 : firstSize) + 8 + (movie->mdat->dataSize > 0xFFFFFFFF ? 8 : 0);
	offset = firstSize + 8 + (movie->mdat->dataSize > 0xFFFFFFFF ? 8 : 0);
	e = ShiftOffset(movie, writers, offset);
	if (e) goto exit;
	//get the size and see if it has changed (eg, we moved to 64 bit offsets)
	finalSize = GetMoovAndMetaSize(movie, writers);
	if (firstSize != finalSize) {
		finalOffset = finalSize + 8 + (movie->mdat->dataSize > 0xFFFFFFFF ? 8 : 0);
		//OK, now we're sure about the final size.
		//we don't need to re-emulate, as the only thing that changed is the offset
		//so just shift the offset
		e = ShiftOffset(movie, writers, finalOffset - offset);
		if (e) goto exit;
	}
	//now write our stuff
	e = WriteMoovAndMeta(movie, writers, bs);
	if (e) goto exit;
	e = gf_isom_box_size((GF_Box *)movie->mdat);
	if (e) goto exit;
	e = gf_isom_box_write((GF_Box *)movie->mdat, bs);
	if (e) goto exit;

	//we don't need the offset as the moov is already written...
	ResetWriters(writers);
	e = DoWrite(mw, writers, bs, 0, 0);
	if (e) goto exit;

	//then the rest
	i=0;
	while ((a = (GF_Box*)gf_list_enum(movie->TopBoxes, &i))) {
		if ((i-1<= (u32) moov_meta_pos) && (a!=cprt_box)) continue;
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